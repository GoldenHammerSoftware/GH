// Copyright Golden Hammer Software
#pragma once

class GHFrameAnimSet;
class GHXMLNode;

// xml save function for a frame anim set.
// will only work correctly if debug identifiers is turned on.
class GHFrameAnimSetXMLSerializer
{
public:
    static void saveAnimSet(const GHFrameAnimSet& animSet, GHXMLNode& topNode);
};
