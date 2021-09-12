// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHController.h"
#include "GHPoint.h"
#include "GHScreenPosUnprojector.h"

class GHInputState;
class GHEntity;
class CGRayPicker;
class GHPropertyContainer;
class GHMessageHandler;

class CGInputHandler : public GHController
{
public:
    CGInputHandler(const GHInputState& inputState,
                   const CGRayPicker& rayPicker,
                   GHPropertyContainer& appProps,
                   GHMessageHandler& messageHandler);
    
    virtual void update(void);
    virtual void onActivate(void);
    virtual void onDeactivate(void);
    
    void setActivePlayer(int teamId) { mTeamId = teamId; selectEntity(0); mLastSelectedEntity= 0; }
    
private:
    GHPoint3& getTargetProperty(void);
    void doDirectArrowKeyInput(void);
    void selectEntity(GHEntity* entity);
    GHEntity* findEntityUnderCursor(int team);
    void handleEntityAction(void);
    bool sendEntityToCursorPos(void);
    
private:
    const GHInputState& mInputState;
    const CGRayPicker& mRayPicker;
    GHPropertyContainer& mAppProps;
    GHMessageHandler& mMessageHandler;
    GHPoint3 mMoveCircle;
    int mTeamId;
    
    GHEntity* mSelectedEntity;
    GHEntity* mLastSelectedEntity;
};
