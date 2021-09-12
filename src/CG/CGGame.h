// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHMessageHandler.h"
#include <vector>
#include "CGLibrary.h"

class GHEntity;
class GHAppShard;
class GHSystemServices;
class CGPlayer;
class GHStateMachine;
class CGEntityPlacementInputHandler;
class CGRayPicker;
class CGInputHandler;
class CGDeadEntityDeleter;
class GHCamera;
class CGBaseUpgrader;

// Structure to hold the stuff associated with a game
// Helpful for knowing what to delete when we end a game.
class CGGame
{
private:
    class MessageHandler : public GHMessageHandler
    {
    public:
        MessageHandler(CGGame& game);
        virtual void handleMessage(const GHMessage& message);
        
    private:
        CGGame& mGame;
    };
    
public:
    CGGame(GHAppShard& appShard, GHSystemServices& systemServices, GHStateMachine& gameStateMachine, CGRayPicker& rayPicker, GHCamera& camera);
    ~CGGame(void);
    
    void startGame(void);
    void endGame(void);
    void endTurn(void);
    void selectCard(int cardIndex);
    void handleCardUsed(void);
    void addEntity(GHEntity* ent);
    
private:
    void createPlayers(void);
    void activatePlayer(int playerIdx);

private:
    MessageHandler mMessageHandler;
    GHAppShard& mAppShard;
    GHSystemServices& mSystemServices;
    GHStateMachine& mGameStateMachine;
    CGEntityPlacementInputHandler* mPlacementInputHandler;
    CGInputHandler* mEntSelector;
    CGDeadEntityDeleter* mEntDeleter;
    CGBaseUpgrader* mBaseUpgrader;
    GHCamera& mCamera;
    CGRayPicker& mRayPicker;
    
    CGLibrary mLibrary;
    std::vector<CGPlayer*> mPlayers;
    int mCurrPlayerIdx;
    int mSelectedCard;
        
    std::vector<GHEntity*> mEntities;
};
