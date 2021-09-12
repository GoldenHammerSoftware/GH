// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHMaterial.h"
#include "GHMDesc.h"
#include "GHRenderDeviceReinitListener.h"

class GHGLESShaderProgram;
class GHGLESStateCache;

class GHGLESMaterialSH : public GHMaterial
{
public:
    GHGLESMaterialSH(GHGLESShaderProgram* shader, GHMDesc* desc, GHEventMgr& eventMgr, GHGLESStateCache& stateCache);
    ~GHGLESMaterialSH(void);
    
    virtual void beginMaterial(const GHViewInfo& viewInfo);
    virtual void beginGeometry(const GHPropertyContainer* geoData, const GHViewInfo& viewInfo);
    virtual void beginEntity(const GHPropertyContainer* entData, const GHViewInfo& viewInfo);
    virtual void beginTransform(const GHTransform& modelToWorld, const GHViewInfo& viewInfo);
	virtual void endMaterial(void) {}
    virtual GHMaterialParamHandle* getParamHandle(const char* paramName);
    int getTextureIndex(int shaderHandle);
    
    bool getExpectsDXLayout(void) const;

private:
    void applyAlpha(void);
    void applyZBuffer(void);
	void applyCulling(bool flipWinding);
	void applyTextures(void);
    void attachTextureHandlesToShader(void);

	void onDeviceReinit(void);
    void handleGraphicsQualityChanged(void);

private:
	GHRenderDeviceReinitListener<GHGLESMaterialSH> mDeviceListener;
    GHGraphicsQualityChangedListener<GHGLESMaterialSH> mQualityChangedListener;
	friend class GHRenderDeviceReinitListener<GHGLESMaterialSH>; // so onDeviceReinit can be private.
    friend class GHGraphicsQualityChangedListener<GHGLESMaterialSH>; 

	GHGLESStateCache& mStateCache;
	GHEventMgr& mEventMgr;

    std::vector<int> mTextureHandles;
    
    // we keep track of which shader callback textures are being applied,
    // in order to support multiple callbacks.
    std::vector<int> mAdditionalTextures;

	GHGLESShaderProgram* mShader{ 0 };
	GHMDesc* mDesc{ 0 };
	GHMDesc::ParamHandles* mDescParamHandles{ 0 };
};

