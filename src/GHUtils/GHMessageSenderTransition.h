// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include <vector>

class GHMessageHandler;
class GHMessage;

// A transition to send a message on activate.
// We take ownership of the message.
class GHMessageSenderTransition : public GHTransition
{
public:  
    GHMessageSenderTransition(GHMessageHandler& handler);
    ~GHMessageSenderTransition(void);
    
    virtual void activate(void);
	virtual void deactivate(void);
    
    void addActivateMessage(GHMessage* message);
    void addDeactivateMessage(GHMessage* message);
    
private:
    GHMessageHandler& mMessageHandler;
    std::vector<GHMessage*> mActivateMessages;
    std::vector<GHMessage*> mDeactivateMessages;
};
