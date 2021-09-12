// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHResource.h"
#include "GHMath/GHRect.h"
#include <map>

class GHMaterial;

// A class to store the source info for rendering text.
class GHFont : public GHResource
{
public:
	GHFont(GHMaterial* fontMaterial);
	~GHFont(void);
    
	void setLetterPosition(const char keyChar, const GHRect<float, 2>& charPos);
	bool getRectForLetter(const char letter, const GHRect<float, 2>*& ret, float& retWidth) const;
    GHMaterial* getMaterial(void) { return mMaterial; }
    
    // font editor functions
    void debugPrint(void) const;
    void offsetAll(const GHPoint2& minOffset, const GHPoint2& maxOffset);
    void offsetLetter(char c, const GHPoint2& minOffset, const GHPoint2& maxOffset);
    
private:
	struct LetterInfo
	{
		GHRect<float, 2> mPosition;
		float mWidthRatio;
	};
	// uv coords for various letters within the font.
	std::map<char, LetterInfo > mLetterPositions;
	// material that has the letters on it.
	GHMaterial* mMaterial;

private:
    void calcWidthRatio(LetterInfo& letterInfo);
};
