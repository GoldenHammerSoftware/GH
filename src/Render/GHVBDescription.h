// Copyright Golden Hammer Software
#pragma once

#include "GHVertexStreamFormat.h"
#include "GHVBBlitter.h"
#include "GHVBUsage.h"

// Description of how a vertex buffer should look.
// Used for creating vertex buffers through a VBFactory.
class GHVBDescription
{
public:
    typedef std::vector<GHVertexComponent> StreamComponents;
    
    class StreamDescription
    {
    public:
        StreamDescription(void)
        : mIsShared(true)
        , mUsage(GHVBUsage::STATIC)
        {}
        
        GHVBUsage::Enum mUsage;
        StreamComponents mComps;
        bool mIsShared;
    };
    
public:
    GHVBDescription(unsigned int numVerts, unsigned int numIndices, GHVBBlitter::Type blitterType)
        : mNumVerts(numVerts), mNumIndices(numIndices), mBlitterType(blitterType) {}
    
    void addStreamDescription(StreamDescription desc) { mStreams.push_back(desc); }
    const std::vector<StreamDescription>& getStreamDescriptions(void) const { return mStreams; }
    
    unsigned int getNumVerts(void) const { return mNumVerts; }
    unsigned int getNumIndices(void) const { return mNumIndices; }
    GHVBBlitter::Type getBlitterType(void) const { return mBlitterType; }

private:
    unsigned int mNumVerts;
    unsigned int mNumIndices;
    GHVBBlitter::Type mBlitterType;
    std::vector<StreamDescription> mStreams;
};
