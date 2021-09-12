package goldenhammer.ghbase;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.util.Log;

public class GHSensorListener implements SensorEventListener
{
	public GHSensorListener(GHEngineInterface engInterface, boolean landscapeDefault)
	{
		mEngineInterface = engInterface;
		mLandscapeDefault = landscapeDefault;
	}
	
	public void onAccuracyChanged(Sensor sensor, int accuracy)
	{
	
	}
	
	float modifyAccelValue(float hardwareValue)
	{
		return -hardwareValue / 10.f;
	}
	
	public void onSensorChanged(SensorEvent event)
	{
		float x = modifyAccelValue(event.values[0]);
		float y = modifyAccelValue(event.values[1]);
		float z = modifyAccelValue(event.values[2]);

		if (mLandscapeDefault)
		{
			mEngineInterface.handleAcceleration(x, y, z);
		}
		else
		{
			mEngineInterface.handleAcceleration(-y, x, z);
		}
	}
	
	private GHEngineInterface mEngineInterface;
	private boolean mLandscapeDefault;
}