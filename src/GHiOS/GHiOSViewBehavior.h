#pragma once

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

class GHInputState;
class GHMutex;

// Objective C does not allow multiple inheritance,
//  but metal and ogl views must inherit from different places.
// This class exists in order to remove duplication between the two.
class GHiOSViewBehavior
{
public:
    GHiOSViewBehavior(void);

    void setInputState(GHInputState* state);
    void setInputMutex(GHMutex* mutex);
    void clearInput(void);
    // change the behavior of becomeFirstResponder.
    void setAllowKeyboard(bool val) { mAllowKeyboard = val; }
    bool getAllowKeyboard(void) const { return mAllowKeyboard; }
    float getSystemVersion(void);
    
    // these are the implementations of view functions.
    // a view using this behavior should implement all these and call through to here.
    void controllerStateChanged(void);
    void handleViewResize(CGSize viewSize);
    void touchesBegan(NSSet* touches, UIEvent* event, UIView* view);
    void touchesMoved(NSSet* touches, UIEvent* event, UIView* view);
    void touchesEnded(NSSet* touches, UIEvent* event);
    void insertText(NSString* text);
    void deleteBackward(void);
    
    void setWindowWidth(int val) { mWindowWidth = val; }
    void setWindowHeight(int val) { mWindowHeight = val; }
    
private:
    void sendTouchPosToApp(int index, float posX, float posY);

private:
    GHInputState* mInputState{0};
    GHMutex* mInputMutex{0};
    UITouch* mTouches[10];
    int mWindowWidth{0};
    int mWindowHeight{0};
    bool mAllowKeyboard{false};
};

