// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHMaterial.h"
#include "GHMDesc.h"

class GHOGLShaderProgram;

class GHMaterialOGLSH : public GHMaterial
{
public:
    GHMaterialOGLSH(GHOGLShaderProgram* shader, GHMDesc* desc);
    ~GHMaterialOGLSH(void);
    
    virtual void beginMaterial(const GHViewInfo& viewInfo) override;
    virtual void endMaterial(void) override {}
    virtual void beginGeometry(const GHPropertyContainer* geoData, const GHViewInfo& viewInfo) override;
    virtual void beginEntity(const GHPropertyContainer* entData, const GHViewInfo& viewInfo) override;
    virtual void beginTransform(const GHTransform& modelToWorld, const GHViewInfo& viewInfo) override;
    virtual GHMaterialParamHandle* getParamHandle(const char* paramName);
    
    int getTextureIndex(int shaderHandle);
    
private:
    void applyAlpha(void);
    void applyZBuffer(void);
	void applyCulling(void);
	void applyTextures(void);
	void applyTextureWrap(int textureStage, GHMDesc::WrapMode val);
    void applyWireframe(void);

private:
    GHOGLShaderProgram* mShader;
    GHMDesc* mDesc;
    
    std::vector<int> mTextureHandles;
    // we keep track of which shader callback textures are being applied,
    // in order to support multiple callbacks.
    std::vector<int> mAdditionalTextures;
};
