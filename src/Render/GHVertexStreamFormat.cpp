// Copyright Golden Hammer Software
#include "GHVertexStreamFormat.h"
#include "GHPlatform/GHDebugMessage.h"
#include <stddef.h>

GHVertexStreamFormat::GHVertexStreamFormat(void)
: mVertexSize(0)
, mShared(true)
{
}

void GHVertexStreamFormat::addComponent(GHVertexComponentShaderID::Enum component,
                                        GHVertexComponentType::Enum type, unsigned short count,
                                        unsigned short offset)
{
    ComponentEntry entry;
    entry.mComponent = component;
    entry.mOffset = offset;
    entry.mType = type;
    entry.mCount = count;
    mComponents.push_back(entry);
}

void GHVertexStreamFormat::setVertexSize(unsigned short size)
{
    mVertexSize = size;
}

const GHVertexStreamFormat::ComponentEntry* 
GHVertexStreamFormat::getComponentEntry(GHVertexComponentShaderID::Enum id) const
{
    for (size_t i = 0; i < mComponents.size(); ++i)
    {
        if (mComponents[i].mComponent == id) return &mComponents[i];
    }
    return 0;
}
