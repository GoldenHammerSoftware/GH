package goldenhammer.ghbase;
import java.io.File;
import java.util.List;

import goldenhammer.ghbase.util.SystemUiHider;

import android.annotation.SuppressLint;
import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Intent;
import android.content.res.Configuration;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.util.Log;
import android.view.Display;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.View;
import android.view.WindowManager;

/**
 * An example full-screen activity that shows and hides the system UI (i.e.
 * status bar and navigation/system bar) with user interaction.
 * 
 * @see SystemUiHider
 */
public class GHBaseActivity extends Activity {
	
	private boolean mPaused = false;
	private boolean mFocused = false;
	protected GLSurfaceView mGLView;
	protected SensorManager mSensorMgr;
	protected GHSensorListener mSensorListener;
	protected GHEngineInterface mEngineInterface;
	protected GHLifecycleHandlerList mLifecycleHandler = new GHLifecycleHandlerList();
	protected GHNativeInitializationFlag mInitializationFlag = new GHNativeInitializationFlag();
	protected GHJavaGamepad mGamepadHandler = new GHJavaGamepad();
	
	private void handleFocusOrPauseChange(boolean newFocus, boolean newPause) {
		Log.e("tag", "in handleFocusOrPauseChange newFocus = " + newFocus
				+ ", newPause = " + newPause);
		Log.e("tag", "oldFocus = " + mFocused + " oldPause = " + mPaused); 

		if (mPaused || !mFocused) { 
			mPaused = newPause;
			mFocused = newFocus;
			if (!mPaused && mFocused) {
				Log.e("tag", "handleJavaResume");
				mGLView.onResume(); 
				mEngineInterface.handleJavaResume();
			}
		} else if (!mPaused && mFocused) {
			mPaused = newPause;
			mFocused = newFocus;
			if (mPaused || !mFocused) {
				Log.e("tag", "handleJavaPause");
				mGLView.onPause();
				mEngineInterface.handleJavaPause();
			}
		} else {
			mPaused = newPause;
			mFocused = newFocus;
		}
	}
	
	public void onWindowFocusChanged(boolean hasFocus) {
		Log.e("GHNative", "In onWindowFocusChanged");
		super.onWindowFocusChanged(hasFocus);
		if (mLifecycleHandler != null) {
			mLifecycleHandler.onWindowFocusChanged(hasFocus);
		}
		if (mInitializationFlag.wasInitialized()) {
			handleFocusOrPauseChange(hasFocus, mPaused);
		}
		mFocused = hasFocus;
		Log.e("tag", "end onWindowFocusChanged");
	}

	protected void onPause() {
		Log.e("tag", "In onPause");

		super.onPause();
		boolean isPaused = true;
		handleFocusOrPauseChange(mFocused, isPaused);
        if (mLifecycleHandler != null) { mLifecycleHandler.onPause(); }
		Log.e("tag", "end onPause");
	}

	protected void onResume() {
		Log.e("tag", "In onResume");

		super.onResume();
		boolean isPaused = false;
		handleFocusOrPauseChange(mFocused, isPaused);
        if (mLifecycleHandler != null) { mLifecycleHandler.onResume(); }
		Log.e("tag", "end onResume");
	}

	public void onConfigurationChanged(Configuration newConfig) {
		Log.e("tag", "In onConfigurationChanged");
		super.onConfigurationChanged(newConfig);
		Log.e("tag", "end onConfigurationChanged");
	}

	protected void onStart() {
		super.onStart();
		if (mLifecycleHandler != null) {
			mLifecycleHandler.onStart();
		}
	}

	protected void onStop() {
		Log.e("tag", "In onStop");
		super.onStop();
		if (mLifecycleHandler != null) {
			mLifecycleHandler.onStop();
		}
		Log.e("tag", "end onStop");
	}

	protected void onDestroy() {
		super.onDestroy();
		Log.e("tag", "In onDestroy");

		List<Sensor> sensorList = mSensorMgr
				.getSensorList(Sensor.TYPE_ACCELEROMETER);
		if (!sensorList.isEmpty()) {
			mSensorMgr.unregisterListener(mSensorListener, sensorList.get(0));
		}
		mEngineInterface.handleJavaShutdown();

		if (mLifecycleHandler != null) {
			mLifecycleHandler.onDestroy();
		}
	}

	@SuppressLint("NewApi")
	protected void enableLightsOut(final View view) {
		if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.HONEYCOMB) {
			view.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LOW_PROFILE);

			view.setOnSystemUiVisibilityChangeListener(new View.OnSystemUiVisibilityChangeListener() {
				public void onSystemUiVisibilityChange(int visibility) {
					if (visibility == 0) {
						Runnable rehideRunnable = new Runnable() {
							public void run() {
								view.setSystemUiVisibility(View.SYSTEM_UI_FLAG_VISIBLE);
								view.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LOW_PROFILE);
							}
						};
						Handler rehideHandler = new Handler();
						rehideHandler.postDelayed(rehideRunnable, 2000);
					}
				}
			});
		}
	}

	protected void setupWindowFlags() {
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);

		getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON,
				WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
	}

	@SuppressLint("NewApi")
	protected boolean calcLandscapeDefault() {
		boolean landscapeDefault = false;
		if (android.os.Build.VERSION.SDK_INT < android.os.Build.VERSION_CODES.FROYO) {
			return landscapeDefault;
		}
		Display display = getWindowManager().getDefaultDisplay();
		int rotation = display.getRotation();
		if (display.getWidth() > display.getHeight()) {
			if (rotation == Surface.ROTATION_0
					|| rotation == Surface.ROTATION_180) {
				landscapeDefault = true;
			}
		} else {
			if (rotation == Surface.ROTATION_90
					|| rotation == Surface.ROTATION_270) {
				landscapeDefault = true;
			}
		}
		return landscapeDefault;
	}
 
	public void onBackPressed() {
		if (mLifecycleHandler != null) {
			if (mLifecycleHandler.onBackPressed()) {
				return;
			}
		}

		if (mEngineInterface != null) {
			mEngineInterface.handleBackPressed();
		} else {
			super.onBackPressed();
		}
	}
	
   	public boolean dispatchKeyEvent(KeyEvent event)
   	{
   		if (mEngineInterface != null && mGamepadHandler.handleGamepadKeyEvent(event, mEngineInterface)) {
   			return true;
   		}
   		return super.dispatchKeyEvent(event);
   	}
   	
   	public boolean dispatchGenericMotionEvent(MotionEvent ev)
   	{
   		if (mEngineInterface != null && mGamepadHandler.handleGamepadJoyEvent(ev, mEngineInterface)) {
   			return true;
   		}
   		return super.dispatchGenericMotionEvent(ev);
   	}
   	
	public void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		if (mLifecycleHandler != null) {
			mLifecycleHandler.onActivityResult(requestCode, resultCode, data);
		}
		super.onActivityResult(requestCode, resultCode, data);
	}
    
    protected String getExternalStoragePath() 
    {
    	File externalStorageFile = Environment.getExternalStorageDirectory();
    	String externalStoragePath = externalStorageFile.getAbsolutePath();
		externalStoragePath += "/trickshotbowling/";
		File appDirectory = new File(externalStoragePath);
		appDirectory.mkdirs(); 
		return externalStoragePath;
    }
}
