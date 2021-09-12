// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHStringIdFactory;

/*
 <animSet>
 <Seq name=stand start=0 numFrames=0 loopFrames=0 fps=25/>
 ....
 </animSet>
 // numFrames means number of frames not including the start frame.
*/
class GHFrameAnimSetLoader : public GHXMLObjLoader
{
public:
    GHFrameAnimSetLoader(const GHStringIdFactory& hashTable);

    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    const GHStringIdFactory& mIdFactory;
};
