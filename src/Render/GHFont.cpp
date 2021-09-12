// Copyright Golden Hammer Software
#include "GHFont.h"
#include "GHMaterial.h"
#include "GHPlatform/GHDebugMessage.h"

GHFont::GHFont(GHMaterial* fontMaterial)
: mMaterial(0)
{
    GHRefCounted::changePointer((GHRefCounted*&)mMaterial, fontMaterial);
}

GHFont::~GHFont(void)
{
    GHRefCounted::changePointer((GHRefCounted*&)mMaterial, 0);
}

void GHFont::setLetterPosition(const char keyChar, const GHRect<float, 2>& charPos)
{
	mLetterPositions[keyChar].mPosition = charPos;
    calcWidthRatio(mLetterPositions[keyChar]);
}

void GHFont::calcWidthRatio(LetterInfo& letterInfo)
{
    float width = letterInfo.mPosition.mMax[0] - letterInfo.mPosition.mMin[0];
    float height = letterInfo.mPosition.mMax[1] - letterInfo.mPosition.mMin[1];
    if (width == 0 || height == 0) letterInfo.mWidthRatio = 0.4f;
    else letterInfo.mWidthRatio = width / height;
}

bool GHFont::getRectForLetter(const char letter, const GHRect<float, 2>*& ret, float& retWidth) const
{
	std::map<char, LetterInfo >::const_iterator findIter = mLetterPositions.find(letter);
	if (findIter == mLetterPositions.end()) {
		retWidth = 0.2f;
		return false;
	}
	ret = &(findIter->second.mPosition);
	retWidth = findIter->second.mWidthRatio;
	return true; 
}

void GHFont::debugPrint(void) const 
{
    std::map<char, LetterInfo >::const_iterator iter;
    for (iter = mLetterPositions.begin(); iter != mLetterPositions.end(); ++iter)
    {
        //<letter char=1 min="0.007 0.008" max="0.0349 0.082"/>
        GHDebugMessage::outputString("<letter char=\"%c\" min=\"%f %f\" max=\"%f %f\"/>",
                                     iter->first,
                                     iter->second.mPosition.mMin[0], iter->second.mPosition.mMin[1],
                                     iter->second.mPosition.mMax[0], iter->second.mPosition.mMax[1]);
                                     
    }
}

void GHFont::offsetAll(const GHPoint2& minOffset, const GHPoint2& maxOffset)
{
    std::map<char, LetterInfo >::const_iterator iter;
    for (iter = mLetterPositions.begin(); iter != mLetterPositions.end(); ++iter)
    {
        offsetLetter(iter->first, minOffset, maxOffset);
    }
}

void GHFont::offsetLetter(char c, const GHPoint2& minOffset, const GHPoint2& maxOffset)
{
    std::map<char, LetterInfo >::iterator iter;
    iter = mLetterPositions.find(c);
    if (iter == mLetterPositions.end()) return;
    iter->second.mPosition.mMin += minOffset;
    iter->second.mPosition.mMax += maxOffset;
    calcWidthRatio(iter->second);
}
