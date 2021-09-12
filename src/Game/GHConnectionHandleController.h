// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"

class GHConnectionHandle;
class GHMessageHandler;

// a class to clear out a connection handle of messages every update
class GHConnectionHandleController : public GHController
{
public:
    GHConnectionHandleController(void);

    void setTarget(GHConnectionHandle* connection, GHMessageHandler* handler);
    
	void update(void);
	void onActivate(void) {}
	void onDeactivate(void) {}
    
private:
    GHConnectionHandle* mConnection;
    GHMessageHandler* mHandler;
};
