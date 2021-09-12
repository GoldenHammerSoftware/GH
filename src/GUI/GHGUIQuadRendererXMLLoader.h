// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHVBFactory;
class GHXMLObjFactory;
class GHScreenInfo;

/*
 // first child is the material.
<guiQuadRenderable drawOrderOffset=0 minUV="0 0" maxUV="1 1">
 <ghm vertex=guivertex.glsl pixel=testpixel.glsl zread="true" zwrite=false/>
</guiQuadRenderable>
*/
class GHGUIQuadRendererXMLLoader : public GHXMLObjLoader
{
public:
    GHGUIQuadRendererXMLLoader(const GHVBFactory& vbFactory, 
                               const GHXMLObjFactory& objFactory,
                               const GHScreenInfo& screenInfo);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    const GHVBFactory& mVBFactory;
    const GHXMLObjFactory& mObjFactory;
    const GHScreenInfo& mScreenInfo;
};
