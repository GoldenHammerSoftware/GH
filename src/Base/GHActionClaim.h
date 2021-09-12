#pragma once

//Simple widget to lock access to performing 
// some actions that might be mutually exclusive.
// (eg: button press within a single frame that can change the game or menu state)
//Claims are an int rather than a bool to handle cases where it may take multiple frames to clear.
class GHActionClaim
{
public:
	GHActionClaim(unsigned int numClaims = 1)
	{
		mNumClaims = numClaims;
	}


	inline bool claimAction(void) 
	{ 
		if (mClaims)
		{
			return false;
		}
		mClaims = mNumClaims;
		return true;
	}

	inline void clearClaim(void)
	{
		if (mClaims)
		{
			--mClaims;
		}
	}

private:
	unsigned int mClaims{ 0 };
	unsigned int mNumClaims{ 1 };
};
