// Copyright 2010 Golden Hammer Software
#include "GHBBScoreloopUser.h"
#include "GHMutex.h"
#include "GHDebugMessage.h"

GHBBScoreloopUser::GHBBScoreloopUser(SC_Client_h& client)
: mClient(client)
, mUser(0)
{
	SC_Error_t errCode;

	errCode = SC_Client_CreateUserController(mClient, &mUserController, onUserDataFetched, this);
	if (errCode != SC_OK) {
		GHDebugMessage::outputString("Failed to create scoreloop user controller with error %d", errCode);
	}

	SC_Session_h session = SC_Client_GetSession(client);
	if (!session) {
		GHDebugMessage::outputString("Failed to get Scoreloop session from client");
	}

	mUser = SC_Session_GetUser(session);
	if (!mUser) {
		GHDebugMessage::outputString("Failed to get Scoreloop user from session");
	}
}

GHBBScoreloopUser::~GHBBScoreloopUser(void)
{
	SC_UserController_Release(mUserController);
}

void GHBBScoreloopUser::onUserDataFetched(void* cookie, SC_Error_t result)
{
	GHBBScoreloopUser* thiss = reinterpret_cast<GHBBScoreloopUser*>(cookie);
	if (result != SC_OK) {
		GHDebugMessage::outputString("Error fetching scoreloop user data with error code %d", result);
		return;
	}

	SC_Session_h session = SC_Client_GetSession(thiss->mClient);
	thiss->mUser = SC_Session_GetUser(session);
}
