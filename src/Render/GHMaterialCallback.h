// Copyright Golden Hammer Software
#pragma once

class GHTransform;
class GHViewInfo;
class GHPropertyContainer;

// A class to handle pushing dynamic values to materials.
// This interface expects to handle exactly one material per instance.
class GHMaterialCallback
{
public:
    // the arguments passed to a CT_PERTRANS callback
    struct CBVal_PerTrans
    {
        CBVal_PerTrans(const GHTransform& modelToWorld,
                       const GHViewInfo& viewInfo)
        : mModelToWorld(modelToWorld)
        , mViewInfo(viewInfo)
        {}
        
        const GHTransform& mModelToWorld;
        const GHViewInfo& mViewInfo;
    };
	// the arguments passed to a CT_PERENT or CT_PERGEO callback
	struct CBVal_PerObj
	{
		CBVal_PerObj(const GHPropertyContainer* props,
			const GHViewInfo& viewInfo)
			: mProps(props)
			, mViewInfo(viewInfo)
		{}

		const GHPropertyContainer* mProps;
		const GHViewInfo& mViewInfo;
	};

public:
    virtual ~GHMaterialCallback(void) {}
    
    virtual void apply(const void* data) = 0;
};
