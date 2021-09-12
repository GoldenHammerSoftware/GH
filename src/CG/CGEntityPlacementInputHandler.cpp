// Copyright 2010 Golden Hammer Software
#include "CGEntityPlacementInputHandler.h"
#include "GHStateMachine.h"
#include "GHInputState.h"
#include "GHXMLObjFactory.h"
#include "GHKeyDef.h"
#include "GHDebugMessage.h"
#include "CGGameStates.h"
#include "CGGame.h"
#include "CGProperties.h"
#include "GHEntity.h"
#include "GHEntityHashes.h"
#include "GHModel.h"
#include "CGRayPicker.h"
#include "CGPlayer.h"

CGEntityPlacementInputHandler::CGEntityPlacementInputHandler(GHStateMachine& gameStateMachine, GHInputState& inputState, GHXMLObjFactory& objFactory, CGGame& game, const GHIdentifier& activeEntId, const CGRayPicker& rayPicker)
: mGameStateMachine(gameStateMachine)
, mInputState(inputState)
, mObjFactory(objFactory)
, mGame(game)
, mActiveEntId(activeEntId)
, mRayPicker(rayPicker)
, mPlayer(0)
{
}

void CGEntityPlacementInputHandler::update(void)
{
    if (mInputState.checkKeyChange(0, GHKeyDef::KEY_MOUSE1, true)) 
    {
        const GHPoint2& screenPos = mInputState.getPointerPosition(0);
        GHPoint3 groundPos;
        if (mRayPicker.findGroundPos(screenPos, groundPos)) 
        {
            GHPoint2 pos2d(groundPos[0], groundPos[2]);
            if (!mPlayer->getPlacementRegion().containsPoint(pos2d)) 
            {
                GHDebugMessage::outputString("failing to place ent: %f %f", pos2d[0], pos2d[1]);
                return;
            }

            groundPos[1] += 1;
            GHEntity* ent = (GHEntity*)mObjFactory.load(mEntityFile.getChars());
            if (ent) {
                GHModel* model = ent->mProperties.getProperty(GHEntityHashes::MODEL);
                if (model) {
                    GHTransform transform;
                    transform.becomeYRotation(mPlayer->getPlacementRot());
                    model->getSkeleton()->getLocalTransform().mult(transform, model->getSkeleton()->getLocalTransform());
                    model->getSkeleton()->getLocalTransform().setTranslate(groundPos);
                }
                ent->mProperties.setProperty(CGProperties::PP_TEAM, mTeam);
                if (mPlayer) {
                    ent->mProperties.setProperty(CGProperties::PP_ENTCOLOR, &mPlayer->getColor());
                }
                ent->mStateMachine.setState(mActiveEntId);
                mGame.addEntity(ent);
            }
            
            mGame.handleCardUsed();
            mGameStateMachine.setState(GS_DEFAULT);
        }
    }
}
