#include "GHMaterialDX12.h"
#include "Render/GHMDesc.h"
#include "GHMaterialShaderInfoDX12.h"

GHMaterialDX12::GHMaterialDX12(GHMDesc* desc, GHShaderResource* vs, GHShaderResource* ps)
	: mDesc(desc)
{
	mShaders[GHShaderType::ST_VERTEX] = new GHMaterialShaderInfoDX12(vs);
	mShaders[GHShaderType::ST_PIXEL] = new GHMaterialShaderInfoDX12(ps);

	// todo: GHRenderProperties::DEVICEREINIT
}

GHMaterialDX12::~GHMaterialDX12(void)
{
	for (unsigned int shaderType = 0; shaderType < GHShaderType::ST_MAX; ++shaderType) {
		delete mShaders[shaderType];
	}
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

GHMaterialParamHandle* GHMaterialDX12::getParamHandle(const char* paramName)
{
	return 0;
}
