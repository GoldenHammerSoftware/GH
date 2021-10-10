#pragma once

#include "Render/GHMaterialParamHandle.h"
#include "Render/GHShaderParamList.h"
#include <vector>

class GHMaterialShaderInfoDX12;

class GHMaterialParamHandleDX12 : public GHMaterialParamHandle
{
public:
	void addShaderHandle(GHMaterialShaderInfoDX12* shader, const GHShaderParamList::Param* param);

	virtual void setValue(HandleType type, const void* value);

private:
	typedef std::pair<GHMaterialShaderInfoDX12*, const GHShaderParamList::Param*> Entry;
	std::vector<Entry> mHandles;
};
