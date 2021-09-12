package goldenhammer.ghbase;

import java.util.ArrayList;

import android.content.Intent;

public class GHLifecycleHandlerList implements GHLifecycleHandler
{
	private ArrayList<GHLifecycleHandler> mHandlers = new ArrayList<GHLifecycleHandler>();
	
	public void addHandler(GHLifecycleHandler handler)
	{
		mHandlers.add(handler);
	}
	
	@Override
	public void onStart() {
		for (int i = 0; i < mHandlers.size(); ++i)
		{
			GHLifecycleHandler handler = mHandlers.get(i);
			handler.onStart();
		}
	}

	@Override
	public void onStop() {
		for (int i = mHandlers.size()-1; i >= 0; --i)
		{
			GHLifecycleHandler handler = mHandlers.get(i);
			handler.onStop();
		}
	}

	@Override
	public void onDestroy() {
		for (int i = mHandlers.size()-1; i >= 0; --i)
		{
			GHLifecycleHandler handler = mHandlers.get(i);
			handler.onDestroy();
		}
	}

	@Override
	public boolean onBackPressed() {
		for (int i = 0; i < mHandlers.size(); ++i)
		{
			GHLifecycleHandler handler = mHandlers.get(i);
			if (handler.onBackPressed())
			{
				return true;
			}
		}
		return false;
	}
	
	public void onResume() 
	{
		for (int i = mHandlers.size()-1; i >= 0; --i)
		{
			GHLifecycleHandler handler = mHandlers.get(i);
			handler.onResume();
		}
	}
	public void onPause() 
	{
		for (int i = mHandlers.size()-1; i >= 0; --i)
		{
			GHLifecycleHandler handler = mHandlers.get(i);
			handler.onPause();
		}
	}

	@Override
	public void onWindowFocusChanged(boolean hasFocus)
	{
		//Iterate forwards if we have focus, backwards if we don't.
		int iter, end, inc;
		if (hasFocus) {
			iter = 0;
			end = mHandlers.size();
			inc = 1;
		}
		else {
			iter = mHandlers.size()-1;
			end = -1;
			inc = -1;
		}
		for (; iter != end; iter += inc)
		{
			GHLifecycleHandler handler = mHandlers.get(iter);
			handler.onWindowFocusChanged(hasFocus);
		}
	}

	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data) 
	{
		for (int i = 0; i < mHandlers.size(); ++i)
		{
			GHLifecycleHandler handler = mHandlers.get(i);
			handler.onActivityResult(requestCode, resultCode, data);
		}
	}
	
};