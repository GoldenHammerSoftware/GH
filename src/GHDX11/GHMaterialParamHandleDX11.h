// Copyright Golden Hammer Software
#pragma once

#include "GHMaterialParamHandle.h"
#include "GHShaderParamList.h"
#include <vector>

class GHMaterialShaderInfoDX11;

class GHMaterialParamHandleDX11 : public GHMaterialParamHandle
{
public:
	void addShaderHandle(GHMaterialShaderInfoDX11* shader, const GHShaderParamList::Param* param);

	virtual void setValue(HandleType type, const void* value);

private:
	typedef std::pair<GHMaterialShaderInfoDX11*, const GHShaderParamList::Param*> Entry;
	std::vector<Entry> mHandles;
};
