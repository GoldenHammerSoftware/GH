// Copyright Golden Hammer Software
#include "GHInputStateKeySet.h"

void GHInputStateKeySet::copyValues(const GHInputStateKeySet& other)
{
	mKeys.clear();
	KeySet::const_iterator keyIter;
	for (keyIter = other.mKeys.begin(); keyIter != other.mKeys.end(); ++keyIter)
	{
		mKeys.insert(*keyIter);
	}
}

const float GHInputStateKeySet::getKeyState(unsigned int key) const
{
	KeySet::const_iterator keyIter = mKeys.find(key);
	if (keyIter == mKeys.end()) return 0;
	return keyIter->second;
}

void GHInputStateKeySet::setKeyState(unsigned int key, float val)
{
	mKeys[key] = val;
}
