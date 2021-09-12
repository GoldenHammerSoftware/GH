package goldenhammer.ghbase;

public class GHNativeInitializationFlag
{
	private boolean mWasInitialized = false;
	public void markInitialized() { mWasInitialized = true; }
	public boolean wasInitialized() { return mWasInitialized; }
};