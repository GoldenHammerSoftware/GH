// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"
#include "GHMath/GHTransform.h"
#include "GHInputStateKeySet.h"
#include <vector>

// Some structs used by input.
// This file exists to remove some dedependencies on GHInputState.h
namespace GHInputStructs
{
	enum InputEventType
	{
		IET_POINTERACTIVATE,
		IET_POINTERDEACTIVE,
		IET_POINTERMOVE,
		IET_KEYCHANGE,
		IET_GAMEPADKEYCHANGE,
		IET_GAMEPADPOINTERCHANGE,
		IET_ACCEL,
	};
	class InputEvent
	{
	public:
		InputEventType mType{ IET_KEYCHANGE };
		// index of the pointer, accel, gamepad, or keyset this is associated with.
		unsigned int mId{ 0 };
		// if accel event, mVal represents accel.
		// if pointer event, first two floats are pos.
		// if key event or gamepadkey, the first float is key, the second is keystate.
		// if gamepadpointer, first two floats are pos and third is index.
		GHPoint3 mVal;
		// pointer id of something that has claimed this event for use.
		// this value is why we need to pass in non const input states to different places.
		void* mClaim{ 0 };
	};
	typedef std::vector<InputEvent> InputEventList;

	struct Poseable
	{
		GHTransform mPosition{ GHTransform::IDENTITY };
		GHPoint3 mAngularVelocity{ 0,0,0 };
		GHPoint3 mLinearVelocity{ 0,0,0 };
		GHPoint3 mAngularAcceleration{ 0,0,0 };
		GHPoint3 mLinearAcceleration{ 0,0,0 };
		bool mActive{ false };
	};

	struct Gamepad
	{
		enum JoyIndex //applies to joysticks and posables
		{
			LEFT,
			RIGHT
		};

		enum Type
		{
			GPT_UNKNOWN, //most things are this until we need them, add more types as necessary
			GPT_VRTOUCH
		};

		std::vector<GHPoint2> mJoySticks;
		std::vector<Poseable> mPoseables;
		GHInputStateKeySet mButtons;
		Type mType{ GPT_UNKNOWN };
		bool mActive;
	};

	struct Pointer
	{
		// whether or not this pointer should be ignored.
		bool mActive{ false };
		// position in 0-1 screenspace.
		// if we're outside that space, active should likely be false.
		GHPoint2 mPosition;
		// index used for pointer claim
		unsigned int mPointerIndex{ 0 };
	};
	typedef std::vector<Pointer> PointerList;
}
