// Copyright 2010 Golden Hammer Software
#include "CGGame.h"
#include "GHModel.h"
#include "GHEntity.h"
#include "GHSystemServices.h"
#include "GHAppShard.h"
#include "GHEntityHashes.h"
#include "GHControllerTransition.h"
#include "CGGameStates.h"
#include "CGProperties.h"
#include "GHMessage.h"
#include "CGPlayer.h"
#include "CGEntityPlacementInputHandler.h"
#include "CGCard.h"
#include "CGInputHandler.h"
#include "CGDeadEntityDeleter.h"
#include "GHCamera.h"
#include "CGBaseUpgrader.h"
#include "CGRayPicker.h"

CGGame::MessageHandler::MessageHandler(CGGame& game)
: mGame(game)
{
}

void CGGame::MessageHandler::handleMessage(const GHMessage& message)
{
    if (message.getType() == CGProperties::NEWGAME) {
        mGame.startGame();
    }
    else if (message.getType() == CGProperties::EXITGAME) {
        mGame.endGame();
    }
    else if (message.getType() == CGProperties::ENDTURN) {
        mGame.endTurn();
    }
    else if (message.getType() == CGProperties::SELECTCARD) {
        int cardIndex = message.getPayload().getProperty(CGProperties::INDEX);
        mGame.selectCard(cardIndex);
    }
}

CGGame::CGGame(GHAppShard& appShard, GHSystemServices& systemServices, GHStateMachine& gameStateMachine, CGRayPicker& rayPicker, GHCamera& camera)
: mAppShard(appShard)
, mSystemServices(systemServices)
, mGameStateMachine(gameStateMachine)
, mMessageHandler(*this)
, mCamera(camera)
, mRayPicker(rayPicker)
{
    mAppShard.mEventMgr.registerListener(CGProperties::NEWGAME, mMessageHandler);
    mAppShard.mEventMgr.registerListener(CGProperties::EXITGAME, mMessageHandler);
    mAppShard.mEventMgr.registerListener(CGProperties::ENDTURN, mMessageHandler);
    mAppShard.mEventMgr.registerListener(CGProperties::SELECTCARD, mMessageHandler);
    
    GHIdentifier activeId = mSystemServices.mHashTable.generateHash("ES_ACTIVE");
    mPlacementInputHandler = new CGEntityPlacementInputHandler(mGameStateMachine, *mSystemServices.mInputState, mAppShard.mXMLObjFactory, *this, activeId, rayPicker);
    mGameStateMachine.addTransition(GS_PLACING, new GHControllerTransition(mAppShard.mControllerMgr, mPlacementInputHandler));
    
    mEntSelector = new CGInputHandler(*mSystemServices.mInputState, rayPicker, mAppShard.mProps, mAppShard.mEventMgr);
    mGameStateMachine.addTransition(GS_DEFAULT, new GHControllerTransition(mAppShard.mControllerMgr, mEntSelector));
    
    mEntDeleter = new CGDeadEntityDeleter(mEntities);
    mAppShard.mControllerMgr.add(mEntDeleter);
    
    mBaseUpgrader = new CGBaseUpgrader(appShard.mXMLObjFactory, appShard.mEventMgr, mCurrPlayerIdx, mEntities);
}

CGGame::~CGGame(void)
{
    delete mBaseUpgrader;
    mAppShard.mControllerMgr.remove(mEntDeleter);
    delete mEntDeleter;
    
    mAppShard.mEventMgr.unregisterListener(CGProperties::NEWGAME, mMessageHandler);
    mAppShard.mEventMgr.unregisterListener(CGProperties::EXITGAME, mMessageHandler);
    mAppShard.mEventMgr.unregisterListener(CGProperties::ENDTURN, mMessageHandler);
    mAppShard.mEventMgr.unregisterListener(CGProperties::SELECTCARD, mMessageHandler);
    endGame();
}

void CGGame::startGame(void)
{
    GHIdentifier activeId = mSystemServices.mHashTable.generateHash("ES_ACTIVE");
    GHEntity* terrEnt = (GHEntity*)mAppShard.mXMLObjFactory.load("heightfieldmapent.xml");
    if (terrEnt) {
        GHModel* model = terrEnt->mProperties.getProperty(GHEntityHashes::MODEL);
        if (model) {
            model->getSkeleton()->getLocalTransform().setTranslate(GHPoint3(-6.3, 0, -6.3));
            
            mRayPicker.setGround(model->getPhysicsObject());
        }
        
        terrEnt->mStateMachine.setState(activeId);
        addEntity(terrEnt);
    }
    
    createPlayers();
    mCurrPlayerIdx = 0;
    activatePlayer(mCurrPlayerIdx);

    GHEntity* p1Base = (GHEntity*)mAppShard.mXMLObjFactory.load("homebaseent.xml");
    if (p1Base) {
        GHModel* model = p1Base->mProperties.getProperty(GHEntityHashes::MODEL);
        if (model) {
            model->getSkeleton()->getLocalTransform().setTranslate(GHPoint3(0, 2, -4.6));
        }
        p1Base->mStateMachine.setState(activeId);
        p1Base->mProperties.setProperty(CGProperties::PP_TEAM, 1);
        p1Base->mProperties.setProperty(CGProperties::PP_ENTCOLOR, &mPlayers[0]->getColor());
        addEntity(p1Base);
    }
    GHEntity* p2Base = (GHEntity*)mAppShard.mXMLObjFactory.load("homebaseent.xml");
    if (p2Base) {
        GHModel* model = p2Base->mProperties.getProperty(GHEntityHashes::MODEL);
        if (model) {
            model->getSkeleton()->getLocalTransform().setTranslate(GHPoint3(0, 2, 3.6));
        }
        p2Base->mStateMachine.setState(activeId);
        p2Base->mProperties.setProperty(CGProperties::PP_TEAM, 2);
        p2Base->mProperties.setProperty(CGProperties::PP_ENTCOLOR, &mPlayers[1]->getColor());
        addEntity(p2Base);
    }
}

void CGGame::createPlayers(void)
{
    CGPlayer* player1 = new CGPlayer(mAppShard.mProps, "Player 1", GHPoint3(1,0.5,0.5), GHRect<float,2>(GHPoint2(-7.8,-5), GHPoint2(7.8, -3)));
    mPlayers.push_back(player1);
    player1->setCameraTrans(GHTransform(0.999989,
                                        0,
                                        -0.00459953,
                                        0,
                                        0.00375942,
                                        0.576141,
                                        0.817341,
                                        0,
                                        0.00264998,
                                        -0.81735,
                                        0.576135,
                                        0,
                                        -0.142693,
                                        8.66482,
                                        -7.47903,
                                        1));
    player1->setPlacementRot(3.14);
    
    CGPlayer* player2 = new CGPlayer(mAppShard.mProps, "Player 2", GHPoint3(0.7,0.7,1), GHRect<float,2>(GHPoint2(-7.8,1.7), GHPoint2(7.8, 4)));
    mPlayers.push_back(player2);
    player2->setCameraTrans(GHTransform(-0.999962,
                                        0,
                                        0.00870035,
                                        0,
                                        -0.00700927,
                                        0.592418,
                                        -0.8056,
                                        0,
                                        -0.00515424,
                                        -0.805631,
                                        -0.592396,
                                        0,
                                        -0.0559302,
                                        8.34958,
                                        6.67214,
                                        1));

    for (int i = 0; i < 2; ++i) {
        mLibrary.createStarterDeck(*mPlayers[i]);
        mPlayers[i]->startGame();
    }
}

void CGGame::endGame(void)
{
    mRayPicker.setGround(0);
    
    mEntSelector->setActivePlayer(-1);
    
    for (int i = 0; i < mPlayers.size(); ++i) {
        delete mPlayers[i];
    }
    mPlayers.clear();

    for (int i = 0; i < mEntities.size(); ++i) {
        if (mEntities[i]->mProperties.getProperty(CGProperties::PP_BASE))
        {
            int brkpt = 5;
            brkpt = 4;
        }
             
        
        delete mEntities[i];
    }
    mEntities.clear();
}

void CGGame::endTurn(void)
{
    mCurrPlayerIdx++;
    if (mCurrPlayerIdx >= mPlayers.size()) {
        mCurrPlayerIdx = 0;
    }
    activatePlayer(mCurrPlayerIdx);
    
    std::vector<GHEntity*>::iterator iter = mEntities.begin();
    while (iter != mEntities.end())
    {
        bool iterated = false;
        
        GHEntity* ent = *iter;
        int team = ent->mProperties.getProperty(CGProperties::PP_TEAM);
        if (team == mCurrPlayerIdx+1)
        {
            ent->mProperties.setProperty(CGProperties::PP_MOVERADIUS, ent->mProperties.getProperty(CGProperties::PP_DEFAULTMOVERADIUS));
            ent->mProperties.setProperty(CGProperties::PP_INITIATIVEPOINTS, ent->mProperties.getProperty(CGProperties::PP_DEFAULTINITIATIVEPOINTS));
        }
        else if (team == (mCurrPlayerIdx+1)%mPlayers.size()+1)
        {
            ent->mProperties.setProperty(CGProperties::PP_ATTACKTARGET, 0);
            ent->mProperties.setProperty(CGProperties::PP_ISATTACKING, false);
            
            int hitPoints = ent->mProperties.getProperty(CGProperties::PP_HITPOINTS);
            if (hitPoints <= 0)
            {
                delete ent;
                iter = mEntities.erase(iter);   
                iterated = true;
            }
        }
        
        if (!iterated)
        {
            ++iter;
        }
    }
}

void CGGame::activatePlayer(int playerIdx)
{
    mPlayers[playerIdx]->startTurn();
    mEntSelector->setActivePlayer(mCurrPlayerIdx+1);
    mCamera.getTransform() = mPlayers[playerIdx]->getCameraTrans();
}

void CGGame::selectCard(int cardIndex)
{
    CGCard* card = mPlayers[mCurrPlayerIdx]->getCard(cardIndex);
    if (!card) return;
    mSelectedCard = cardIndex;
    
    mPlacementInputHandler->setEntity(card->mEntFile.getChars());
    mPlacementInputHandler->setTeam(mCurrPlayerIdx+1);
    mPlacementInputHandler->setPlayer(mPlayers[mCurrPlayerIdx]);
    mGameStateMachine.setState(GS_PLACING);
}

void CGGame::handleCardUsed(void)
{
    mPlayers[mCurrPlayerIdx]->removeCard(mSelectedCard);
}

void CGGame::addEntity(GHEntity* ent)
{
    mEntities.push_back(ent);
}

