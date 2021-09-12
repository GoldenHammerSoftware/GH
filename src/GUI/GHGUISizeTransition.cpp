// Copyright Golden Hammer Software
#include "GHGUISizeTransition.h"
#include "GHGUIWidget.h"
#include "GHUtils/GHAlign.h"

GHGUISizeTransition::GHGUISizeTransition(GHGUIWidget& parent, float sizeMult)
: mParent(parent)
, mSizeMult(sizeMult)
{
}

void GHGUISizeTransition::activate(void)
{
	GHGUIPosDesc pos = mParent.getPosDesc();
	mOffsetChange = pos.mSize;
	pos.mSize *= mSizeMult;

	mOffsetChange -= pos.mSize;
	for (int i = 0; i < 2; ++i)
	{
		if (pos.mSizeAlign[i] == GHAlign::A_LEFT) mOffsetChange[i] /= 2.0f;
		else if (pos.mSizeAlign[i] == GHAlign::A_RIGHT) mOffsetChange[i] /= -2.0f;
		else if (pos.mSizeAlign[i] == GHAlign::A_TOP) mOffsetChange[i] /= 2.0f;
		else if (pos.mSizeAlign[i] == GHAlign::A_BOTTOM) mOffsetChange[i] /= -2.0f;
		else if (pos.mSizeAlign[i] == GHAlign::A_CENTER) mOffsetChange[i] = 0;
	}
	
	pos.mOffset -= mOffsetChange;

	mParent.setPosDesc(pos);
	mParent.updatePosition();
}

void GHGUISizeTransition::deactivate(void)
{
	GHGUIPosDesc pos = mParent.getPosDesc();
	pos.mSize /= mSizeMult;
	pos.mOffset += mOffsetChange;
	mParent.setPosDesc(pos);
	mParent.updatePosition();
}
