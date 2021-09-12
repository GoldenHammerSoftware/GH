// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"

// Sets a value reference on enter and on exit.
template< typename T >
class GHValueSetterTransition : public GHTransition
{
public:
	GHValueSetterTransition(T& target, const T& enterVal, const T& exitVal)
		: mTarget(target), mEnterVal(enterVal), mExitVal(exitVal)
	{
	}

	virtual void activate(void)
	{ 
		mTarget = mEnterVal;
	}

	virtual void deactivate(void)
	{
		mTarget = mExitVal;
	}

private:
	T& mTarget;
	T mEnterVal;
	T mExitVal;
};
