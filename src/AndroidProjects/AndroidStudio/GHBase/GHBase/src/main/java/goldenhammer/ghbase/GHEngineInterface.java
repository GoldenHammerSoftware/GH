package goldenhammer.ghbase;

import java.io.FileDescriptor;
import java.io.IOException;

import android.app.Activity;
import android.content.Intent;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.util.Log;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.SystemClock;

import java.util.ArrayList;
import java.util.HashMap;

public class GHEngineInterface {
	public native void runNativeFrame();

	public native void resizeWindow(int windowWidth, int windowHeight);

	public native void launchNativeApp(int windowWidth, int windowHeight,
			String externalStoragePath, Object engineInterface, int isTablet,
			int supportIAP);

	public native void handleTouchStart(int pointerId);

	public native void handleTouchEnd(int pointerId);

	public native void handleTouchPos(int pointerId, float x, float y);

	public native void handleAcceleration(float x, float y, float z);

	public native void handleJavaPause();

	public native void handleJavaResume();

	public native void handleJavaShutdown();

	public native void handleBackPressed();

    public native void handleGamepadButtonChange(int gamepadIdx, int key, float pressed);
    public native void handleGamepadJoystickChange(int gamepadIdx, int joyIdx, float x, float y);
    public native void handleGamepadActive(int gamepadIdx, int active);
    
	public native String calculatePublicKey();

	public native void onInterstitialRewardGranted();
	
	public native void onRewardInterstitialAvailabilityChange(boolean available);
	
	//resizeWindow, handleTouchStart, handleTouchEnd, handleTouchPos
	//handleAcceleration, handleJavaPause, handleJavaResume, handleJavaShutdown,
	//handleBackPressed, calculatePublicKey, onInterstitialRewardGranted,
	//onRewardInterstitialAvailabilityChange, onInterstitialDismiss

	// GHAndroidInterstitialAd.cpp
	public native void onInterstitialDismiss();

	// GHAndroidFileOpener.cpp
	public native void loadFile(Object fileDescriptor, long offset,
			long length, int fileHandle);

	// GHAndroidBitmapLoader.cpp
	public native void handleTextureLoadConfirmed(int width, int height,
			int depth, int textureId);

	// GHAndroidAdHandler.cpp
	public native void onAdActivation();

	public native void onAdDeactivation();

	// GHAndroidIAPStore.cpp
	public native void onIAPPurchase(int isSuccess, int id);

	public GHEngineInterface(AssetManager assetMgr, Activity activity) {
		mAssetMgr = assetMgr;
		mTextureBuffers = new HashMap<Integer, String>();
		mNextTextureId = 0;

		mMediaPlayers = new ArrayList<GHMediaPlayerInfo>();

		mActivity = activity;
	}

	public void getFileDescription(String filename, int fileHandle) {
		filename += ".mp3";
		try {
			AssetFileDescriptor afd = mAssetMgr.openFd(filename);
			FileDescriptor fd = afd.getFileDescriptor();
			long offset = afd.getStartOffset();
			long length = afd.getLength();
			loadFile(fd, offset, length, fileHandle);
			afd.close();
		}

		catch (IOException e) {
			Log.e("file load fail", e.getMessage());
		}
	}

	public void initializeSoundPool() {
		mSoundPool = new GHSoundPool(mSoundPoolConfig);
	}

	public GHSoundPoolSound createSoundPoolSound(String filename) {
		return mSoundPool.createSound(mAssetMgr, filename);
	}

	public void destroySoundPoolSound(GHSoundPoolSound sound) {
		mSoundPool.destroySound(sound);
	}

	GHSoundPool mSoundPool;
	GHSoundPoolConfig mSoundPoolConfig;

	public void setSoundPoolConfig(GHSoundPoolConfig soundPoolConfig) {
		mSoundPoolConfig = soundPoolConfig;
	}

	public GHMediaPlayerInfo createMediaPlayer(String filename) {
		GHMediaPlayerInfo mpi = new GHMediaPlayerInfo();
		mpi.mMediaPlayer = new MediaPlayer();

		mpi.mMediaPlayer
				.setOnCompletionListener(new GHMediaPlayerCompletionListener(
						mMediaPlayers));
		mpi.mMediaPlayer.setOnErrorListener(new GHMediaErrorListener(
				mMediaPlayers));
		// mpi.mMediaPlayer.setOnInfoListener(new GHMediaInfoListener());
		mpi.mMediaPlayer.setOnPreparedListener(new GHMediaPrepareListener(
				mMediaPlayers));

		mpi.mFilename = filename;
		mpi.loadFileDescriptor(mAssetMgr);
		mpi.setDataSource();
		mMediaPlayers.add(mpi);
		mpi.mMediaPlayers = mMediaPlayers;

		return mpi;
	}

	public void destroyMediaPlayer(GHMediaPlayerInfo player) {
		int index = mMediaPlayers.indexOf(player);
		player.shutdown();
		mMediaPlayers.remove(index);
	}

	ArrayList<GHMediaPlayerInfo> mMediaPlayers;

	public long getCurrTime() {
		return SystemClock.uptimeMillis();
	}

	public void loadJavaBitmap(String filename, int wantMips) {
		// Log.e("bmjava", "loading bitmap " + filename);

		GHJavaTexture texture = GHJavaTexture.loadTextureFromFile(filename,
				mAssetMgr);
		if (texture == null) {
			String failString = "Couldn't decode java bitmap " + filename;
			Log.e("bmjava", failString);
			return;
		}

		int textureId = mNextTextureId;
		mNextTextureId++;
		mTextureBuffers.put(textureId, filename);

		handleTextureLoadConfirmed(texture.mWidth, texture.mHeight,
				texture.mDepth, textureId);
		texture.giveToOGL(wantMips);
	}

	public void freeBitmap(int textureId) {
		mTextureBuffers.remove(textureId);
		// todo? trigger a garbage collect?
	}

	public void applyBitmap(int textureId, int wantMips) {
		String bitmapName = mTextureBuffers.get(textureId);
		if (bitmapName == null) {
			Log.e("bmjava", "Failed to find bitmap name in java");
			return;
		}
		GHJavaTexture texture = GHJavaTexture.loadTextureFromFile(bitmapName,
				mAssetMgr);
		if (texture != null) {
			texture.giveToOGL(wantMips);
		}
	}

	public void openURL(String url) {
		Intent i = new Intent(Intent.ACTION_VIEW);
		i.setData(Uri.parse(url));
		mActivity.startActivity(i);
	}

	public void exitApp() {
		mActivity.finish();
	}

	public GHInterstitialAdInterface mInterstitialHandler = null;
	public GHInterstitialAdInterface mRewardsInterstitial = null;

	public void showInterstitialAd() {
		Log.e("tag", "in GHEngineinterface.showInterstitialAd");
		if (mInterstitialHandler != null) {
			mInterstitialHandler.showInterstitial();
		} else {
			onInterstitialDismiss();
		}
	}

	public void hideInterstitialAd() {
		Log.e("tag", "in GHEngineinterface.hideInterstitialAd");
		if (mInterstitialHandler != null) {
			mInterstitialHandler.hideInterstitial();
		}
	}

	public void showRewardsInterstitial() {
		Log.e("tag", "in GHEngineinterface.showRewardsInterstitial");
		if (mRewardsInterstitial != null) {
			mRewardsInterstitial.showInterstitial();
		} else {
			onInterstitialDismiss();
		}
	}

	public void hideRewardsInterstitial() {
		Log.e("tag", "in GHEngineinterface.hideRewardsInterstitial");
		if (mRewardsInterstitial != null) {
			mRewardsInterstitial.hideInterstitial();
		}
	}

	public GHBannerAd mBannerAdHandler = null;

	// called from GHAndroidAdHandler
	public void activateBannerAd() {
		if (mBannerAdHandler != null) {
			mBannerAdHandler.activateAd();
		}
	}

	public void deactivateBannerAd() {
		if (mBannerAdHandler != null) {
			mBannerAdHandler.deactivateAd();
		}
	}

	public GHIAPInterface mIAP;

	// called from GHAndroidIAPStore
	public void purchaseIAP(int itemID) {
		if (mIAP != null) {
			mIAP.purchaseItem(itemID);
		} else {
			Log.e("tag",
					"Error: attempting to purchase an IAP when we haven't created the GHIAP");
			onIAPPurchase(0, itemID);
		}
	}

	public void checkAllIAPs() {
		if (mIAP != null) {
			mIAP.checkAllPurchases();
		}
	}

	public void setSupportsIAP(boolean val) {
		mSupportsIAP = val ? 1 : 0;
	}

	public void setSupportsES2(boolean val) {
		mSupportsES2 = val ? 1 : 0;
	}

	public AssetManager mAssetMgr;
	int mNextTextureId;
	HashMap<Integer, String> mTextureBuffers;
	Activity mActivity;

	private int mSupportsES2;
	private int mSupportsIAP = 0;

	public int supportsIAP() {
		return mSupportsIAP;
	}
}