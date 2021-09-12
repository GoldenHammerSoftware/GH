// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHAlign.h"
#include "GHMath/GHPoint.h"
#include "GHString/GHString.h"
#include "GHMath/GHRect.h"
#include <vector>
#include <list>

class GHVBFactory;
class GHFont;
class GHVertexBuffer;

// A util class to generate vertex buffers for letters.
class GHFontRenderer
{
public:
    GHFontRenderer(const GHVBFactory& vbFactory);
 
    GHVertexBuffer* createText(const GHFont& font, const char* text, float charHeight, float aspectRatio, bool wrap,
                               GHAlign::Enum vAlign, GHAlign::Enum hAlign,
                               const GHPoint2& minPoint, const GHPoint2& maxPoint,
                               GHVertexBuffer* oldVB) const;
    
private:
    class LetterEntry
    {
    public:
		char mChar{ 0 };
        GHRect<float, 2> mUV;
		float mWidth{ 0 };
    };
    class TextSegment
    {
    public:
        TextSegment(void) : mColor(1,1,1,1), mOutlineColor(0,0,0,0) {}
        
    public:
        std::vector<LetterEntry> mLetters;
        GHPoint4 mColor;
		GHPoint4 mOutlineColor;
    };
    typedef std::list<TextSegment> TextSegmentList;
    class TextLine
    {
    public:
        int countLetters(void) const;
        float countWidth(void) const;

		bool getPreviousLetter(const TextSegmentList::iterator& inSegment, int inLetter, TextSegmentList::iterator& outSegment, int& outLetter) const;
        
    public:
        TextSegmentList mSegments;
    };
	typedef std::list<TextLine> TextLineList;
	class TextGroup
	{
	public:

		TextLineList mLines;
	};

private:
    // take a char* and break it into letter entries for pushing into a vb
    // also filters out ^999 color codes and stores those out.
    void createTextSegments(TextGroup& ret, const GHFont& font, 
                            const char* text, float charHeight, float aspectRatio) const;

	//Take a set of text segments in one line and break it
	// up into multiple lines
	void separateIntoLines(TextGroup& ret, const GHPoint2& minPoint, const GHPoint2& maxPoint) const;

	//Find the letter in the line that overflows the max width.
	// Return false if the all the text fits on the line.
	bool findOverflow(TextLine& line, float maxWidth, TextSegmentList::iterator& outOverflowSegment, int& outOverflowLetter) const;

	void findWrapPoint(const TextLine& line, TextSegmentList::iterator& overflowSegment, int overflowLetter, TextSegmentList::iterator& outWrapSegment, int& outWrapLetter) const;

	// create an empty vb of the right size to hold numLetters.
    GHVertexBuffer* createEmptyVB(int numLetters) const;
    // copy the uv coords and color from segments into the vb.
    void fillUVandColor(GHVertexBuffer& vb, const TextGroup& segments, float* streamVerts) const;
    // put each letter in the right place on the screen.
    void fillPosition(GHVertexBuffer& vb, const TextGroup& segments, 
                      float charHeight, const GHPoint2& minPoint, const GHPoint2& maxPoint,
					  GHAlign::Enum vAlign, GHAlign::Enum hAlign, float* streamVerts) const;
 
	float calcVerticalStart(const TextGroup& segments, float charHeight,
							const GHPoint2& minPoint, const GHPoint2& maxPoint,
							GHAlign::Enum vAlign) const;

	float calcHorizontalStart(const TextLine& segments, float charHeight,
							  const GHPoint2& minPoint, const GHPoint2& maxPoint,
							  GHAlign::Enum hAlign) const;

private:
    const GHVBFactory& mVBFactory;
};
