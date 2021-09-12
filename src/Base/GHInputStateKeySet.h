// Copyright Golden Hammer Software
#pragma once

#include <map>

class GHInputStateKeySet
{
public:
	void copyValues(const GHInputStateKeySet& other);
	const float getKeyState(unsigned int key) const;
	void setKeyState(unsigned int key, float val);

private:
	// map of key identifier to state. 0 = unpressed, 1 = fully pressed.
	typedef std::map<unsigned int, float> KeySet;
	KeySet mKeys;
};
