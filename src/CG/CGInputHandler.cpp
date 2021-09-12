// Copyright 2010 Golden Hammer Software
#include "CGInputHandler.h"
#include "GHInputState.h"
#include "GHEntity.h"
#include "GHKeyDef.h"
#include "GHEntityHashes.h"
#include "GHModel.h"
#include "CGProperties.h"
#include "GHRefCounted.h"
#include "GHFloat.h"
#include "GHPhysicsSim.h"
#include "GHPhysicsObject.h"
#include "CGRayPicker.h"
#include "GHDebugMessage.h"
#include "GHMessageHandler.h"
#include "GHMessage.h"
#include "GHMessageTypes.h"
#include "GHGUIProperties.h"

CGInputHandler::CGInputHandler(const GHInputState& inputState,
                               const CGRayPicker& rayPicker,
                               GHPropertyContainer& appProps,
                               GHMessageHandler& messageHandler)
: mInputState(inputState)
, mRayPicker(rayPicker)
, mSelectedEntity(0)
, mLastSelectedEntity(0)
, mAppProps(appProps)
, mMessageHandler(messageHandler)
, mMoveCircle(0,0,0)
{

}

void CGInputHandler::update(void)
{
    if(mInputState.checkKeyChange(0, GHKeyDef::KEY_MOUSE1, true))
    {
        GHEntity* entity = findEntityUnderCursor(mTeamId);
        if (entity && entity->mProperties.getProperty(CGProperties::PP_BASE))
        {
            entity = 0;
        }
        
        if (entity)
        {
            mLastSelectedEntity = entity;
        }
        if (entity || !mSelectedEntity)
        {
            selectEntity(entity);
        }
                
        if (!entity && mSelectedEntity)
        {
            handleEntityAction();
        }
    }
    
    if (mInputState.checkKeyChange(0, GHKeyDef::KEY_MOUSE1, false))
    {
        GHEntity* entity = findEntityUnderCursor(mTeamId);
        if (entity && entity->mProperties.getProperty(CGProperties::PP_BASE))
        {
            GHPropertyContainer props;
            props.setProperty(GHGUIProperties::GP_WIDGETFILENAME, "basemodalgui.xml");
            GHMessage message(GHMessageTypes::GUIPUSH, &props);
            mMessageHandler.handleMessage(message);
        }
    }
    
    if (mLastSelectedEntity)
    {
        GHModel* model = mLastSelectedEntity->mProperties.getProperty(GHEntityHashes::MODEL);
        if (model && model->getSkeleton())
        {
            GHPoint3 entPos;
            model->getSkeleton()->getTransform().getTranslate(entPos);
            mMoveCircle.setCoords(entPos[0], -entPos[2], mLastSelectedEntity->mProperties.getProperty(CGProperties::PP_MOVERADIUS));
        }
    }
    else 
    {
        mMoveCircle.setCoords(0,0,0);
    }
    
    
    mAppProps.setProperty(CGProperties::GP_MOVECIRCLE, &mMoveCircle);
    
    //doDirectArrowKeyInput();
}

void CGInputHandler::handleEntityAction(void)
{
    if (!mSelectedEntity)
    {
        return;
    }
    
    bool didAction = false;
    GHEntity* enemy = findEntityUnderCursor(mTeamId%2+1);
    if (enemy)
    {
        float attackRadius = mSelectedEntity->mProperties.getProperty(CGProperties::PP_ATTACKRADIUS);
        float moveRadius = mSelectedEntity->mProperties.getProperty(CGProperties::PP_MOVERADIUS);
        
        GHPoint3 enemyPos, ourPos;
        GHModel* enemyModel = (GHModel*)enemy->mProperties.getProperty(GHEntityHashes::MODEL);
        GHModel* ourModel = (GHModel*)mSelectedEntity->mProperties.getProperty(GHEntityHashes::MODEL);
        
        enemyModel->getSkeleton()->getTransform().getTranslate(enemyPos);
        ourModel->getSkeleton()->getTransform().getTranslate(ourPos);
        
        enemyPos[1] = ourPos[1];
        enemyPos -= ourPos;
        
        float distSqr = enemyPos.lenSqr();
        
        
        if (distSqr < moveRadius*moveRadius)
        {
            int initiativePoints = mSelectedEntity->mProperties.getProperty(CGProperties::PP_INITIATIVEPOINTS);
            if (initiativePoints > 0)
            {
                mSelectedEntity->mProperties.setProperty(CGProperties::PP_INITIATIVEPOINTS, initiativePoints-1);
                mSelectedEntity->mProperties.setProperty(CGProperties::PP_ATTACKTARGET, enemy);
                if (distSqr < attackRadius*attackRadius)
                {
                    mSelectedEntity->mProperties.setProperty(CGProperties::PP_ISATTACKING, true);
                }
                else 
                {
                    GHPoint3 targetPos = enemyPos;
                    float dist = targetPos.normalize();
                    dist -= .5f; //fudge for radius of players
                    targetPos *= dist;
                    targetPos += ourPos;
                    
                    GHPoint3& worldPos = getTargetProperty();
                    worldPos = targetPos;
                    mSelectedEntity->mProperties.setProperty(CGProperties::PP_HASDESTINATION, true);
                }
                
                didAction = true;   
            }
        }

    }
    
    
    if (!didAction)
    {
        didAction = sendEntityToCursorPos();
    }
    
    if (didAction)
    {
        selectEntity(0);
    }
}

bool CGInputHandler::sendEntityToCursorPos(void)
{
    GHModel* model = (GHModel*)mSelectedEntity->mProperties.getProperty(GHEntityHashes::MODEL);
    if (!model || !model->getSkeleton())
    {
        return false;
    }
    
    const GHPoint2& screenPos = mInputState.getPointerPosition(0);
    GHPoint3 groundPos;
    if (!mRayPicker.findGroundPos(screenPos, groundPos)) {
        mSelectedEntity->mProperties.setProperty(CGProperties::PP_HASDESTINATION, false);
        return false;
    }
    
    GHPoint3 entityPos;
    model->getSkeleton()->getTransform().getTranslate(entityPos);
    
    GHPoint2 dist(groundPos[0]-entityPos[0], groundPos[2]-entityPos[2]);
    float remainingRadius = mSelectedEntity->mProperties.getProperty(CGProperties::PP_MOVERADIUS);
    if (remainingRadius*remainingRadius < dist.lenSqr())
    {
        //Todo: make it visually obvious to the player that they clicked.
        // too far away from the entity for the entity to move this turn.
        
        return false;
    }

    GHPoint3& worldPos = getTargetProperty();
    worldPos = groundPos;

    mSelectedEntity->mProperties.setProperty(CGProperties::PP_HASDESTINATION, true);

    mLastSelectedEntity = mSelectedEntity;
    
    return true;
}

GHEntity* CGInputHandler::findEntityUnderCursor(int team)
{
    std::vector<GHEntity*> ents;
    const GHPoint2& screenPos = mInputState.getPointerPosition(0);
    mRayPicker.findEntities(screenPos, ents);
    
    for (int i = 0; i < ents.size(); ++i)
    {
        GHEntity* ent = ents[i];
        if (ent == mSelectedEntity) {
            continue;
        }
        int entTeam = ent->mProperties.getProperty(CGProperties::PP_TEAM);
        if (entTeam == team)
        {
            int hitPoints = ent->mProperties.getProperty(CGProperties::PP_HITPOINTS);
            if (hitPoints > 0)
            {
                return ent;
            }
        }
    }
    
    return 0;
}

void CGInputHandler::selectEntity(GHEntity* entity)
{
    if (mSelectedEntity == entity) { return; }
    
    if (entity && entity->mProperties.getProperty(CGProperties::PP_BASE))
    {
        return;
    }
    
    if (mSelectedEntity)
    {
        mSelectedEntity->mProperties.setProperty(CGProperties::PP_ISSELECTED, false);
    }
    
    if (entity)
    {
        entity->mProperties.setProperty(CGProperties::PP_ISSELECTED, true);        
    }
    
    mSelectedEntity = entity;
}

void CGInputHandler::doDirectArrowKeyInput(void)
{
    if (!mSelectedEntity)
    {
        return;
    }
    
    GHPoint3 dir(0,0,0);
    
    bool up = mInputState.getKeyState(0, GHKeyDef::KEY_UPARROW);
    bool down = mInputState.getKeyState(0, GHKeyDef::KEY_DOWNARROW);
    bool left = mInputState.getKeyState(0, GHKeyDef::KEY_LEFTARROW);
    bool right = mInputState.getKeyState(0, GHKeyDef::KEY_RIGHTARROW);
    
    if (up && !down)
    {
        dir[2] = 1;
    }
    else if (down && !up)
    {
        dir[2] = -1;
    }
    
    if (left && !right)
    {
        dir[0] = 1;
    }
    else if (right && !left)
    {
        dir[0] = -1;
    }
    
    dir.normalize();
    
    bool hasDestination = !GHFloat::isZero(dir);
    mSelectedEntity->mProperties.setProperty(CGProperties::PP_HASDESTINATION, hasDestination);
    
    if (hasDestination)
    {
        GHModel* model = mSelectedEntity->mProperties.getProperty(GHEntityHashes::MODEL);
        if (!model || !model->getSkeleton()) { return; }
        GHPoint3& targetPos = getTargetProperty();
        model->getSkeleton()->getTransform().getTranslate(targetPos);
        targetPos += dir;   
    }
}

GHPoint3& CGInputHandler::getTargetProperty(void)
{
    GHPoint3* target = mSelectedEntity->mProperties.getProperty(CGProperties::PP_DESTINATION);
    if (!target)
    {
        target = new GHPoint3(0,-100,0);
        mSelectedEntity->mProperties.setProperty(CGProperties::PP_DESTINATION, GHProperty(target, new GHRefCountedType<GHPoint3>(target)));
    }
    return *target;
}

void CGInputHandler::onActivate(void)
{
    
}

void CGInputHandler::onDeactivate(void)
{
    if (mSelectedEntity)
    {
        mSelectedEntity->mProperties.setProperty(CGProperties::PP_ISSELECTED, false);
    }

    mSelectedEntity = 0;
    mLastSelectedEntity = 0;
    mMoveCircle.setCoords(0,0,0);
}
