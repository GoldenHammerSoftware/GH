#include "GHMaterialDX12.h"
#include "Render/GHMDesc.h"

GHMaterialDX12::GHMaterialDX12(GHMDesc* desc)
	: mDesc(desc)
{
}

GHMaterialDX12::~GHMaterialDX12(void)
{
}

void GHMaterialDX12::beginMaterial(const GHViewInfo& viewInfo)
{
}

void GHMaterialDX12::beginGeometry(const GHPropertyContainer* geoData, const GHViewInfo& viewInfo)
{
}

void GHMaterialDX12::beginEntity(const GHPropertyContainer* entData, const GHViewInfo& viewInfo)
{
}

void GHMaterialDX12::beginTransform(const GHTransform& modelToWorld, const GHViewInfo& viewInfo)
{
}

void GHMaterialDX12::endMaterial(void)
{
}
