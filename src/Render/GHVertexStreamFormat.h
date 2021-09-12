// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHRefCounted.h"
#include "GHVertexComponent.h"
#include <vector>
#include "GHVBUsage.h"

// describes the position of vert components in a stream.
class GHVertexStreamFormat
{
public:
    struct ComponentEntry
    {
        // which vertex component we represent
        GHVertexComponentShaderID::Enum mComponent;
        // type of component, eg byte/short/float
        GHVertexComponentType::Enum mType;
        // number of mTypes in a row that represent this thing.
        unsigned short mCount;
        
        // offset in floats from the start of a vertex for our entry
        unsigned short mOffset;
    };

public:
    GHVertexStreamFormat(void);
    
    // set the offset from vertex start for an individual component in floats.
    // for use by vb factory.
    void addComponent(GHVertexComponentShaderID::Enum component,
                      GHVertexComponentType::Enum type, unsigned short count,
                      unsigned short offset);
    // set the size of each vert in the stream in floats.
    // for use by vb factory.
    void setVertexSize(unsigned short size);
    
    unsigned short getVertexSize(void) const { return mVertexSize; }
    const std::vector<ComponentEntry>& getComponents(void) const { return mComponents; }
    const ComponentEntry* getComponentEntry(GHVertexComponentShaderID::Enum id) const;

    void setShared(bool val) { mShared = val; }
    bool isShared(void) const { return mShared; }
    
private:
    std::vector<ComponentEntry> mComponents;
	// size of a vertex in number of floats.
    unsigned short mVertexSize;
    // flag to say whether to share or copy this stream between vb clones.
    bool mShared;
};

typedef GHRefCountedType<GHVertexStreamFormat> GHVertexStreamFormatPtr;
