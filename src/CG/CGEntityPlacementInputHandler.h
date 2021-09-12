// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHController.h"
#include "GHString.h"
#include "GHIdentifier.h"

class GHStateMachine;
class GHInputState;
class GHXMLObjFactory;
class CGGame;
class CGRayPicker;
class CGPlayer;

// Input handling class for once a card that summons an entity is selected.
class CGEntityPlacementInputHandler : public GHController
{
public:
    CGEntityPlacementInputHandler(GHStateMachine& gameStateMachine, GHInputState& inputState, GHXMLObjFactory& objFactory, CGGame& game, const GHIdentifier& activeEntId, const CGRayPicker& rayPicker);

    void setEntity(const char* entFile) { mEntityFile.setConstChars(entFile, GHString::CHT_COPY); }
    void setTeam(int team) { mTeam = team; }
    void setPlayer(CGPlayer* player) { mPlayer = player; }
    
    virtual void update(void);
	virtual void onActivate(void) {}
	virtual void onDeactivate(void) {}

private:
    GHStateMachine& mGameStateMachine;
    GHInputState& mInputState;
    GHXMLObjFactory& mObjFactory;
    CGGame& mGame;
    const CGRayPicker& mRayPicker;
    GHString mEntityFile;
    GHIdentifier mActiveEntId;
    int mTeam;
    CGPlayer* mPlayer;
};
