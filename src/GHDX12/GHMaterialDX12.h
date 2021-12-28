#pragma once

#include "Render/GHMaterial.h"
#include "Render/GHShaderType.h"
#include "GHShaderDX12.h"
#include "GHDX12CBuffer.h"

class GHMDesc;
class GHMaterialShaderInfoDX12;
class GHRenderDeviceDX12;
class GHDX12MaterialHeapPool;
class GHDX12PSOPool;

class GHMaterialDX12 : public GHMaterial
{
public:
    GHMaterialDX12(GHRenderDeviceDX12& device, GHDX12MaterialHeapPool& heapPool, GHDX12PSOPool& psoPool, GHMDesc* desc, GHShaderResource* vs, GHShaderResource* ps);
    ~GHMaterialDX12(void);

    virtual void beginMaterial(const GHViewInfo& viewInfo) override;
    virtual void beginVB(const GHVertexBuffer& vb) override;
    virtual void beginGeometry(const GHPropertyContainer* geoData, const GHViewInfo& viewInfo) override;
    virtual void beginEntity(const GHPropertyContainer* entData, const GHViewInfo& viewInfo) override;
    virtual void beginTransform(const GHTransform& modelToWorld, const GHViewInfo& viewInfo) override;
    virtual void preBlit(void) override;
    virtual void endMaterial(void) override;
    virtual GHMaterialParamHandle* getParamHandle(const char* paramName) override;

private:
    void applyDXArgs(GHMaterialCallbackType::Enum type);
    Microsoft::WRL::ComPtr<ID3D12PipelineState> createPSO(const GHVertexBuffer& vb);
    void updateDescriptorHeap(void);
    void updateMaterialPsoHash(void);

private:
    GHRenderDeviceDX12& mDevice;
    GHDX12MaterialHeapPool& mHeapPool;
    GHDX12PSOPool& mPSOPool;
    GHMDesc* mDesc{ nullptr };
    GHMaterialShaderInfoDX12* mShaders[GHShaderType::ST_MAX];
    // hash of things on this material that don't change between draw calls.
    size_t mMaterialPsoHash;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDescriptorHeap{ nullptr };
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSamplerHeap{ nullptr };

    bool mDescriptorsDirty{ true };
};

