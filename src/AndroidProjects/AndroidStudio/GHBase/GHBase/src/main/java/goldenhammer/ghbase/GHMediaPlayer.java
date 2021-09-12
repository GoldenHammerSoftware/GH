package goldenhammer.ghbase;

import java.io.FileDescriptor;
import java.io.IOException;

import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.util.Log;
import android.media.MediaPlayer;

import java.util.ArrayList;

class GHMediaPlayerInfo 
{
	public MediaPlayer mMediaPlayer;
	public String mFilename;
	public ArrayList<GHMediaPlayerInfo> mMediaPlayers;
	
	private boolean mNeedsPrepare = false;
	private boolean mStartRequested = false;
	private boolean mIsPlaying = false;
	private float mLeftVolume = 1;
	private float mRightVolume = 1;
	private boolean mLooping = false;
	private boolean mIsPreparing = false;
	
	private AssetFileDescriptor mAFD = null;
	private FileDescriptor mFD = null;
	private long mOffset = 0;
	private long mLength = 0;

	public void shutdown()
	{
		if (mMediaPlayer != null)
		{
			mMediaPlayer.release();
		}
		if (mAFD != null) {
			try {
				mAFD.close();
			}
			catch (IOException e) {
				Log.e("tag", "Failed file handle close.");
			}
		}
	}
	
    public static int getIndexOfMediaPlayer(ArrayList<GHMediaPlayerInfo > list, MediaPlayer player) 
    {
    	int index = 0;
    	for(; index < list.size(); ++index) 
    	{
    		if (list.get(index).mMediaPlayer == player) {
    			return index;
    		}
    	}
    	return -1;
    }

    public void setNeedsPrepare()
    {
    	mNeedsPrepare = true;
    }
    
    public void prepare()
    {
    	if (mIsPreparing) return;
		try {
			mIsPreparing = true;
			mMediaPlayer.prepareAsync();
		}
		catch(IllegalStateException e) {
			Log.e("tag", "IllegalStateException wtf? " + e.getStackTrace());
			// this causes an infinite loop crash.
			//setDataSource();
		}
    }
    
    public void handlePrepared()
    {
    	mIsPreparing = false;
    	mNeedsPrepare = false;
    	if (mStartRequested) {
    		startInternal();
    	}
    }
    
    public void loadFileDescriptor(AssetManager assetMgr)
    {
    	try {
    		mAFD = assetMgr.openFd(mFilename);
    		mFD = mAFD.getFileDescriptor();
    		mOffset = mAFD.getStartOffset();
    		mLength = mAFD.getLength();
    	}
    	catch (IOException e) {
    		Log.e("tag", "Failed file descriptor load.");
    	}
    }
    
    public void setDataSource() 
    {
    	try {
    		mMediaPlayer.reset();
    		mMediaPlayer.setDataSource(mFD, mOffset, mLength);
    	}
    	catch (IOException e) {
    		Log.e("tag", "Failed setDataSource! " + e.getMessage());
    	}
    	catch (IllegalStateException e) {
    		Log.e("tag", "Illegal State Exception in setDataSource");
    	}
    	setNeedsPrepare();
    }

    public void start() 
    {
    	if (mIsPlaying) {
    		return;
    	}
    
    	if (mNeedsPrepare) {
    		mStartRequested = true;
    		prepare();
    		return;
    	}
    	
    	startInternal();
    }
    
    private void startInternal()
    {
    	mStartRequested = false;
    	mIsPlaying = true;
    	mMediaPlayer.start();
		mMediaPlayer.setVolume(mLeftVolume, mRightVolume);
		mMediaPlayer.setLooping(mLooping);
    }

    public void stop() 
    {
    	mStartRequested = false;
    	if (mIsPlaying) 
    	{
    		mIsPlaying = false;
    		mMediaPlayer.stop();
    		setDataSource();
    	}
    }
    
    public void pause() 
    {
    	if (mIsPlaying) 
    	{
    		mIsPlaying = false;
    		mMediaPlayer.pause();
    	}
    }
    
    public void setVolume(float leftVolume, float rightVolume) 
    {
    	mLeftVolume = leftVolume;
    	mRightVolume = rightVolume;
    	if (mIsPlaying) {
    		mMediaPlayer.setVolume(leftVolume, rightVolume);
    	}
    }
    
    public void setPitch(float pitch)
    {
    	
    }

    public void setLooping(boolean looping) 
    {
    	mLooping = looping;
    	if (mIsPlaying)
    	{
    		mMediaPlayer.setLooping(looping);
    	}
    }
    
    public boolean isPlaying()
    {
    	return mIsPlaying;
    }
}

class GHMediaErrorListener implements MediaPlayer.OnErrorListener 
{
	//private ArrayList<YrgMediaPlayerInfo> mMediaPlayerList;
	
	public GHMediaErrorListener(ArrayList<GHMediaPlayerInfo > mediaPlayerList) 
	{
		//mMediaPlayerList = mediaPlayerList;
	}
	
	public boolean onError(MediaPlayer mp, int what, int extra) 
	{
		Log.e("tag", "ERROR MediaErrorListener onError");
		return true;
		/*
		int index = YrgMediaPlayerInfo.getIndexOfMediaPlayer(mMediaPlayerList, mp);
		if(index != -1) {
			YrgMediaPlayerInfo info = mMediaPlayerList.get(index);
			Log.e("tag", "onError called, setDataSource called");
			info.setDataSource();
		}
		else {
			Log.e("tag", "COULD NOT FIND MEDIAPLAYER IN OUR LIST!?");
		}
		return true;
		*/
	}
}

class GHMediaPlayerCompletionListener implements MediaPlayer.OnCompletionListener
{
	private ArrayList<GHMediaPlayerInfo> mMediaPlayerList;
	
	public GHMediaPlayerCompletionListener(ArrayList<GHMediaPlayerInfo> mediaPlayerList) 
	{
		mMediaPlayerList = mediaPlayerList;
	}

	public void onCompletion(MediaPlayer mp) 
	{
		//Log.e("tag", "MediaPlayer onCompletion");
		int index = GHMediaPlayerInfo.getIndexOfMediaPlayer(mMediaPlayerList, mp);
		if (index != -1) {
			GHMediaPlayerInfo info = mMediaPlayerList.get(index);
			info.stop();
		}		
	}
}

class GHMediaInfoListener implements android.media.MediaPlayer.OnInfoListener
{
	//@Override
	public boolean onInfo(android.media.MediaPlayer mp, int what, int extra) 
	{
		return true;
		/*
		String infoStr = "no val?";
		if (what == MediaPlayer.MEDIA_INFO_UNKNOWN) {
			infoStr = "MEDIA_INFO_UNKNOWN";
		}
		else if (what == MediaPlayer.MEDIA_INFO_VIDEO_TRACK_LAGGING) {
			infoStr = "MEDIA_INFO_VIDEO_TRACK_LAGGING";
		}
		else if (what == MediaPlayer.MEDIA_INFO_BAD_INTERLEAVING) {
			infoStr = "MEDIA_INFO_BAD_INTERLEAVING";
		}
		else if (what == MediaPlayer.MEDIA_INFO_NOT_SEEKABLE) {
			infoStr = "MEDIA_INFO_NOT_SEEKABLE";
		}
		else if (what == MediaPlayer.MEDIA_INFO_METADATA_UPDATE) {
			infoStr = "MEDIA_INFO_METADATA_UPDATE";
		}
		Log.e("tag", "Media Player info: " + infoStr + " " + Integer.toString(extra));
		return true;
		*/
	}
}

class GHMediaPrepareListener implements android.media.MediaPlayer.OnPreparedListener
{
	private ArrayList<GHMediaPlayerInfo> mMediaPlayerList;
	
	public GHMediaPrepareListener(ArrayList<GHMediaPlayerInfo > mediaPlayerList) 
	{
		mMediaPlayerList = mediaPlayerList;
	}

	//@Override
	public void onPrepared(android.media.MediaPlayer mp)
	{
		int index = GHMediaPlayerInfo.getIndexOfMediaPlayer(mMediaPlayerList, mp);
		if (index != -1) {
			GHMediaPlayerInfo info = mMediaPlayerList.get(index);
			info.handlePrepared();
		}		
	}
}
