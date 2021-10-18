#pragma once

#include "Render/GHMaterial.h"
#include "Render/GHShaderType.h"
#include "GHShaderDX12.h"

class GHMDesc;
class GHMaterialShaderInfoDX12;
class GHRenderDeviceDX12;

class GHMaterialDX12 : public GHMaterial
{
public:
    GHMaterialDX12(GHRenderDeviceDX12& device, GHMDesc* desc, GHShaderResource* vs, GHShaderResource* ps);
    ~GHMaterialDX12(void);

    virtual void beginMaterial(const GHViewInfo& viewInfo) override;
    virtual void beginGeometry(const GHPropertyContainer* geoData, const GHViewInfo& viewInfo) override;
    virtual void beginEntity(const GHPropertyContainer* entData, const GHViewInfo& viewInfo) override;
    virtual void beginTransform(const GHTransform& modelToWorld, const GHViewInfo& viewInfo) override;
    virtual void endMaterial(void) override;
    virtual GHMaterialParamHandle* getParamHandle(const char* paramName) override;

private:
    void applyDXArgs(GHMaterialCallbackType::Enum type);
    void createRootSignature(void);

private:
    GHRenderDeviceDX12& mDevice;
    GHMDesc* mDesc{ nullptr };
    GHMaterialShaderInfoDX12* mShaders[GHShaderType::ST_MAX];
    ID3D12RootSignature* mRootSignature{ nullptr };
};

