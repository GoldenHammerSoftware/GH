#include "GHiOSViewBehavior.h"
#include "GHInputState.h"
#include "GHKeyDef.h"
#include "GHPlatform/GHMutex.h"
#include "GHPlatform/GHDebugMessage.h"
#import <GameController/GameController.h>

static int sMaxTouches = 10;

static int findTouchIndex(UITouch* touch, __strong UITouch** mTouches)
{
    // first look for an existing match.
    for (int i = 1; i < sMaxTouches; ++i) {
        if (mTouches[i] == touch) {
            return i;
        }
    }
    // then look for an open slot.
    for (int i = 1; i < sMaxTouches; ++i) {
        if (mTouches[i] == 0) {
            mTouches[i] = touch;
            return i;
        }
    }
    return -1;
}

GHiOSViewBehavior::GHiOSViewBehavior(void)
{
    for (int i = 0; i < sMaxTouches; ++i) {
        mTouches[i] = 0;
    }
    mAllowKeyboard = false;
};

void GHiOSViewBehavior::setInputState(GHInputState* state)
{
    mInputState = state;
}

void GHiOSViewBehavior::setInputMutex(GHMutex* mutex)
{
    mInputMutex = mutex;
}

void GHiOSViewBehavior::clearInput(void)
{
    for (int i = 0; i < sMaxTouches; ++i)
    {
        if (mTouches[i])
        {
            mTouches[i] = 0;
        }
    }
}

void GHiOSViewBehavior::sendTouchPosToApp(int index, float posX, float posY)
{
    if (!mInputState || !mInputMutex) return;
    
    mInputMutex->acquire();
    
    float xDiv = mWindowWidth;
    float yDiv = mWindowHeight;
    // We used to do flipping of input coords here.
    // Width/height are now flipped in GHiOSViewBehavior::handleViewResize
    float normX = posX/xDiv;
    float normY = posY/yDiv;
    
    mInputState->handlePointerChange(index, GHPoint2(normX, normY));
    mInputMutex->release();
}

float GHiOSViewBehavior::getSystemVersion(void)
{
    float systemVersion = [[[UIDevice currentDevice] systemVersion] floatValue];
    return systemVersion;
}

void GHiOSViewBehavior::controllerStateChanged(void)
{
    int controllerIndex = 0;
    
    for (size_t i = 0; i < mInputState->getNumGamepads(); ++i)
    {
        mInputState->handleGamepadActive((unsigned int)i, false);
    }
    
    for (GCController* controller in [GCController controllers])
    {
        mInputMutex->acquire();
        mInputState->handleGamepadActive(controllerIndex, true);
        mInputMutex->release();
        
        controller.controllerPausedHandler = ^(GCController *parentController)
        {
            mInputMutex->acquire();
            mInputState->handleGamepadButtonChange(controllerIndex, GHKeyDef::KEY_GP_START, 1);
            mInputState->handleGamepadButtonChange(controllerIndex, GHKeyDef::KEY_GP_START, 0);
            mInputMutex->release();
        };

        controller.gamepad.buttonY.valueChangedHandler = ^(GCControllerButtonInput *axis, float value, BOOL pressed)
        {
            mInputMutex->acquire();
            mInputState->handleGamepadButtonChange(controllerIndex, GHKeyDef::KEY_GP_Y, value);
            mInputMutex->release();
        };
        controller.gamepad.buttonX.valueChangedHandler = ^(GCControllerButtonInput *axis, float value, BOOL pressed)
        {
            mInputMutex->acquire();
            mInputState->handleGamepadButtonChange(controllerIndex, GHKeyDef::KEY_GP_X, value);
            mInputMutex->release();
        };
        controller.gamepad.buttonB.valueChangedHandler = ^(GCControllerButtonInput *axis, float value, BOOL pressed)
        {
            mInputMutex->acquire();
            mInputState->handleGamepadButtonChange(controllerIndex, GHKeyDef::KEY_GP_B, value);
            mInputMutex->release();
        };
        controller.gamepad.buttonA.valueChangedHandler = ^(GCControllerButtonInput *axis, float value, BOOL pressed)
        {
            mInputMutex->acquire();
            mInputState->handleGamepadButtonChange(controllerIndex, GHKeyDef::KEY_GP_A, value);
            mInputMutex->release();
        };
        
        controller.gamepad.dpad.up.valueChangedHandler = ^(GCControllerButtonInput *axis, float value, BOOL pressed)
        {
            mInputMutex->acquire();
            mInputState->handleGamepadButtonChange(controllerIndex, GHKeyDef::KEY_GP_DPADUP, value);
            mInputMutex->release();
        };
        
        controller.gamepad.dpad.down.valueChangedHandler = ^(GCControllerButtonInput *axis, float value, BOOL pressed)
        {
            mInputMutex->acquire();
            mInputState->handleGamepadButtonChange(controllerIndex, GHKeyDef::KEY_GP_DPADDOWN, value);
            mInputMutex->release();
        };
        
        controller.gamepad.dpad.left.valueChangedHandler = ^(GCControllerButtonInput *axis, float value, BOOL pressed)
        {
            mInputMutex->acquire();
            mInputState->handleGamepadButtonChange(controllerIndex, GHKeyDef::KEY_GP_DPADLEFT, value);
            mInputMutex->release();
        };
        
        controller.gamepad.dpad.right.valueChangedHandler = ^(GCControllerButtonInput *axis, float value, BOOL pressed)
        {
            mInputMutex->acquire();
            mInputState->handleGamepadButtonChange(controllerIndex, GHKeyDef::KEY_GP_DPADRIGHT, value);
            mInputMutex->release();
        };
        
        controller.gamepad.leftShoulder.valueChangedHandler = ^(GCControllerButtonInput *axis, float value, BOOL pressed)
        {
            mInputMutex->acquire();
            mInputState->handleGamepadButtonChange(controllerIndex, GHKeyDef::KEY_GP_LEFTBUMPER, value);
            mInputMutex->release();
        };
        
        controller.gamepad.rightShoulder.valueChangedHandler = ^(GCControllerButtonInput *axis, float value, BOOL pressed)
        {
            mInputMutex->acquire();
            mInputState->handleGamepadButtonChange(controllerIndex, GHKeyDef::KEY_GP_RIGHTBUMPER, value);
            mInputMutex->release();
        };
        
        if (controller.extendedGamepad != nil)
        {
            controller.extendedGamepad.leftTrigger.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
            {
                mInputMutex->acquire();
                mInputState->handleGamepadButtonChange(controllerIndex, GHKeyDef::KEY_GP_LEFTTRIGGER, value);
                mInputMutex->release();
            };
            
            controller.extendedGamepad.rightTrigger.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
            {
                mInputMutex->acquire();
                mInputState->handleGamepadButtonChange(controllerIndex, GHKeyDef::KEY_GP_RIGHTTRIGGER, value);
                mInputMutex->release();
            };
            
            controller.extendedGamepad.leftThumbstick.valueChangedHandler = ^(GCControllerDirectionPad *dpad, float xValue, float yValue)
            {
                mInputMutex->acquire();
                mInputState->handleGamepadJoystickChange(controllerIndex, 0, GHPoint2(xValue, yValue));
                mInputMutex->release();
            };
            
            controller.extendedGamepad.rightThumbstick.valueChangedHandler = ^(GCControllerDirectionPad *dpad, float xValue, float yValue)
            {
                mInputMutex->acquire();
                mInputState->handleGamepadJoystickChange(controllerIndex, 1, GHPoint2(xValue, yValue));
                mInputMutex->release();
            };
        }
        ++controllerIndex;
    }
}

void GHiOSViewBehavior::handleViewResize(CGSize viewSize)
{
    bool isLandscape = false;
    if(@available(ios 14.0, *))
    {
        if([NSProcessInfo processInfo].iOSAppOnMac)
        {
            isLandscape = true;
        }
    }
    if (!isLandscape)
    {
        UIInterfaceOrientation orientation = [[UIApplication sharedApplication] statusBarOrientation];
        isLandscape = !UIInterfaceOrientationIsPortrait(orientation);
    }
    
    // some duplicated logic with deviceOrientationDidChange
    bool rotateScreen = false;
    if (!isLandscape)
    {
        if (viewSize.width > viewSize.height)
        {
            rotateScreen = true;
        }
    }
    else
    {
        if (viewSize.width < viewSize.height)
        {
            rotateScreen = true;
        }
    }
    
    if (rotateScreen)
    {
        mWindowWidth = viewSize.height;
        mWindowHeight = viewSize.width;
    }
    else
    {
        mWindowWidth = viewSize.width;
        mWindowHeight = viewSize.height;
    }
}

void GHiOSViewBehavior::touchesBegan(NSSet* touches, UIEvent* event, UIView* view)
{
    if (!mInputState || !mInputMutex) return;

    NSArray *touchArr = [touches allObjects];
    NSInteger touchCnt = [touchArr count];
    for (int i = 0; i < touchCnt; ++i)
    {
        UITouch *touch = [touchArr objectAtIndex:i];
        int index = findTouchIndex(touch, mTouches);
        CGPoint touchPoint = [touch locationInView:view];
        
        mInputMutex->acquire();
        mInputState->handlePointerActive(index, true);
        mInputState->handleKeyChange(0, (int)GHKeyDef::KEY_MOUSE1 + (index*3), true);
        mInputMutex->release();
        
        sendTouchPosToApp(index, touchPoint.x, touchPoint.y);
    }
}

void GHiOSViewBehavior::touchesMoved(NSSet* touches, UIEvent* event, UIView* view)
{
    NSArray *touchArr = [touches allObjects];
    NSInteger touchCnt = [touchArr count];
    for (int i = 0; i < touchCnt; ++i)
    {
        UITouch *touch = [touchArr objectAtIndex:i];
        int index = findTouchIndex(touch, mTouches);
        CGPoint touchPoint = [touch locationInView:view];
        sendTouchPosToApp(index, touchPoint.x, touchPoint.y);
    }
}

void GHiOSViewBehavior::touchesEnded(NSSet* touches, UIEvent* event)
{
    NSArray *touchArr = [touches allObjects];
    NSInteger touchCnt = [touchArr count];
    for (int i = 0; i < touchCnt; ++i)
    {
        UITouch *touch = [touchArr objectAtIndex:i];
        int index = findTouchIndex(touch, mTouches);
        if (index == -1) {
            continue;
        }
        mInputMutex->acquire();
        mInputState->handleKeyChange(0, (int)GHKeyDef::KEY_MOUSE1 + (index*3), false);
        mInputState->handlePointerActive(index, false);
        mInputMutex->release();
        
        // deactivate the pointer works better than moving it offscreen.
        //sendTouchPosToApp(index, -1, -1);
        
        mTouches[index] = 0;
    }
}

void GHiOSViewBehavior::insertText(NSString* text)
{
    char c = [text UTF8String][0];
    if (c == '\n') c = GHKeyDef::KEY_ENTER;
    mInputMutex->acquire();
    mInputState->handleKeyChange(0, c, true);
    mInputState->handleKeyChange(0, c, false);
    mInputMutex->release();
}

void GHiOSViewBehavior::deleteBackward(void)
{
    mInputMutex->acquire();
    mInputState->handleKeyChange(0, GHKeyDef::KEY_DELETE, true);
    mInputState->handleKeyChange(0, GHKeyDef::KEY_DELETE, false);
    mInputMutex->release();
}
