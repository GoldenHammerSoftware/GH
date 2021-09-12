// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"

class GHInputState;
class GHFont;
class GHMessageHandler;

// a controller for simple font editing.
// hit ' then any character to select a letter
// use arrow keys to move letter min
// use ijkl to move letter max.
// hit p to print out the changes.
// hold shift to move faster.
class GHFontEditor : public GHController
{
public:
    GHFontEditor(const GHInputState& inputState, GHFont& font, GHMessageHandler& messageHandler);
    
    virtual void update(void);
	virtual void onActivate(void) {}
	virtual void onDeactivate(void) {}
    
private:
    const GHInputState& mInputState;
    GHFont& mFont;
    GHMessageHandler& mMessageHandler;
    
    // bool to say whether we detected a '
    bool mSelectLetter;
    char mSelectedLetter;
};
