// Copyright Golden Hammer Software
#pragma once

#include "GHGUIRectMaker.h"
#include <vector>
#include "Base/GHInputClaimGroup.h"
#include "GHGUINavMgr.h"

class GHSystemServices;
class GHRenderServices;
class GHRenderGroup;
class GHGUIMgr;
class GHAppShard;
class GHDeletionHandle;
class GHGUITransitionControllerFactory;
class GHTimeVal;
class GHGUINavMgr;

// A place to do any necessary gui initialization,
//  such as creating xml loaders and the gui mgr.
class GHGUISystem
{
public:
    GHGUISystem(GHSystemServices& systemServices,
                GHRenderServices& renderServices,
                GHRenderGroup& sceneGroup,
                GHAppShard& appShard,
                const GHTimeVal& time);
    ~GHGUISystem(void);

public:
	GHGUINavMgr mNavMgr;
    GHGUIMgr* mGUIMgr;
	GHInputClaimGroup mInputClaim;
    GHGUITransitionControllerFactory* mTransitionSpawner;
    GHGUIRectMaker mRectMaker;
    std::vector<GHDeletionHandle*> mOwnedItems;
};
