// Copyright 2010 Golden Hammer Software
using PhoneDirect3DXamlAppComponent;
using Windows.ApplicationModel;
using Windows.ApplicationModel.Store;
using System;
using System.Threading.Tasks;
using System.Windows.Threading;

namespace PhoneDirect3DXamlAppInterop
{

// The C# part of supporting nokia ads.
public class GHWPIAPInterface
{
    Dispatcher mUIDispatcher;
    MainPage mGameInterface; 

    public GHWPIAPInterface(GHWPEventInterface eventInterface, Dispatcher uiDispatcher,
        MainPage gameInterface)
    {
        mGameInterface = gameInterface;
        eventInterface.BuyIAP += OnBuyIAP;
        eventInterface.ConfirmIAP += OnConfirmIAP;
        mUIDispatcher = uiDispatcher;
    }

    protected void OnBuyIAP(string iapName, bool isTrialPurchase)
    {
        mUIDispatcher.BeginInvoke(async() =>
        {
	        LicenseInformation licenseInformation = CurrentApp.LicenseInformation; 
	        if (!isTrialPurchase)
	        {
                if (!licenseInformation.ProductLicenses[iapName].IsActive) 
		        { 
			        try {
                        await CurrentApp.RequestProductPurchaseAsync(iapName, false); 
                        if (!licenseInformation.ProductLicenses[iapName].IsActive) {
                            buyTaskCallback(iapName, false);
                        }
                        else {
                            buyTaskCallback(iapName, true);
                        }
			        }
			        catch(Exception exception)
			        {
				        System.Diagnostics.Debug.WriteLine("Exception on RequestProductPurchaseAsync");
				        buyTaskCallback(iapName, false);
			        }
		        } 
		        else 
		        { 
			        System.Diagnostics.Debug.WriteLine("Already purchased");
			        buyTaskCallback(iapName, true);
		        }
	        }
	        else 
	        {
		        if (!licenseInformation.IsTrial)
		        {
			        System.Diagnostics.Debug.WriteLine("Already have the full version");
			        buyTaskCallback(iapName, true);
		        }
		        else
		        {
			        try {
                        await CurrentApp.RequestAppPurchaseAsync(false); 
                        if (!licenseInformation.ProductLicenses[iapName].IsActive) {
                            buyTaskCallback(iapName, false);
                        }
                        else {
                            buyTaskCallback(iapName, true);
                        }
			        }
			        catch(Exception exception)
			        {
				        System.Diagnostics.Debug.WriteLine("Exception on RequestAppPurchaseAsync");
				        buyTaskCallback(iapName, false);
			        }
		        }
	        }
        });
    }

    protected void OnConfirmIAP(string iapName, bool isTrialPurchase)
    {
        mUIDispatcher.BeginInvoke(async() =>
        {
            LicenseInformation licenseInformation = CurrentApp.LicenseInformation;
            if (!isTrialPurchase)
            {
                if (!licenseInformation.ProductLicenses[iapName].IsActive)
                {
                    buyTaskCallback(iapName, false);
                }
                else
                {
                    buyTaskCallback(iapName, true);
                }
            }
            else
            {
                if (!licenseInformation.IsTrial)
                {
                    buyTaskCallback(iapName, true);
                }
                else
                {
                    buyTaskCallback(iapName, false);
                }
            }
        });
    }

    protected void buyTaskCallback(string iapName, bool success)
    {
        mGameInterface.handleIAPComplete(iapName, success);
    }

}

}
