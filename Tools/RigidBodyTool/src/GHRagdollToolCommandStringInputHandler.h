// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHController.h"

class GHInputState;
class GHPropertyContainer;
class GHTimeVal;
class GHRagdollToolCommandParser;

class GHRagdollToolCommandStringInputHandler : public GHController
{
public:
    GHRagdollToolCommandStringInputHandler(const GHInputState& inputState,
                                           GHPropertyContainer& properties,
                                           const GHTimeVal& timeVal,
                                           GHRagdollToolCommandParser& commandParser);
    
    virtual void onActivate(void);
    virtual void onDeactivate(void);
    virtual void update(void);
    
private:
    void handleKey(int key);
    void appendCursor(void);
    bool isAccepted(int key) const;
    void eraseLastCharacter(void);
    void handleRepetition(void);
    
private:
    GHRagdollToolCommandParser& mCommandParser;
    const GHInputState& mInputState;
    GHPropertyContainer& mProperties;
    const GHTimeVal& mTimeVal;
    
    const static int BUFLEN = 256;
    char mBuffer[BUFLEN];
    int mCurrentChar;
    bool mCursorVisible;
    float mLastCursorChangeTime;
    int mLastKey;
    float mLastKeyDownTime;
    float mLastRepetitionTime;
};
