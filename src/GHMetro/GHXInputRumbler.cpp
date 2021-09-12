// Copyright Golden Hammer Software
#include "GHXInputRumbler.h"
#include "GHMath/GHMath.h"
#include "GHBaseIdentifiers.h"
#include "GHUtils/GHPropertyContainer.h"

#include "GHPlatform/win32/GHWin32Include.h"
//#define WIN32_LEAN_AND_MEAN
//#include "Windows.h"
#include "XInput.h"

void GHXInputRumbler::setPropertyContainer(GHPropertyContainer* props)
{
	mProps = props;
	if (props)
	{
		props->setProperty(GHBaseIdentifiers::RUMBLESUPPORTED, true);
	}
}

void GHXInputRumbler::setRumble(int controllerIndex, float leftRumble, float rightRumble)
{
	if (mProps && (bool)mProps->getProperty(GHBaseIdentifiers::RUMBLEDISABLED)) {
		return;
	}

	leftRumble = GHMath::clamp(0.f, 1.f, leftRumble);
	rightRumble = GHMath::clamp(0.f, 1.f, rightRumble);

	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	const static float kMaxRumble = 65535.f; //from microsoft online docs
	vibration.wLeftMotorSpeed = WORD(leftRumble*kMaxRumble);
	vibration.wRightMotorSpeed = WORD(rightRumble*kMaxRumble);
	XInputSetState(controllerIndex, &vibration);
}
