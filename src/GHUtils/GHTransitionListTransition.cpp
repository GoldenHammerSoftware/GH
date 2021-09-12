// Copyright Golden Hammer Software
#include "GHTransitionListTransition.h"
#include <cassert>

GHTransitionListTransition::GHTransitionListTransition(size_t sizeReserve)
{
	mTransitions.reserve(sizeReserve);
}

GHTransitionListTransition::~GHTransitionListTransition(void)
{
	std::vector<GHTransition*>::reverse_iterator iter = mTransitions.rbegin();
	while (iter != mTransitions.rend())
	{
		delete *iter;
		++iter;
	}

	iter = mReverseTransitions.rbegin();
	while (iter != mReverseTransitions.rend())
	{
		delete* iter;
		++iter;
	}
}

void GHTransitionListTransition::addTransition(GHTransition* trans)
{
	assert(trans);
	mTransitions.push_back(trans);
}

void GHTransitionListTransition::addReverseTransition(GHTransition* trans)
{
	assert(trans);
	mReverseTransitions.push_back(trans);
}

void GHTransitionListTransition::activate(void)
{
	std::vector<GHTransition*>::iterator iter = mReverseTransitions.begin();
	while (iter != mReverseTransitions.end())
	{
		(*iter)->deactivate();
		++iter;
	}

	iter = mTransitions.begin();
	while (iter != mTransitions.end())
	{
		(*iter)->activate();
		++iter;
	}
}

void GHTransitionListTransition::deactivate(void)
{
	std::vector<GHTransition*>::reverse_iterator iter = mTransitions.rbegin();
	while (iter != mTransitions.rend())
	{
		(*iter)->deactivate();
		++iter;
	}

	iter = mReverseTransitions.rbegin();
	while (iter != mReverseTransitions.rend())
	{
		(*iter)->activate();
		++iter;
	}
}
