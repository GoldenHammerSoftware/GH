// Copyright Golden Hammer Software
#include "GHFontRenderer.h"
#include "GHVBFactory.h"
#include "GHFont.h"
#include "GHPlatform/GHDebugMessage.h"

#include "GHVBBlitter.h"
#include "GHVBDescription.h"
#include "GHVertexBuffer.h"
#include "GHIndexBufferUtil.h"
#include "GHVertexBufferUtil.h"
#include "GHMath/GHColor.h"

GHFontRenderer::GHFontRenderer(const GHVBFactory& vbFactory)
: mVBFactory(vbFactory)
{
}


//2D text: guivertcolorvertex, onetexturevertcolorpixel
//	struct VertexShaderInput
//	{
//		float4 pos : POSITION;
//		float4 diffuse : DIFFUSE;
//		float2 uv : UV;
//	};

//3D text: font3Dvertex, onetexturevertcolorpixel
//	struct VertexShaderInput
//	{
//		float3 pos : POSITION;
//		float4 diffuse : DIFFUSE;
//		float2 uv : UV;
//	};

GHVertexBuffer* GHFontRenderer::createText(const GHFont& font, const char* text, 
                                           float charHeight, float aspectRatio, bool wrap,
                                           GHAlign::Enum vAlign, GHAlign::Enum hAlign,
                                           const GHPoint2& minPoint, const GHPoint2& maxPoint,
                                           GHVertexBuffer* oldVB) const
{
    TextGroup group;
    createTextSegments(group, font, text, charHeight, aspectRatio);
	
	if (wrap) {
		separateIntoLines(group, minPoint, maxPoint);
	}

	int numLetters = 0;
	TextLineList::iterator lineIter = group.mLines.begin(), lineIterEnd = group.mLines.end();
	for (; lineIter != lineIterEnd; ++lineIter)
	{
		TextLine& line = *lineIter;
		numLetters += line.countLetters();
	}

    GHVertexBuffer* vb = oldVB;
    if (!oldVB || 
        oldVB->getBlitter()->get()->getType() != GHVBBlitter::BT_INDEX ||
        ((GHVBBlitterIndex*)oldVB->getBlitter()->get())->getNumIndices() != numLetters*6)
    {
        // todo: re-use the buffer if it is big enough to hold the letters.
        vb = createEmptyVB(numLetters);
    }
    if (!vb) return 0;

    GHVertexStreamPtr* streamPtr = vb->getStream(0);
    GHVertexStream* stream = streamPtr->get();
    float* streamVerts = stream->lockWriteBuffer();

    fillUVandColor(*vb, group, streamVerts); 
    fillPosition(*vb, group, charHeight, minPoint, maxPoint, vAlign, hAlign, streamVerts);
    
    stream->unlockWriteBuffer();    
    return vb;
}

void GHFontRenderer::createTextSegments(TextGroup& ret, const GHFont& font, 
                                        const char* text, float charHeight, float aspectRatio) const
{
	//Everything will be on the same line by the end of this function.
	// Another function will separate the text into separate lines.
	ret.mLines.resize(1);

    const char* currChar = text;
	char dummyChar[2];
	dummyChar[1] = '\0';

	//Go through entire string
    while (currChar && *currChar != '\0')
    {
        TextSegment textSegment;
		//figure out the color
        if (*currChar == '^')
        {
			//determine outline status (^^999000 as opposed to ^999)
            bool hasOutline = false;
            currChar++;
			if (!currChar || *currChar == '\0') {
				GHDebugMessage::outputString("Invalid color format");
				break;
			}
            if (*currChar == '^') {
                hasOutline = true;
                currChar++;
            }
            
			//read the three color characters
            dummyChar[0] = *currChar;
            textSegment.mColor[0] = (float)::atof(dummyChar) / 9.0f;
            currChar++; 
			if (!currChar || *currChar == '\0') {
				GHDebugMessage::outputString("Invalid color format");
				break;
			}
            dummyChar[0] = *currChar;
            textSegment.mColor[1] = (float)::atof(dummyChar) / 9.0f;
            currChar++; 
			if (!currChar || *currChar == '\0') {
				GHDebugMessage::outputString("Invalid color format");
				break;
			}
            dummyChar[0] = *currChar;
            textSegment.mColor[2] = (float)::atof(dummyChar) / 9.0f;
            currChar++;
            
			//read the three outline color characters
            if (hasOutline)
            {
                dummyChar[0] = *currChar;
                textSegment.mOutlineColor[0] = (float)::atof(dummyChar) / 9.0f;
                currChar++;
                if (!currChar || *currChar == '\0') {
                    GHDebugMessage::outputString("Invalid color format");
                    break;
                }
                dummyChar[0] = *currChar;
                textSegment.mOutlineColor[1] = (float)::atof(dummyChar) / 9.0f;
                currChar++;
                if (!currChar || *currChar == '\0') {
                    GHDebugMessage::outputString("Invalid color format");
                    break;
                }
                dummyChar[0] = *currChar;
                textSegment.mOutlineColor[2] = (float)::atof(dummyChar) / 9.0f;
                currChar++;
            }
            else
            {
                textSegment.mOutlineColor[0] = 0;
                textSegment.mOutlineColor[1] = 0;
                textSegment.mOutlineColor[2] = 0;
            }
        } //end figure out color
        
		//after determining the color of the entry, extract the letters
        while (currChar && *currChar != '\0')
        {
            LetterEntry letterEntry;
            letterEntry.mChar = *currChar;
            const GHRect<float, 2>* srcUV = 0;
            if (font.getRectForLetter(letterEntry.mChar, srcUV, letterEntry.mWidth))
            {
                letterEntry.mUV = *srcUV;
                letterEntry.mWidth /= aspectRatio;
                letterEntry.mWidth *= charHeight;
                textSegment.mLetters.push_back(letterEntry);
            }
            ++currChar;
            //stop extracting letters if we hit another color specifier
			if (currChar && *currChar == '^') break;
        }

        ret.mLines.front().mSegments.push_back(textSegment);
    } //end loop through entire string
}

void GHFontRenderer::separateIntoLines(TextGroup& ret, const GHPoint2& minPoint, const GHPoint2& maxPoint) const
{
	float maxWidth = maxPoint[0] - minPoint[0];
	TextLine& line = ret.mLines.back();

	//count letters until we reach maxWidth
	TextSegmentList::iterator overflowSegmentIter;
	int overflowLetterIndex;
	if (findOverflow(line, maxWidth, overflowSegmentIter, overflowLetterIndex))
	{
		//then count backwards until we can wrap
		TextSegmentList::iterator wrapSegmentIter;
		int wrapLetterIndex;
		findWrapPoint(line, overflowSegmentIter, overflowLetterIndex, wrapSegmentIter, wrapLetterIndex);

		ret.mLines.push_back(TextLine());
		TextLine& newLine = ret.mLines.back();

		TextSegment& wrapSegment = *wrapSegmentIter;
		//If our wrap point is in the middle of a segment, then break the segment up
		// into two segments that can each be on a different line
		if (wrapLetterIndex > 0)
		{
			newLine.mSegments.push_back(TextSegment());
			TextSegment& newSegment = newLine.mSegments.back();
			newSegment.mColor = wrapSegment.mColor;
			newSegment.mOutlineColor = wrapSegment.mOutlineColor;
			newSegment.mLetters.resize(wrapSegment.mLetters.size() - wrapLetterIndex);
			std::copy(wrapSegment.mLetters.begin() + wrapLetterIndex, wrapSegment.mLetters.end(), newSegment.mLetters.begin());
			wrapSegment.mLetters.erase(wrapSegment.mLetters.begin() + wrapLetterIndex, wrapSegment.mLetters.end());
		}

		//and then move the remaining segments onto the next line
		TextSegmentList::iterator nextSegment = wrapSegmentIter;
		for (++nextSegment; nextSegment != line.mSegments.end(); ++nextSegment)
		{
			newLine.mSegments.push_back(*nextSegment);
		}
		nextSegment = wrapSegmentIter;
		for (++nextSegment; nextSegment != line.mSegments.end(); nextSegment = line.mSegments.erase(nextSegment))
		{
			;
		}

		/*
		int numSegmentsToCopy = line.mSegments.size() - wrapSegmentIndex;
		newLine.mSegments.resize(numSegmentsToCopy);
		for (size_t i = 0; i < numSegmentsToCopy; ++i)
		{
			newLine.mSegments[i] = line.mSegments[wrapSegmentIndex + i];
		}
		line.mSegments.erase(line.mSegments.begin() + wrapSegmentIndex, line.mSegments.end());
		*/

		//We have successfully isolated a line. Now do it again recursively for the next line
		separateIntoLines(ret, minPoint, maxPoint);
	}

	//Our final line has been processed. It's ok to return now.
}

bool GHFontRenderer::findOverflow(TextLine& line, float maxWidth, TextSegmentList::iterator& outOverflowSegment, int& outOverflowLetter) const
{
	TextSegmentList::iterator segmentIter = line.mSegments.begin(), segmentIterEnd = line.mSegments.end();
	for (; segmentIter != segmentIterEnd; ++segmentIter)
	{
		const TextSegment& segment = *segmentIter;
		size_t numLetters = segment.mLetters.size();
		for (size_t letterIndex = 0; letterIndex < numLetters; ++letterIndex)
		{
			const LetterEntry& letter = segment.mLetters[letterIndex];
			maxWidth -= letter.mWidth;
			if (maxWidth < 0) 
			{
				outOverflowSegment = segmentIter;
				outOverflowLetter = (int)letterIndex;
				return true;
			}
		}
	}
	return false;
}

bool GHFontRenderer::TextLine::getPreviousLetter(const TextSegmentList::iterator& inSegment, int inLetter, TextSegmentList::iterator& outSegment, int& outLetter) const
{
	outSegment = inSegment;
	outLetter = inLetter - 1;
	if (outLetter < 0) 
	{
		do
		{
			if (outSegment == mSegments.begin())
			{
				return false;
			}
			--outSegment;
			const TextSegment& segment = *outSegment;
			outLetter = (int)segment.mLetters.size();
			if (outLetter > 0)
			{
				--outLetter;
				return true;
			}
		}while(true);
	}
	
	//If we only decremented a letter without changing segments
	return true;
}

void GHFontRenderer::findWrapPoint(const TextLine& line, TextSegmentList::iterator& overflowSegment, int overflowLetter, TextSegmentList::iterator& outWrapSegment, int& outWrapLetter) const
{
	TextSegmentList::iterator currSegment = overflowSegment;
	int currLetter = overflowLetter;
	TextSegmentList::iterator prevSegment;
	int prevLetter;
	while (line.getPreviousLetter(currSegment, currLetter, prevSegment, prevLetter))
	{
		const TextSegment& segment = *prevSegment;
		const LetterEntry& letter = segment.mLetters[prevLetter];
		if (letter.mChar == ' ')
		{
			outWrapSegment = currSegment;
			outWrapLetter = currLetter;
			return;
		}
		currSegment = prevSegment;
		currLetter = prevLetter;
	}
	
	//failure case: the line doesn't have any easy whitespace breaks
	outWrapSegment = overflowSegment;
	outWrapLetter = overflowLetter;
}

int GHFontRenderer::TextLine::countLetters(void) const
{
    int ret = 0;
	TextSegmentList::const_iterator iter = mSegments.begin(), iterEnd = mSegments.end();
	for (; iter != iterEnd; ++iter)
    {
        ret += (int)iter->mLetters.size();
    }
    return ret;
}

float GHFontRenderer::TextLine::countWidth(void) const
{
    float ret = 0;
	TextSegmentList::const_iterator iter = mSegments.begin(), iterEnd = mSegments.end();
	for (; iter != iterEnd; ++iter)
    {
		const TextSegment& segment = *iter;
        for (size_t x = 0; x < segment.mLetters.size(); ++x)
        {
            ret += segment.mLetters[x].mWidth;
        }
    }
    return ret;
}

GHVertexBuffer* GHFontRenderer::createEmptyVB(int numLetters) const
{
	if (numLetters < 1) return 0;

    GHVBDescription vbdesc(4*numLetters, 6*numLetters, GHVBBlitter::BT_INDEX);
    GHVBDescription::StreamDescription streamDesc;
    streamDesc.mIsShared = true;
    streamDesc.mUsage = GHVBUsage::DYNAMIC;
    streamDesc.mComps.push_back(GHVertexComponent::AP_POS2D);
    streamDesc.mComps.push_back(GHVertexComponent::AP_UV1);
    streamDesc.mComps.push_back(GHVertexComponent::AP_DIFFUSE);
	streamDesc.mComps.push_back(GHVertexComponent::AP_SPECULAR);
    vbdesc.addStreamDescription(streamDesc);
    GHVertexBuffer* ret = mVBFactory.createVB(vbdesc);

    GHVBBlitterPtr* blitterPtr = ret->getBlitter();
    GHVBBlitterIndex* indexBlitter = (GHVBBlitterIndex*)blitterPtr->get();
    GHIndexBufferUtil::fillIndexBufferForQuads(*indexBlitter, numLetters);

    return ret;
}

void GHFontRenderer::fillUVandColor(GHVertexBuffer& vb, const TextGroup& segments, float* streamVerts) const
{
    GHVertexStreamPtr* streamPtr = vb.getStream(0);
    GHVertexStream* stream = streamPtr->get();
    const GHVertexStreamFormat& streamFormat = stream->getFormat();
    const GHVertexStreamFormat::ComponentEntry* colorCompEntry = streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_DIFFUSE);
	const GHVertexStreamFormat::ComponentEntry* outlineCompEntry = streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_SPECULAR);

	TextLineList::const_iterator lineIter = segments.mLines.begin(), lineIterEnd = segments.mLines.end();
	for (; lineIter != lineIterEnd; ++lineIter)
	{
		const TextLine& currLine = *lineIter;
		TextSegmentList::const_iterator segIter = currLine.mSegments.begin(), segIterEnd = currLine.mSegments.end();
		for (; segIter != segIterEnd; ++segIter)
		{
			const TextSegment& currSeg = *segIter;
			int32_t textColorDword = GHColor::convertToDword(currSeg.mColor);
			int32_t outlineColorDword = GHColor::convertToDword(currSeg.mOutlineColor);

			for (size_t letterId = 0; letterId < currSeg.mLetters.size(); ++letterId)
			{
				const LetterEntry& currLetter = currSeg.mLetters[letterId];

				GHVertexBufferUtil::fillUVRect(streamVerts, streamFormat,
											   currLetter.mUV.mMin, currLetter.mUV.mMax);
				GHVertexBufferUtil::fillColors(streamVerts, streamFormat, *colorCompEntry, 4, textColorDword);
				GHVertexBufferUtil::fillColors(streamVerts, streamFormat, *outlineCompEntry, 4, outlineColorDword);

				streamVerts += (4 * streamFormat.getVertexSize());
			}
		}
	}
}

void GHFontRenderer::fillPosition(GHVertexBuffer& vb, 
                                  const TextGroup& segments,
                                  float charHeight,
                                  const GHPoint2& minPoint, const GHPoint2& maxPoint,
								  GHAlign::Enum vAlign, GHAlign::Enum hAlign,
                                  float* streamVerts) const
{
    GHVertexStreamPtr* streamPtr = vb.getStream(0);
    GHVertexStream* stream = streamPtr->get();
    const GHVertexStreamFormat& streamFormat = stream->getFormat();

	float vStart = calcVerticalStart(segments, charHeight,
									 minPoint, maxPoint, vAlign);
	TextLineList::const_iterator lineIter = segments.mLines.begin(), lineIterEnd = segments.mLines.end();
	for (; lineIter != lineIterEnd; ++lineIter)
	{
		const TextLine& currLine = *lineIter;
		float hStart = calcHorizontalStart(currLine, charHeight,
										   minPoint, maxPoint, hAlign);
		TextSegmentList::const_iterator segIter = currLine.mSegments.begin(), segIterEnd = currLine.mSegments.end();
		for (; segIter != segIterEnd; ++segIter)
		{
			const TextSegment& currSeg = *segIter;

			for (size_t letterId = 0; letterId < currSeg.mLetters.size(); ++letterId)
			{
				const LetterEntry& currLetter = currSeg.mLetters[letterId];

				GHPoint2 letterMin(hStart, vStart);
				GHPoint2 letterMax(hStart + currLetter.mWidth,
								   vStart + charHeight);

				// clamp to screen edge.
				// todo? make this an option.
				for (size_t clampIdx = 0; clampIdx < 2; ++clampIdx)
				{
					if (letterMin[clampIdx] < 0.0f) letterMin[clampIdx] = 0.0f;
					if (letterMin[clampIdx] > 1.0f) letterMin[clampIdx] = 1.0f;
					if (letterMax[clampIdx] < 0.0f) letterMax[clampIdx] = 0.0f;
					if (letterMax[clampIdx] > 1.0f) letterMax[clampIdx] = 1.0f;
				}

				GHVertexBufferUtil::fillRect2D(streamVerts, streamFormat, letterMin, letterMax);

				streamVerts += (4 * streamFormat.getVertexSize());
				hStart += currLetter.mWidth;
			}
		}
		vStart += charHeight;
	}
}

float GHFontRenderer::calcVerticalStart(const TextGroup& segments, float charHeight,
										const GHPoint2& minPoint, const GHPoint2& maxPoint,
										GHAlign::Enum vAlign) const
{
	float ret;

	size_t numLines = segments.mLines.size();

	if (vAlign == GHAlign::A_TOP) ret = minPoint[1];
	else if (vAlign == GHAlign::A_BOTTOM) ret = maxPoint[1] - (charHeight * numLines);
	else {
		ret = minPoint[1] + (maxPoint[1] - minPoint[1]) / 2.0f - (charHeight * numLines) / 2.0f;
	}

	return ret;
}

float GHFontRenderer::calcHorizontalStart(const TextLine& segments, float charHeight,
										  const GHPoint2& minPoint, const GHPoint2& maxPoint,
										  GHAlign::Enum hAlign) const
{
	float ret;
	float width = segments.countWidth();
	if (hAlign == GHAlign::A_LEFT) ret = minPoint[0];
	else if (hAlign == GHAlign::A_RIGHT) ret = maxPoint[0] - width;
	else {
		ret = minPoint[0] + (maxPoint[0] - minPoint[0]) / 2.0f - width / 2.0f;
	}

	return ret;
}

