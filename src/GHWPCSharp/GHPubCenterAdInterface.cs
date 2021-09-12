// Copyright 2010 Golden Hammer Software
//#define TEST_PUBCENTER_ADS

using PhoneDirect3DXamlAppComponent;
using Microsoft.Advertising.Mobile.UI; 
using Microsoft.Phone.Controls;
using System.Windows.Controls;
using System.Windows.Threading;

namespace PhoneDirect3DXamlAppInterop
{

// The C# part of supporting pubcenter ads.
public class GHPubCenterAdInterface
{
    AdControl mBanner = null;
    DrawingSurfaceBackgroundGrid mBackground = null;
    bool mAdShown = false;
    Dispatcher mUIDispatcher;

    public GHPubCenterAdInterface(GHWPEventInterface eventInterface, DrawingSurfaceBackgroundGrid background,
        Dispatcher uiDispatcher)
    {
        mUIDispatcher = uiDispatcher;
        mBackground = background;
        eventInterface.HideAds += OnHideAds;
        eventInterface.ShowAds += OnShowAds;
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
            try
            {
                if (mBanner == null)
                {
#if TEST_PUBCENTER_ADS
                    mBanner = new AdControl("test_client",  // ApplicationID
                                            "Image480_80",  // AdUnitID
                                            true);          // isAutoRefreshEnabled
#else
                mBanner = new AdControl("9ce769bd-b753-4238-b4d8-fbeec82af9b7",     // ApplicationID
                                        "108743",                                    // AdUnitID
                                        true);                                       // isAutoRefreshEnabled
#endif
                    mBanner.VerticalAlignment = System.Windows.VerticalAlignment.Top;
                    mBanner.Width = 480;
                    mBanner.Height = 80;
                    mBanner.ErrorOccurred += AdControl_ErrorOccurred;
                }
                mBackground.Children.Add(mBanner);
            }
            catch (System.Exception exception)
            {

            }
        });
    }

    private void AdControl_ErrorOccurred(object sender, Microsoft.Advertising.AdErrorEventArgs e)
    {
        System.Diagnostics.Debug.WriteLine("Ad Error : ({0}) {1}", e.ErrorCode, e.Error);
    }
}

}
