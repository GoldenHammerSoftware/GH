// Copyright Golden Hammer Software
#pragma once

class GHMultiplayerMgr;
class GHAppShard;

class GHMultiplayerFactory
{
public:
    virtual ~GHMultiplayerFactory(void) { }
    
    //The enable property signals whether multiplayer is ever available on the device at all.
    //The active property signals whether multiplayer is currently active. For example,
    // it will get set in gamecenter after the local player is authenticated.
    
    //Both properties are set by the platform multiplayer system.
    
    virtual GHMultiplayerMgr* createMultiplayerMgr(GHAppShard& appShard) = 0;
};
