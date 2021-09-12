// Copyright Golden Hammer Software
#pragma once
#include "GHUtils/GHTransition.h"
#include <vector>

//A transition which contains a list of transitions - for aggregating
// multiple transitions in a place where you can only fit one
class GHTransitionListTransition : public GHTransition
{
public:
	GHTransitionListTransition(size_t sizeReserve = 0);
	virtual ~GHTransitionListTransition(void);

	void addTransition(GHTransition* trans);

	//Call deactivate in activate and activate in deactivate
	void addReverseTransition(GHTransition* trans);

	virtual void activate(void);
	virtual void deactivate(void);

private:
	std::vector<GHTransition*> mTransitions;
	std::vector<GHTransition*> mReverseTransitions;
};
