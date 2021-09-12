// Copyright 2010 Golden Hammer Software
#include "GHRagdollToolCommandStringInputHandler.h"
#include "GHInputState.h"
#include "GHPropertyContainer.h"
#include "GHRagdollToolProperties.h"
#include "GHTimeVal.h"
#include "GHKeyDef.h"
#include "GHRagdollToolCommandParser.h"

GHRagdollToolCommandStringInputHandler::GHRagdollToolCommandStringInputHandler(const GHInputState& inputState,
                                                                               GHPropertyContainer& properties,
                                                                               const GHTimeVal& timeVal,
                                                                               GHRagdollToolCommandParser& commandParser)
: mCommandParser(commandParser)
, mInputState(inputState)
, mProperties(properties)
, mTimeVal(timeVal)
, mCursorVisible(false)
, mLastCursorChangeTime(0)
, mLastKey(0)
, mLastKeyDownTime(-1)
, mLastRepetitionTime(-1)
{
    
}

void GHRagdollToolCommandStringInputHandler::onActivate(void)
{
    mProperties.setProperty(GHRagdollToolProperties::COMMANDTEXT, mBuffer);
    mBuffer[0] = '>';
    mBuffer[1] = ' ';
    mCurrentChar = 2;
    mBuffer[mCurrentChar] = 0;
    
    mCursorVisible = true;
    mLastCursorChangeTime = mTimeVal.getTime();
}

void GHRagdollToolCommandStringInputHandler::onDeactivate(void)
{
    mBuffer[mCurrentChar]=0;
    mCommandParser.parseCommand(&mBuffer[2]);
    
    mProperties.setProperty(GHRagdollToolProperties::COMMANDTEXT, 0);
    mLastKey = 0;
    mLastKeyDownTime = -1;
}

bool GHRagdollToolCommandStringInputHandler::isAccepted(int key) const
{
    if ((key >= 'a' && key <= 'z')
        || (key >= 'A' && key <= 'Z')
        || (key >= '0' && key <= '9'))
    {
        return true;
    }
    
    int allowedKeys[] = {
        GHKeyDef::KEY_BACKSPACE,
        ' ',
        '=',
        '_',
        '-',
        '.',
        ','
    };
    
    for (int i = 0; i < sizeof(allowedKeys)/sizeof(allowedKeys[0]); ++i)
    {
        if (key == allowedKeys[i]) {
            return true;
        }
    }
    
    return false;
}

void GHRagdollToolCommandStringInputHandler::eraseLastCharacter(void)
{
    if (mCurrentChar <= 2) {
        return;
    }
    
    --mCurrentChar;
}

void GHRagdollToolCommandStringInputHandler::handleKey(int key)
{
    if (key == GHKeyDef::KEY_BACKSPACE) {
        eraseLastCharacter();
        return;
    }
    
    if (mCurrentChar >= BUFLEN) {
        return;
    }
    
    mBuffer[mCurrentChar] = key;
    ++mCurrentChar;
}

void GHRagdollToolCommandStringInputHandler::update(void)
{
    const GHInputState::InputEventList& inputEvents = mInputState.getInputEvents();
    size_t numEvents = inputEvents.size();
    for (size_t i = 0; i < numEvents; ++i)
    {
        const GHInputState::InputEvent& event = inputEvents[i];
        if (event.mType == GHInputState::IET_KEYCHANGE
            && event.mVal[1] != 0)
        {
            int key = event.mVal[0];
            if (isAccepted(key))
            {
                handleKey(key);
                mLastKey = key;
                mLastKeyDownTime = mTimeVal.getTime();
                mLastRepetitionTime = -1;
            }
        }
    }
    handleRepetition();
    appendCursor();
}

void GHRagdollToolCommandStringInputHandler::handleRepetition(void)
{
    if (mLastKeyDownTime == -1) {
        return;
    }
    
    if (!mInputState.getKeyState(0, mLastKey))
    {
        mLastKeyDownTime = -1;
        return;
    }
    
    const float kInitialLag = .5f;
    const float kTimeBetweenReps = .1f;
    
    float currTime = mTimeVal.getTime();
    
    if (currTime - mLastKeyDownTime > kInitialLag)
    {
        if (mLastRepetitionTime == -1
            || currTime - mLastRepetitionTime > kTimeBetweenReps)
        {
            mLastRepetitionTime = mTimeVal.getTime();
            handleKey(mLastKey);
        }
    }
}

void GHRagdollToolCommandStringInputHandler::appendCursor(void)
{
    if (mTimeVal.getTime() - mLastCursorChangeTime > .5f)
    {
        mLastCursorChangeTime = mTimeVal.getTime();
        mCursorVisible = !mCursorVisible;
    }
    
    if (mCursorVisible) {
        snprintf(&mBuffer[mCurrentChar], BUFLEN-mCurrentChar, "|");
    }
    else {
        mBuffer[mCurrentChar] = 0;
    }
}

