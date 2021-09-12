// Copyright Golden Hammer Software
#pragma once

class GHConnectionHandle;

// struct for storing info about a client on the server
class GHCSClientInfo 
{
public:
    GHCSClientInfo(GHConnectionHandle* connection, int clientId);
    ~GHCSClientInfo(void);

    GHConnectionHandle& getConnection(void) { return *mConnection; }
    int getClientId(void) const { return mClientId; }
    
private:
    GHConnectionHandle* mConnection;
    // unique id for this client.
    int mClientId;
};
