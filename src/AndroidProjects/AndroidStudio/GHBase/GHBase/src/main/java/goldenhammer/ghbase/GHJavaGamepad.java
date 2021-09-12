package goldenhammer.ghbase;

import android.annotation.SuppressLint;
import android.util.Log;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;

// place for gamepad handling functions.
public class GHJavaGamepad
{
	boolean mJoyActivated = false;
	
	protected void verifyJoyActivated(GHEngineInterface engineInterface)
	{
		if (mJoyActivated) return;
		// todo: handle losing the joystick.
		engineInterface.handleGamepadActive(0, 1);
	}
	
	// return -1 if the key event is not gamepad related, otherwise return the yrg keycode
	// return values much match GHKeyDef.h
	protected int getGHGamepadKeyCode(int keyCode)
	{
		if (keyCode == KeyEvent.KEYCODE_DPAD_UP) {
			return 12; // KEY_GAMEPAD_DPADUP
		}
		if (keyCode == KeyEvent.KEYCODE_DPAD_DOWN){
			return 13; // KEY_GAMEPAD_DPADDOWN
		}
		if (keyCode == KeyEvent.KEYCODE_DPAD_LEFT){
			return 14; // KEY_GAMEPAD_DPADLEFT
		}
		if (keyCode == KeyEvent.KEYCODE_DPAD_RIGHT){
			return 15; // KEY_GAMEPAD_DPADRIGHT
		}
		
		if (keyCode == KeyEvent.KEYCODE_BUTTON_A){
			return 0; // KEY_GP_A
		}
		if (keyCode == KeyEvent.KEYCODE_BUTTON_B){
			return 3; // KEY_GP_B
		}
		if (keyCode == KeyEvent.KEYCODE_BUTTON_X){
			return 1; // KEY_GP_X
		}
		if (keyCode == KeyEvent.KEYCODE_BUTTON_Y){
			return 2; // KEY_GP_Y
		}

		if (keyCode == KeyEvent.KEYCODE_BUTTON_R1) {
			return 6; // KEY_GAMEPADRIGHTBUMPER
		}
		if (keyCode == KeyEvent.KEYCODE_BUTTON_R2) {
			return 7; // KEY_GAMEPADRIGHTTRIGGER
		}
		if (keyCode == KeyEvent.KEYCODE_BUTTON_L1) {
			return 4; // KEY_GAMEPADLEFTBUMPER
		}
		if (keyCode == KeyEvent.KEYCODE_BUTTON_L1) {
			return 5; // KEY_GAMEPADLEFTTRIGGER
		}

		if (keyCode == KeyEvent.KEYCODE_BUTTON_THUMBL) {
			return 8; // KEY_GP_LEFTSTICKBUTTON
		}
		if (keyCode == KeyEvent.KEYCODE_BUTTON_THUMBR) {
			return 9; // KEY_GP_RIGHTSTICKBUTTON
		}
		
		return -1;
	}
	
	protected boolean handleGamepadKey(int key, float isPressed, GHEngineInterface engineInterface)
	{
		int yrgCode = getGHGamepadKeyCode(key);
		if (yrgCode == -1) {
			return false;
		}
		verifyJoyActivated(engineInterface);
		engineInterface.handleGamepadButtonChange(0, yrgCode, isPressed);
		return true;
	}
	
	// if event is a gamepad event that we want to handle, then return true.
	// otherwise return false and dispatchKeyEvent will send it on.
	protected boolean handleGamepadKeyEvent(KeyEvent event, GHEngineInterface engineInterface)
	{
		int keyCode = event.getKeyCode();
		int pressed = 0;
		if (event.getAction() == KeyEvent.ACTION_DOWN) {
			pressed = 1;
		}

		return handleGamepadKey(keyCode, pressed, engineInterface);
	}
	
	protected float getCenteredAxis(MotionEvent event, InputDevice inputDevice, int axis)
	{
		float val = event.getAxisValue(axis);
		final InputDevice.MotionRange motionRange = inputDevice.getMotionRange(axis, event.getSource());
		if (motionRange != null)
		{
			final float flat = motionRange.getFlat();
			if (Math.abs(val) < flat) {
				return 0;
			}
		}
		return val;
	}
	
	@SuppressLint("InlinedApi")
	protected void handleGamepadJoyEventDPad(MotionEvent event, GHEngineInterface engineInterface)
	{
		if (android.os.Build.VERSION.SDK_INT < 12) {
			// no controller support pre-honeycomb
			return;
		}
		
		InputDevice inputDevice = event.getDevice();

		float dpadX = getCenteredAxis(event, inputDevice, MotionEvent.AXIS_HAT_X);
		if (dpadX == 0)
		{
			handleGamepadKey(KeyEvent.KEYCODE_DPAD_LEFT, 0, engineInterface);
			handleGamepadKey(KeyEvent.KEYCODE_DPAD_RIGHT, 0, engineInterface);
		}
		else if (dpadX < 0)
		{
			handleGamepadKey(KeyEvent.KEYCODE_DPAD_LEFT, 1, engineInterface);
			handleGamepadKey(KeyEvent.KEYCODE_DPAD_RIGHT, 0, engineInterface);
		}
		else if (dpadX > 0)
		{
			handleGamepadKey(KeyEvent.KEYCODE_DPAD_LEFT, 0, engineInterface);
			handleGamepadKey(KeyEvent.KEYCODE_DPAD_RIGHT, 1, engineInterface);
		}
		
		float dpadY = getCenteredAxis(event, inputDevice, MotionEvent.AXIS_HAT_Y);
		if (dpadY == 0)
		{
			handleGamepadKey(KeyEvent.KEYCODE_DPAD_UP, 0, engineInterface);
			handleGamepadKey(KeyEvent.KEYCODE_DPAD_DOWN, 0, engineInterface);
		}
		else if (dpadY < 0)
		{
			handleGamepadKey(KeyEvent.KEYCODE_DPAD_UP, 1, engineInterface);
			handleGamepadKey(KeyEvent.KEYCODE_DPAD_DOWN, 0, engineInterface);
		}
		else if (dpadY > 0)
		{
			handleGamepadKey(KeyEvent.KEYCODE_DPAD_UP, 0, engineInterface);
			handleGamepadKey(KeyEvent.KEYCODE_DPAD_DOWN, 1, engineInterface);
		}
	}

	@SuppressLint("InlinedApi")
	protected boolean handleGamepadJoyEvent(MotionEvent event, GHEngineInterface engineInterface)
	{
		if (android.os.Build.VERSION.SDK_INT < 12) {
			// no controller support pre-honeycomb
			return false;
		}
		
		if ((event.getSource() & InputDevice.SOURCE_JOYSTICK) == InputDevice.SOURCE_JOYSTICK)
		{
			if (event.getAction() == MotionEvent.ACTION_MOVE)
			{
				verifyJoyActivated(engineInterface);
				InputDevice inputDevice = event.getDevice();
				
				float leftX = getCenteredAxis(event, inputDevice, MotionEvent.AXIS_X);
				float leftY = getCenteredAxis(event, inputDevice, MotionEvent.AXIS_Y);
				engineInterface.handleGamepadJoystickChange(0, 0, leftX, -1.0f * leftY);
				
				float rightX = getCenteredAxis(event, inputDevice, MotionEvent.AXIS_Z);
				float rightY = getCenteredAxis(event, inputDevice, MotionEvent.AXIS_RZ);
				engineInterface.handleGamepadJoystickChange(0, 1, rightX, rightY);

				handleGamepadJoyEventDPad(event, engineInterface);
				
				float lTrigger = getCenteredAxis(event, inputDevice, MotionEvent.AXIS_LTRIGGER);
				lTrigger += getCenteredAxis(event, inputDevice, MotionEvent.AXIS_BRAKE);
				handleGamepadKey(KeyEvent.KEYCODE_BUTTON_L2, lTrigger, engineInterface);
				float rTrigger = getCenteredAxis(event, inputDevice, MotionEvent.AXIS_RTRIGGER);
				rTrigger += getCenteredAxis(event, inputDevice, MotionEvent.AXIS_GAS);
				handleGamepadKey(KeyEvent.KEYCODE_BUTTON_R2, rTrigger, engineInterface);
				
				//Log.e("tag", "LeftJoy:"+leftX+","+leftY+" RightJoy:"+rightX+","+rightY + " LTrig:"+lTrigger + " RTrig:"+rTrigger);
				return true;
			}
		}

		return false;
	}
}
