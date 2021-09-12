// Copyright Golden Hammer Software
#pragma once

#include "GHMaterialCallbackFactory.h"
#include "GHMath/GHPoint.h"

class GHMaterialCallbackMgr;

// Place to manage the lights for a scene.
// We create transform material callbacks.
// In a complicated scene we might look for the nearest lights to apply
// todo: support light types other than directional
class GHLightMgr
{
private:
    static const int sMaxLights = 8;
    
    class CallbackFactory : public GHMaterialCallbackFactory
    {
    public:
        CallbackFactory(const GHLightMgr& lightMgr);
        virtual void createCallbacks(GHMaterial& mat) const;
        
    private:
        const GHLightMgr& mLightMgr;
    };
    
public:
	struct GHLight
	{
		// direction for directionals.
		// position for point lights.
		GHPoint3 mVec;
		// Light color/intensity.
		GHPoint3 mIntensity;
	};

public:
    GHLightMgr(GHMaterialCallbackMgr& matCallbackMgr);
    ~GHLightMgr(void);
    
	const GHLight& getLight(int index) const;
	GHLight& getLight(int index);

	// deprecated.  use getLight instead.
    void setLightDir(int index, const GHPoint3& dir);

    void setAmbientAmount(float val) { mAmbientAmount = val; }
    float getAmbientAmount(void) const { return mAmbientAmount; }
    
public:
    GHMaterialCallbackMgr& mMatCallbackMgr;
    CallbackFactory mCallbackFactory;

	GHLight mLights[sMaxLights];
    // global ambient
    float mAmbientAmount;
};
