// Copyright Golden Hammer Software
#include "GHCSClientList.h"
#include "GHCSClientInfo.h"

GHCSClientList::GHCSClientList(void)
: mNextClientId(1)
{
}

GHCSClientList::~GHCSClientList(void)
{
    disconnectAll();
}

GHCSClientInfo* GHCSClientList::connect(GHConnectionHandle* connection)
{
    GHCSClientInfo* ret = new GHCSClientInfo(connection, mNextClientId);
    mNextClientId++;
    mClients.push_back(ret);
    return ret;
}

void GHCSClientList::disconnect(GHConnectionHandle& connection)
{
    std::vector<GHCSClientInfo*>::iterator iter;
    for (iter = mClients.begin(); iter != mClients.end(); ++iter)
    {
        if (&(*iter)->getConnection() == &connection)
        {
            GHCSClientInfo* ci = *iter;
            mClients.erase(iter);
            delete ci;
        }
    }

}

void GHCSClientList::disconnectAll(void)
{
    for (size_t i = 0; i < mClients.size(); ++i)
    {
        delete mClients[i];
    }
    mClients.clear();
}

GHCSClientInfo* GHCSClientList::getClient(GHConnectionHandle& connection)
{
    for (size_t i = 0; i < mClients.size(); ++i)
    {
        if (&mClients[i]->getConnection() == &connection)
        {
            return mClients[i];
        }
    }
    return 0;
}

void GHCSClientList::sendReliable(const GHMessage& message)
{
    for (size_t i = 0; i < mClients.size(); ++i)
    {
        mClients[i]->getConnection().sendReliable(message);
    }
}

void GHCSClientList::sendUnreliable(const GHMessage& message)
{
    for (size_t i = 0; i < mClients.size(); ++i)
    {
        mClients[i]->getConnection().sendUnreliable(message);
    }
}

void GHCSClientList::readMessages(GHMessageHandler& handler)
{
    for (size_t i = 0; i < mClients.size(); ++i)
    {
        mClients[i]->getConnection().readMessages(handler);
    }
}

