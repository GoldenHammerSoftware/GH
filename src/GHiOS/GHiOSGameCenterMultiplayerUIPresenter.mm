// Copyright 2010 Golden Hammer Software
#include "GHiOSGameCenterMultiplayerUIPresenter.h"
#include "GHGameCenterNetworkDataMgr.h"

GHiOSGameCenterMultiplayerUIPresenter::GHiOSGameCenterMultiplayerUIPresenter(UIViewController* viewController)
: mViewController(viewController)
{

}

void GHiOSGameCenterMultiplayerUIPresenter::present(GKTurnBasedMatchmakerViewController* matchmakerController)
{
    [mViewController presentViewController:matchmakerController animated:YES completion:^(void){
        if(mNetworkDataMgr)
        {
            mNetworkDataMgr->notifyMultiplayerMatchDialogPresented();
        }
    }];
}

void GHiOSGameCenterMultiplayerUIPresenter::dismiss(void)
{
    [mViewController dismissViewControllerAnimated:YES completion:^(void){
        
    }];
}
