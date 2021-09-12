package goldenhammer.ghbase;

import java.io.FileDescriptor;
import java.io.IOException;
import java.util.HashMap;

import android.annotation.SuppressLint;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.media.AudioManager;
import android.media.SoundPool;
import android.util.Log;

// a class to manage background loading of the sound pool sound.
class GHSoundPoolSoundDescriptor
{
	private int mSoundID;
	private int mLength; //pass -1 if we don't care
	
	private class SoundLoader implements Runnable
	{
		private GHSoundPoolSoundDescriptor mParent;
		private AssetManager mAssetMgr;
		private String mFilename;
		private SoundPool mSoundPool;
		private GHSoundPoolConfig mSoundPoolConfig;
		
		SoundLoader(GHSoundPoolSoundDescriptor parent, AssetManager assetMgr, 
				String filename, SoundPool soundPool, GHSoundPoolConfig soundPoolConfig) 
		{
			mParent = parent;
			mAssetMgr = assetMgr;
			mFilename = filename;
			mSoundPool = soundPool;
			mSoundPoolConfig = soundPoolConfig;
		}
		
		@Override
		public void run() {
	    	try {
	    		AssetFileDescriptor afd = mAssetMgr.openFd(mFilename);
	    		int soundID = mSoundPool.load(afd, 1);
	    		int length = mSoundPoolConfig.getLength(mFilename);
	    		mParent.loadFinished(soundID,  length);
	    	}
	    	catch (IOException e) {
	    		Log.e("tag", "Failed to load sound file: " + e.getLocalizedMessage());
	    	}
		}
	}
	
	public GHSoundPoolSoundDescriptor(AssetManager assetMgr, String filename, 
			SoundPool soundPool, GHSoundPoolConfig soundPoolConfig)
	{
		mSoundID = -1;
		mLength = -1;
		
		SoundLoader loader = new SoundLoader(this, assetMgr, filename, soundPool, soundPoolConfig); 
		Thread thread = new Thread(loader);
		thread.start();
	}

	public synchronized void loadFinished(int soundID, int length) {
		mSoundID = soundID;
		mLength = length;
	}
	public synchronized boolean isInitialized() { return mSoundID != -1; }
	public synchronized int getSoundID() { return mSoundID; }
	public synchronized int getLength() { return mLength; }
}

class GHSoundPoolSound
{
	private SoundPool mSoundPool;
	private GHSoundPoolSoundDescriptor mDesc;
	private int mStreamID;
	private float mLeftVolume;
	private float mRightVolume;
	private float mPlaybackRate;
	private boolean mIsLooping;
	private boolean mIsPaused;
	private long mStartTime;
	private long mPauseTime;
	
	public String mName;
	
	public GHSoundPoolSound(SoundPool soundPool, GHSoundPoolSoundDescriptor desc)
	{
		mSoundPool = soundPool;
		mDesc = desc;
		
		mLeftVolume = 1.0f;
		mRightVolume = 1.0f;
		mPlaybackRate = 1.0f;
		mIsLooping = false;
		mIsPaused = false;
		
		mStreamID = 0;
		mStartTime = 0;
		mPauseTime = 0;
	}
	
	public void unload()
	{
		mStreamID = 0;
		mIsPaused = false;
		// todo: handle destruction while initializing
		if (!mDesc.isInitialized()) return;
		mSoundPool.unload(mDesc.getSoundID());
	} 
	
	public void start()
	{
		if (!mDesc.isInitialized()) return;
		
		if (mIsPaused)
		{
			mSoundPool.resume(mStreamID);
			mIsPaused = false;
			mStartTime += new java.util.Date().getTime() - mPauseTime;
		}
		else
		{
			int loop = mIsLooping ? -1 : 0;
			mStreamID = mSoundPool.play(mDesc.getSoundID(), mLeftVolume, mRightVolume, 1, loop, mPlaybackRate);
			mStartTime = new java.util.Date().getTime();
		}
	}
	
	public void stop()
	{
		if (mStreamID == 0) { return; }
		
		mSoundPool.stop(mStreamID);
		mStreamID = 0;
		mIsPaused = false;
		mStartTime = 0;
		mPauseTime = 0;
	}
	
	public void pause()
	{
		if (mStreamID == 0 || mIsPaused) { return; }
		
		mSoundPool.pause(mStreamID);
		mIsPaused = true;
		mPauseTime = new java.util.Date().getTime();
	}
	
	public void setVolume(float leftVolume, float rightVolume)
	{
		mLeftVolume = leftVolume;
		mRightVolume = rightVolume;
		if (mStreamID != 0) {
			mSoundPool.setVolume(mStreamID, leftVolume, rightVolume);
		}
	}
	
	public void setLooping(boolean looping)
	{
		mIsLooping = looping;
		if (mStreamID != 0) {
			mSoundPool.setLoop(mStreamID, looping ? -1 : 0);	
		}
	}
	
	public void setPitch(float pitch)
	{
		mPlaybackRate = pitch;
		if (mStreamID != 0) {
			mSoundPool.setRate(mStreamID, pitch);
		}
	}
	
	public boolean isPlaying()
	{
		if (!mDesc.isInitialized()) return false;
		
		//First check that we started playing and are not paused.
		//After that, looping sounds are always playing.
		//Non-looping sounds with unspecified lengths are considered to never be playing.
		//For ones with specified lengths, we check the difference between the current time and the play start time.
		
		return mStreamID != 0 && !mIsPaused && 
			( mIsLooping || (mDesc.getLength() != -1 && 
			(new java.util.Date().getTime() - mStartTime < mDesc.getLength())));
	}
}

@SuppressLint("NewApi")
class GHSoundPool
{
	private SoundPool mSoundPool;
	private GHSoundPoolConfig mConfig;
	
	public GHSoundPool(GHSoundPoolConfig config)
	{
		int maxStreams = 32; //arbitrary
		mSoundPool = new SoundPool(maxStreams, AudioManager.STREAM_MUSIC, 0);	
		mConfig = config;
		if (mConfig == null)
		{
			mConfig = new GHSoundPoolConfig(); //empty, but non-null
		}
	}
	
	GHSoundPoolSound createSound(AssetManager assetMgr, String filename)
	{	
		GHSoundPoolSoundDescriptor desc = new GHSoundPoolSoundDescriptor(assetMgr, filename, 
				mSoundPool, mConfig);
		return new GHSoundPoolSound(mSoundPool, desc);
	}
	
	void destroySound(GHSoundPoolSound sound)
	{
		sound.unload();
	}
}
