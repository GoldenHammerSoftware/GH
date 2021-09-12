// Copyright 2010 Golden Hammer Software

using PhoneDirect3DXamlAppComponent;
using Inneractive.Nokia.Ad;
using Microsoft.Phone.Controls;
using System.Windows.Controls;
using System.Windows.Threading;

namespace PhoneDirect3DXamlAppInterop
{

// The C# part of supporting nokia ads.
public class GHNokiaAdInterface
{
    InneractiveAd mBanner = null;
    DrawingSurfaceBackgroundGrid mBackground = null;
    bool mAdShown = false;
    Dispatcher mUIDispatcher;

    public GHNokiaAdInterface(GHWPEventInterface eventInterface, DrawingSurfaceBackgroundGrid background,
        Dispatcher uiDispatcher)
    {
        mUIDispatcher = uiDispatcher;
        mBackground = background;
        eventInterface.HideAds += OnHideAds;
        eventInterface.ShowAds += OnShowAds;

        InneractiveAd.AdReceived += new InneractiveAd.IaAdReceived(InneractiveAd_AdReceived);
        InneractiveAd.DefaultAdReceived += new InneractiveAd.IaDefaultAdReceived(InneractiveAd_DefaultAdReceived);
        InneractiveAd.AdFailed += new InneractiveAd.IaAdFailed(InneractiveAd_AdFailed);
        InneractiveAd.AdClicked += new InneractiveAd.IaAdClicked(InneractiveAd_AdClicked);
    }

    protected void OnHideAds()
    {
        if (!mAdShown) return;
        mAdShown = false;
        if (mBanner == null) return;

        mUIDispatcher.BeginInvoke(() =>
        {
            mBackground.Children.Remove(mBanner);
        }); 
    }

    protected void OnShowAds()
    {
        if (mAdShown) return;
        mAdShown = true;

        mUIDispatcher.BeginInvoke(() =>
        {
            if (mBanner == null)
            {
                mBanner = new InneractiveAd("GoldenHammerSoftware_TrickShotBowling_WP", InneractiveAd.IaAdType.IaAdType_Banner, 60);
                mBanner.VerticalAlignment = System.Windows.VerticalAlignment.Top;
            }
            mBackground.Children.Add(mBanner);
        });
    }

    void InneractiveAd_AdReceived(object sender)
    {
        System.Diagnostics.Debug.WriteLine("InneractiveAd: AdReceived");
    }

    void InneractiveAd_DefaultAdReceived(object sender)
    {
        System.Diagnostics.Debug.WriteLine("InneractiveAd: DefaultAdReceived");
    }

    void InneractiveAd_AdFailed(object sender)
    {
        System.Diagnostics.Debug.WriteLine("InneractiveAd: AdFailed");
    }

    void InneractiveAd_AdClicked(object sender)
    {
        System.Diagnostics.Debug.WriteLine("InneractiveAd: AdClicked");
    }
}

}
