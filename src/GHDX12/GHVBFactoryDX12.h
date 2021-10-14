#pragma once

#include "Render/GHVBFactory.h"

class GHRenderDeviceDX12;

class GHVBFactoryDX12 : public GHVBFactory
{
public:
	GHVBFactoryDX12(GHRenderDeviceDX12& device);

	virtual GHVertexStream* createVBStream(GHVertexStreamFormatPtr* format, unsigned int numVerts, GHVBUsage::Enum usage) const override;
	virtual GHVBBlitterIndex* createIBStream(unsigned int numIndex, GHVBUsage::Enum usage) const override;

private:
	GHRenderDeviceDX12& mDevice;
};