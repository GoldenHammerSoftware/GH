#pragma once

#include "Render/GHMaterial.h"

class GHMDesc;

class GHMaterialDX12 : public GHMaterial
{
public:
    GHMaterialDX12(GHMDesc* desc);
    ~GHMaterialDX12(void);

    virtual void beginMaterial(const GHViewInfo& viewInfo) override;
    virtual void beginGeometry(const GHPropertyContainer* geoData, const GHViewInfo& viewInfo) override;
    virtual void beginEntity(const GHPropertyContainer* entData, const GHViewInfo& viewInfo) override;
    virtual void beginTransform(const GHTransform& modelToWorld, const GHViewInfo& viewInfo) override;
    virtual void endMaterial(void) override;

private:
    GHMDesc* mDesc;
};

