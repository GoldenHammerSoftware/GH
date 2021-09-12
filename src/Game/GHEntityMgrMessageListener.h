// Copyright Golden Hammer Software
#include "GHUtils/GHMessageHandler.h"

class GHEventMgr;
class GHEntityMgr;

// a way to control an entity mgr through messages
class GHEntityMgrMessageListener : public GHMessageHandler
{
public:
    GHEntityMgrMessageListener(GHEntityMgr& entMgr, GHEventMgr& eventMgr);
    ~GHEntityMgrMessageListener(void);
  
    virtual void handleMessage(const GHMessage& message);
    
private:
    GHEntityMgr& mEntityMgr;
    GHEventMgr& mEventMgr;
};
