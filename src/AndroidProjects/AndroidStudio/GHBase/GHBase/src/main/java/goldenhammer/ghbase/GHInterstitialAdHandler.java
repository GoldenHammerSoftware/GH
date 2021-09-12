package goldenhammer.ghbase;

import java.io.Serializable;

import android.app.Activity;
import android.util.Log;
import goldenhammer.ghbase.GHEngineInterface;
import goldenhammer.ghbase.GHInterstitialAdInterface;

class GHInterstitialAdHandler implements GHInterstitialAdInterface
										 ,Serializable
{

	private java.util.ArrayList<GHInterstitialAdInterface> mAdList = new java.util.ArrayList<GHInterstitialAdInterface>();
	private GHInterstitialAdInterface mCurrentAd = null;
	private GHEngineInterface mEngineInterface;
	
	public GHInterstitialAdHandler(GHEngineInterface engineInterface)
	{
		mEngineInterface = engineInterface;
	}
	
	public void addInterstitial(GHInterstitialAdInterface interstitial)
	{
		mAdList.add(interstitial);
	}
	
	public void showInterstitial() {
		if (mAdList.size() == 0) {
			mEngineInterface.onInterstitialDismiss();
			return;
		}
		
		java.util.Random random = new java.util.Random();
		int index = random.nextInt(mAdList.size());
		mCurrentAd = mAdList.get(index);
		mCurrentAd.showInterstitial();
	}

	public void hideInterstitial() {
		if (mCurrentAd == null) {
			return;
		}
		
		mCurrentAd.hideInterstitial();
		mCurrentAd = null;
	}

}