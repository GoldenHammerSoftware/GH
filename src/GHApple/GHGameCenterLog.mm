// Copyright 2010 Golden Hammer Software
#include "GHGameCenterLog.h"
#import <GameKit/GameKit.h>

namespace GHGameCenterLog
{
    void printError(NSError* error)
    {
        switch ([error code])
        {
            case GKErrorNotSupported:
            {
                GHGameCenterLOG("GKErrorNotSupported");
                break;
            }
            case GKErrorGameUnrecognized:
            {
                GHGameCenterLOG("GKErrorGameUnrecognized");
                break;
            }
            case GKErrorUnknown:
            {
                GHGameCenterLOG("GCErrorUnknown");
                break;
            }
            case GKErrorCancelled:
            {
                GHGameCenterLOG("GKErrorCancelled");
                break;
            }
            case GKErrorCommunicationsFailure:
            {
                GHGameCenterLOG("GKErrorCommunicationsFailure");
                break;
            }
            case GKErrorUserDenied:
            {
                GHGameCenterLOG("GKErrorUserDenied");
                break;
            }
            case GKErrorInvalidCredentials:
            {
                GHGameCenterLOG("GKErrorInvalidCredentials");
                break;
            }
            case GKErrorNotAuthenticated:
            {
                GHGameCenterLOG("GKErrorNotAuthenticated");
                break;
            }
            case GKErrorAuthenticationInProgress:
            {
                GHGameCenterLOG("GKErrorAuthenticationInProgress");
                break;
            }
            case GKErrorInvalidPlayer:
            {
                GHGameCenterLOG("GKErrorInvalidPlayer");
                break;
            }
            case GKErrorScoreNotSet:
            {
                GHGameCenterLOG("GKErrorScoreNotSet");
                break;
            }
            case GKErrorParentalControlsBlocked:
            {
                GHGameCenterLOG("GKErrorParentalControlsBlocked");
                break;
            }
            case GKErrorMatchRequestInvalid:
            {
                GHGameCenterLOG("GKErrorMatchRequestInvalid");
                break;
            }
            case GKErrorUnderage:
            {
                GHGameCenterLOG("GKErrorUnderage");
                break;
            }
            case GKErrorInvalidParameter:
            {
                GHGameCenterLOG("GKErrorInvalidParameter");
                break;
            }
        }
    }
}
