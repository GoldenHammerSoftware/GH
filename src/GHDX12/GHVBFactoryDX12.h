#pragma once

#include "Render/GHVBFactory.h"

class GHVBFactoryDX12 : public GHVBFactory
{
public:
	virtual GHVertexStream* createVBStream(GHVertexStreamFormatPtr* format, unsigned int numVerts, GHVBUsage::Enum usage) const override;
	virtual GHVBBlitterIndex* createIBStream(unsigned int numIndex, GHVBUsage::Enum usage) const override;
};