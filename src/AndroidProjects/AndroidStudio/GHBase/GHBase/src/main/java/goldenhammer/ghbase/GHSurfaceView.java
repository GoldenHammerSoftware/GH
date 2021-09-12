package goldenhammer.ghbase;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView;
import android.util.Log;

public class GHSurfaceView extends GLSurfaceView
{
	@SuppressLint("NewApi")
	public GHSurfaceView(Activity activity, GHEngineInterface engInterface)
	{
		super(activity);
		mEngineInterface = engInterface;
		
		boolean hasES2 = checkES2Support(activity);
		if (hasES2)
		{
			setEGLContextClientVersion(2); //use OGL 2.0
			setEGLContextFactory(new GHES2ContextFactory());
		}
		mEngineInterface.setSupportsES2(hasES2);
	} 
	
	public boolean onTouchEvent(android.view.MotionEvent event)
	{
		final int action = event.getAction();
		int actionIndex = (action & android.view.MotionEvent.ACTION_POINTER_INDEX_MASK) >> android.view.MotionEvent.ACTION_POINTER_INDEX_SHIFT;;
		int pointerId = event.getPointerId(actionIndex);
		
	    switch (action & android.view.MotionEvent.ACTION_MASK) 
	    {
	    	case android.view.MotionEvent.ACTION_DOWN:
	    	case android.view.MotionEvent.ACTION_POINTER_DOWN: {
				mEngineInterface.handleTouchStart(pointerId);
				break;
	    	}
	    	case android.view.MotionEvent.ACTION_UP:
	    	case android.view.MotionEvent.ACTION_CANCEL:
	    	case android.view.MotionEvent.ACTION_POINTER_UP: {
				mEngineInterface.handleTouchEnd(pointerId);
				// don't send the touch position after mouse up.
				return true;
	    	}
	    	case android.view.MotionEvent.ACTION_MOVE:
	    		break;
	    	default:
	    		return true;
	    }

	    final int pointerCount = event.getPointerCount();
		for (int i = 0; i < pointerCount; ++i)
		{
			pointerId = event.getPointerId(i);
			mEngineInterface.handleTouchPos(pointerId, event.getX(i), event.getY(i));
		}
		return true;
	}
	
	private boolean checkES2Support(Activity activity)
	{
		if (android.os.Build.VERSION.SDK_INT > 7)
		{
			// this will handle the lack of es2 on pre-2.2 devices.
			// it won't catch devices that are 2.2+ and don't support es2 though (emulator).
			
	        ActivityManager am = (ActivityManager)activity.getSystemService(Context.ACTIVITY_SERVICE);
	        ConfigurationInfo info = am.getDeviceConfigurationInfo();
	        return (info.reqGlEsVersion >= 0x20000);
		}
		return false;
	}
	
	private static class GHES2ContextFactory implements GLSurfaceView.EGLContextFactory
	{		
		private static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
		public EGLContext createContext(EGL10 egl, EGLDisplay display,
				EGLConfig eglConfig)
		{
			//Log.w("tag", "creating OpenGL ES 2.0 context");
			checkEglError("Before eglCreateContext", egl);
			int[] attrib_list = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE };
			EGLContext context = egl.eglCreateContext(display, eglConfig, EGL10.EGL_NO_CONTEXT, attrib_list);
			checkEglError("After eglCreateContext", egl);
			return context;
		}

		public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context) 
		{
			egl.eglDestroyContext(display, context);
		}
		
		private static void checkEglError(String prompt, EGL10 egl)
		{
	        int error;
	        while ((error = egl.eglGetError()) != EGL10.EGL_SUCCESS) 
	        {
	            Log.e("tag", String.format("%s: EGL error: 0x%x", prompt, error));
	        }
		}
	}

    public GHEngineInterface mEngineInterface;
}
