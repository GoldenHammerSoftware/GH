// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include "GHMath/GHPoint.h"

class GHResourceFactory;
class GHHeightField;
class GHTexture;

/*
 <heightField size="64 256" nodeDist=0.2 maxHeight=10>
    <hfseg texture="choices_hf_x0y0.hff" pos="0 0"/>
    <hfseg texture="choices_hf_x0y1.hff" pos="0 64"/>
    <hfseg texture="choices_hf_x0y2.hff" pos="0 128"/>
    <hfseg texture="choices_hf_x0y3.hff" pos="0 192"/>
 </heighField>
*/
class GHHeightFieldXMLLoader : public GHXMLObjLoader
{
public:
    GHHeightFieldXMLLoader(GHResourceFactory& resFactory);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
    void loadHFSeg(const GHXMLNode& node, GHHeightField& hf, float maxHeight, GHPropertyContainer& extraData) const;
    void loadHeightVals(GHHeightField& ret, GHTexture& tex, float maxHeight, const GHPoint2& posStart) const;

private:
    GHResourceFactory& mResFactory;
};
