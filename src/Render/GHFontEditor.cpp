// Copyright Golden Hammer Software
#include "GHFontEditor.h"
#include "Base/GHInputState.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHFont.h"
#include "Base/GHKeyDef.h"
#include "GHUtils/GHMessage.h"
#include "Base/GHMessageTypes.h"
#include "GHUtils/GHMessageHandler.h"
#include "Base/GHCharUtil.h"

GHFontEditor::GHFontEditor(const GHInputState& inputState, GHFont& font, GHMessageHandler& messageHandler)
: mInputState(inputState)
, mFont(font)
, mSelectLetter(false)
, mSelectedLetter('1')
, mMessageHandler(messageHandler)
{
}

void GHFontEditor::update(void)
{
    bool letterChanged = false;
    
	float moveAmount = 0.001f;
	if (mInputState.getKeyState(0, GHKeyDef::KEY_SHIFT)) {
		moveAmount = 0.01f;
	}

    if (mSelectLetter)
    {
        // next char is our selected letter
        const GHInputStructs::InputEventList& eventList = mInputState.getInputEvents();
        GHInputStructs::InputEventList::const_iterator iter;
        for (iter = eventList.begin(); iter != eventList.end(); ++iter)
        {
            if ((*iter).mType == GHInputStructs::IET_KEYCHANGE)
            {
                char c = (char)((*iter).mVal[0]);
                if (c == GHKeyDef::KEY_SHIFT) {
                    continue;
                }
				bool shiftHeld = (mInputState.getKeyState(0, GHKeyDef::KEY_SHIFT) != 0);
				c = GHCharUtil::applyUpperCase((GHKeyDef::Enum)c, shiftHeld);
				mSelectedLetter = c;
				GHDebugMessage::outputString("Selected %c for editing", mSelectedLetter);
                mSelectLetter = false;
                break;
            }
        }
		return;
    }
    else if (mInputState.checkKeyChange(0, '`', false))
    {
        mSelectLetter = true;
    }
    else if (mInputState.checkKeyChange(0, 'p', false))
    {
        mFont.debugPrint();
    }
    
    else if (mInputState.checkKeyChange(0, GHKeyDef::KEY_RIGHTARROW, false))
    {
		mFont.offsetLetter(mSelectedLetter, GHPoint2(moveAmount, 0), GHPoint2(0, 0));
        letterChanged = true;
    }
    else if (mInputState.checkKeyChange(0, GHKeyDef::KEY_LEFTARROW, false))
    {
		mFont.offsetLetter(mSelectedLetter, GHPoint2(-moveAmount, 0), GHPoint2(0, 0));
        letterChanged = true;
    }
    else if (mInputState.checkKeyChange(0, GHKeyDef::KEY_UPARROW, false))
    {
		mFont.offsetLetter(mSelectedLetter, GHPoint2(0, -moveAmount), GHPoint2(0, 0));
        letterChanged = true;
    }
    else if (mInputState.checkKeyChange(0, GHKeyDef::KEY_DOWNARROW, false))
    {
		mFont.offsetLetter(mSelectedLetter, GHPoint2(0, moveAmount), GHPoint2(0, 0));
        letterChanged = true;
    }

    else if (mInputState.checkKeyChange(0, 'j', false))
    {
		mFont.offsetLetter(mSelectedLetter, GHPoint2(0, 0), GHPoint2(-moveAmount, 0));
        letterChanged = true;
    }
    else if (mInputState.checkKeyChange(0, 'l', false))
    {
		mFont.offsetLetter(mSelectedLetter, GHPoint2(0, 0), GHPoint2(moveAmount, 0));
        letterChanged = true;
    }
    else if (mInputState.checkKeyChange(0, 'i', false))
    {
		mFont.offsetLetter(mSelectedLetter, GHPoint2(0, 0), GHPoint2(0, moveAmount));
        letterChanged = true;
    }
    else if (mInputState.checkKeyChange(0, 'k', false))
    {
		mFont.offsetLetter(mSelectedLetter, GHPoint2(0, 0), GHPoint2(0, -moveAmount));
        letterChanged = true;
    }

    if (letterChanged)
    {
        GHMessage changeMessage(GHMessageTypes::WINDOWRESIZE);
        mMessageHandler.handleMessage(changeMessage);
    }
}

