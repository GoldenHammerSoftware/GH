// Copyright Golden Hammer Software
#include "GHPlatform/win32/GHWin32Include.h"
//#define WIN32_LEAN_AND_MEAN
//#include "Windows.h"
#include "XInput.h"

#include "GHXInputHandler.h"
#include "Base/GHInputState.h"
#include "Base/GHKeyDef.h"
#include "GHPlatform/GHDebugMessage.h"

static void createNormalizedJoystickValue(int inputX, int inputY, float& outputX, float& outputY)
{
	const float INPUT_DEADZONE = 10000;

	//determine how far the controller is pushed
	float magnitude = (float)sqrt(inputX*inputX + inputY*inputY);

	//determine the direction the controller is pushed
	float normalizedLX = inputX / magnitude;
	float normalizedLY = inputY / magnitude;

	float normalizedMagnitude = 0;

	//check if the controller is outside a circular dead zone
	if (magnitude > INPUT_DEADZONE)
	{
		//clip the magnitude at its expected maximum value
		if (magnitude > 32767) magnitude = 32767;

		//adjust magnitude relative to the end of the dead zone
		magnitude -= INPUT_DEADZONE;

		//optionally normalize the magnitude with respect to its expected range
		//giving a magnitude value of 0.0 to 1.0
		normalizedMagnitude = magnitude / (32767 - INPUT_DEADZONE);

		outputX = normalizedLX*normalizedMagnitude;
		outputY = normalizedLY*normalizedMagnitude;
		//GHDebugMessage::outputString("joystick %f %f", outputX, outputY);
	}
	else //if the controller is in the deadzone zero out the magnitude
	{
		magnitude = 0.0;
		normalizedMagnitude = 0.0;
		outputX = 0.0;
		outputY = 0.0;
	}
}

static void handleGamepadButton(GHInputState& state, int gamepadId, int key, bool pressed)
{
	if (state.getGamepad(gamepadId).mButtons.getKeyState(key)) {
		if (!pressed) {
			state.handleGamepadButtonChange(gamepadId, key, pressed);
		}
	}
	else if (pressed) {
		state.handleGamepadButtonChange(gamepadId, key, pressed);
	}
}

void GHXInputHandler::pollGamepads(GHInputState& inputState)
{
	DWORD dwResult;
	for (DWORD i = 0; i < XUSER_MAX_COUNT && i < inputState.getNumGamepads(); i++)
	{
		if (inputState.getGamepadOwner(i)) // assume a gamepad owner is always not us.
		{
			continue;
		}

		const GHInputStructs::Gamepad& ipad = inputState.getGamepad(i);
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		// Simply get the state of the controller from XInput.
		dwResult = XInputGetState(i, &state);

		if (dwResult == ERROR_SUCCESS)
		{
			// Controller is connected 
			if (!ipad.mActive)
			{
				inputState.handleGamepadActive(i, true);
			}

			float leftX, leftY = 0;
			createNormalizedJoystickValue(state.Gamepad.sThumbLX, state.Gamepad.sThumbLY, leftX, leftY);
			inputState.handleGamepadJoystickChange(i, 0, GHPoint2(leftX, leftY));

			float rightX, rightY = 0;
			createNormalizedJoystickValue(state.Gamepad.sThumbRX, state.Gamepad.sThumbRY, rightX, rightY);
			inputState.handleGamepadJoystickChange(i, 1, GHPoint2(rightX, rightY));

			bool aPressed = (state.Gamepad.wButtons & XINPUT_GAMEPAD_A);
			handleGamepadButton(inputState, i, GHKeyDef::KEY_GP_A, aPressed);
			bool bPressed = (state.Gamepad.wButtons & XINPUT_GAMEPAD_B);
			handleGamepadButton(inputState, i, GHKeyDef::KEY_GP_B, bPressed);
			bool xPressed = (state.Gamepad.wButtons & XINPUT_GAMEPAD_X);
			handleGamepadButton(inputState, i, GHKeyDef::KEY_GP_X, xPressed);
			bool yPressed = (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y);
			handleGamepadButton(inputState, i, GHKeyDef::KEY_GP_Y, yPressed);
			bool leftTrigger = (state.Gamepad.bLeftTrigger > 150);
			handleGamepadButton(inputState, i, GHKeyDef::KEY_GP_LEFTTRIGGER, leftTrigger);
			bool rightTrigger = (state.Gamepad.bRightTrigger > 150);
			handleGamepadButton(inputState, i, GHKeyDef::KEY_GP_RIGHTTRIGGER, rightTrigger);
			bool leftBumper = (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
			handleGamepadButton(inputState, i, GHKeyDef::KEY_GP_LEFTBUMPER, leftBumper);
			bool rightBumper = (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
			handleGamepadButton(inputState, i, GHKeyDef::KEY_GP_RIGHTBUMPER, rightBumper);
			bool leftStickButton = (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
			handleGamepadButton(inputState, i, GHKeyDef::KEY_GP_LEFTSTICKBUTTON, leftStickButton);
			bool rightStickButton = (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
			handleGamepadButton(inputState, i, GHKeyDef::KEY_GP_RIGHTSTICKBUTTON, rightStickButton);
			bool dPadUp = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP);
			handleGamepadButton(inputState, i, GHKeyDef::KEY_GP_DPADUP, dPadUp);
			bool dPadDown = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
			handleGamepadButton(inputState, i, GHKeyDef::KEY_GP_DPADDOWN, dPadDown);
			bool dPadLeft = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
			handleGamepadButton(inputState, i, GHKeyDef::KEY_GP_DPADLEFT, dPadLeft);
			bool dPadRight = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
			handleGamepadButton(inputState, i, GHKeyDef::KEY_GP_DPADRIGHT, dPadRight);

			bool startButton = (state.Gamepad.wButtons & XINPUT_GAMEPAD_START);
			handleGamepadButton(inputState, i, GHKeyDef::KEY_GP_START, startButton);
			bool gpbackButton = (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK);
			handleGamepadButton(inputState, i, GHKeyDef::KEY_GP_BACK, gpbackButton);
		}
		else
		{
			// Controller is not connected 
			if (ipad.mActive)
			{
				inputState.handleGamepadActive(i, false);
			}
		}
	}
}
