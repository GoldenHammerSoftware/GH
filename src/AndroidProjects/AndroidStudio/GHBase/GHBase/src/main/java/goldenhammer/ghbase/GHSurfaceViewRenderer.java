package goldenhammer.ghbase;
import java.util.List;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.opengl.GLSurfaceView;

public class GHSurfaceViewRenderer implements GLSurfaceView.Renderer 
{
    private GHNativeInitializationFlag mInitializationFlag = null;
    public GHEngineInterface mEngineInterface;
	SensorManager mSensorMgr;
	GHSensorListener mSensorListener;
	int mScreenOffset;
	private String mExternalStoragePath;
	
	public GHSurfaceViewRenderer(GHEngineInterface engineInterface, 
			GHSensorListener sensorListener,
			SensorManager sensorMgr,
			GHNativeInitializationFlag initializationFlag,
			String externalStoragePath) 
	{
		android.util.Log.d("Output", "GHSurfaceViewRenderer created."); 
		mInitializationFlag = initializationFlag;
		mEngineInterface = engineInterface;
		mSensorMgr = sensorMgr;
		mSensorListener = sensorListener;
		mScreenOffset = 0;
		mExternalStoragePath = externalStoragePath;
	} 
	
	public void setScreenOffset(int val) { mScreenOffset = val; }
	
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    	android.util.Log.d("Output", "onSurfaceCreated");
    }

    public void onSurfaceChanged(GL10 gl, int w, int h) {
    	android.util.Log.d("Output", "onSurfaceChanged");

    	if (!mInitializationFlag.wasInitialized()){
    		mInitializationFlag.markInitialized();
    		int isTablet = 0;
    		mEngineInterface.launchNativeApp(w, h, mExternalStoragePath, mEngineInterface, isTablet, mEngineInterface.supportsIAP());
    		// if we don't wait until launchSBApp is called to register the listener,
    		//  we can get a threading crash.
            List<Sensor> sensorList = mSensorMgr.getSensorList(Sensor.TYPE_ACCELEROMETER);
            if (!sensorList.isEmpty()) {
            	mSensorMgr.registerListener(mSensorListener, sensorList.get(0), SensorManager.SENSOR_DELAY_GAME);
            }
    	}
    	else {
    		mEngineInterface.resizeWindow(w, h);
    	}
    }

    public void onDrawFrame(GL10 gl) {
		mEngineInterface.runNativeFrame();
    } 
}