// Copyright Golden Hammer Software
#pragma once

#include "GHViewInfo.h"
#include "GHRenderList.h"
#include "GHMath/GHPoint.h"
#include "GHString/GHIdentifier.h"
#include "GHUtils/GHPredicate.h"

class GHCamera;
class GHRenderDevice;
class GHScreenInfo;
class GHRenderTarget;
class GHRenderDevice;

class GHRenderPass
{
public:
    static GHIdentifier sMainPassId;
    
	enum HMDRenderType
	{
		HMDRT_MAINONLY, // only draw to the assigned render target
		HMDRT_HMDONLY, // only draw to the hmd target, ignore mRenderTarget
		HMDRT_BOTH, // draw to the assigned render target and the hmd target.
	};

public:
    GHRenderPass(const GHCamera& camera, const GHScreenInfo& screenInfo, 
                 GHRenderTarget* renderTarget, GHRenderDevice& device,
                 bool clearBuffers, const GHPoint4* clearColor,
                 const GHIdentifier& id, bool supportsStereo, bool isRenderToTexture=false);
    ~GHRenderPass(void);

    // do any once per frame setup.
    void prepareForCollect(void);
    // assume our renderlist was filled, and blit.
    void draw(void);
	// clear out our renderlist because we're done with it.
	void clear(void);
    
    const GHViewInfo& getViewInfo(GHViewInfo::EyeIndex eye) const { return mViewInfos[eye]; }
    GHRenderList& getRenderList(void) { return mRenderList; }
    const GHIdentifier& getIdentifier(void) const { return mId; }
	const GHCamera& getCamera(void) const { return mCamera; }

	GHRenderTarget* getRenderTarget(void) { return mRenderTarget; }
	void setRenderTarget(GHRenderTarget* target) { mRenderTarget = target; }

	HMDRenderType getHMDRenderType(void) const { return mHMDRenderType; }
	void setHMDRenderType(HMDRenderType type) { mHMDRenderType = type; }

    // Add something that is allowed to say "no" on whether we should render a frame.
    // We take ownership.
    void addPredicate(GHPredicate* val) { mPredicates.push_back(val); }

private:
    // look at our predicates and decide if we should draw.
    bool shouldRenderFrame(void) const;

private:
    std::vector<GHPredicate*> mPredicates;
    const GHCamera& mCamera;
    const GHScreenInfo& mScreenInfo;
	GHRenderDevice& mRenderDevice;

	GHRenderList mRenderList;
    // optional offscreen render target.
    GHRenderTarget* mRenderTarget;
    bool mClearBuffers;
    GHPoint4 mClearColor;
    GHIdentifier mId;

	bool mSupportsStereo;
	bool mIsRenderToTexture;

	GHViewInfo mViewInfos[GHViewInfo::EYE_MAX];
	HMDRenderType mHMDRenderType{ HMDRT_MAINONLY };
};
