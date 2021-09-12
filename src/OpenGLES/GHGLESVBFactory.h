// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHVBFactory.h"

class GHEventMgr;

class GHGLESVBFactory : public GHVBFactory
{
public:
    GHGLESVBFactory(GHEventMgr& eventMgr);
    virtual GHVertexStream* createVBStream(GHVertexStreamFormatPtr* format, unsigned int numVerts, GHVBUsage::Enum usage) const;
    virtual GHVBBlitterIndex* createIBStream(unsigned int numIndex, GHVBUsage::Enum usage) const;

private:
    GHEventMgr& mEventMgr;

	bool mVAOSupported{ false };
};
