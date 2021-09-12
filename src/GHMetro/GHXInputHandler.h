// Copyright Golden Hammer Software
#pragma once

class GHInputState;

// util class for handling xinput gamepads.
class GHXInputHandler
{
public:
	static void pollGamepads(GHInputState& inputState);
};
