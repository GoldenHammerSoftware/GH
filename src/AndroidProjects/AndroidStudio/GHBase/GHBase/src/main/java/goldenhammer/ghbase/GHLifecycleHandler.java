package goldenhammer.ghbase;

import android.content.Intent;

public interface GHLifecycleHandler
{
	public void onStart();
	public void onStop();
	public void onDestroy();	
	public void onResume();
	public void onPause();
	public boolean onBackPressed();
	public void onWindowFocusChanged(boolean hasFocus);
	public void onActivityResult(int requestCode, int resultCode, Intent data);
}