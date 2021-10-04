// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHResource.h"

class GHRenderDeviceDX11;
class GHShaderParamListDX11;

// wrapper class to associate an ID3D11[vert/pixel]Shader with its bytecode.
// also handles binding the shader to the device.
class GHShaderDX11
{
public:
	virtual ~GHShaderDX11(void) {}

	virtual void bind(void) = 0;
	virtual void getBytecode(const char*& data, size_t& dataLen) const = 0;
	virtual const GHShaderParamListDX11& getParamList(void) const = 0;
};

typedef GHResourcePtr<GHShaderDX11> GHShaderResource;
