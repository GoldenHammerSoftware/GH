// Copyright 2010 Golden Hammer Software
#pragma once

#include <scoreloop/scoreloopcore.h>

//Shared reference to the local user in Scoreloop.
// Used by both achievements and high scores.
class GHBBScoreloopUser
{
public:
	GHBBScoreloopUser(SC_Client_h& client);
	~GHBBScoreloopUser(void);

	SC_User_h getUser(void) { return mUser; }

private:
	//callback method to be called by Scoreloop
	static void onUserDataFetched(void* cookie, SC_Error_t result);

private:
	//Passed in, shared. We don't own this.
	SC_Client_h mClient;
	//Handle for getting the user data, we own this.
	SC_UserController_h mUserController;
	//We request this, but don't need to own it.
	SC_User_h mUser;
};
