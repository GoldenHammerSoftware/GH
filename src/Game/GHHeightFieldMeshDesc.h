// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"

class GHMaterial;
class GHXMLObjFactory;
class GHXMLNode;

// struct for storing info necessary to create a heightfield mesh.
/*
 <heightFieldMesh size="512 512" pos="0 512" origin="0 512" divisions="4 4">
    <resource file=bms2-2mat_x0y1.xml/>
 </heightFieldMesh>
*/
class GHHeightFieldMeshDesc
{
public:
    GHHeightFieldMeshDesc(void);
    GHHeightFieldMeshDesc(const GHHeightFieldMeshDesc& other);
    ~GHHeightFieldMeshDesc(void);
    
    bool loadXML(const GHXMLNode& node, GHXMLObjFactory& objFactory);
    
    const GHPoint2i& getMeshSize(void) const { return mMeshSize; }
    const GHPoint2i& getMeshPos(void) const { return mMeshPos; }
    const GHPoint2i& getMeshOrigin(void) const { return mMeshOrigin; }
    const GHPoint2i& getDivisions(void) const { return mDivisions; }
    GHMaterial* getMaterial(void) const { return mMaterial; }
	unsigned int getVertsPerDivision(void) const { return mVertsPerDivision; }

	void setMeshSize(const GHPoint2i& size) { mMeshSize = size; }
	void setMeshPos(const GHPoint2i& pos) { mMeshPos = pos; }
	void setMeshOrigin(const GHPoint2i& origin) { mMeshOrigin = origin; }
	void setDivisions(const GHPoint2i& div) { mDivisions = div; }
	void setVertsPerDivision(unsigned int val) { mVertsPerDivision = val; }

    GHHeightFieldMeshDesc& operator =(const GHHeightFieldMeshDesc& other);
    
private:
    // number of nodes we cover.
    GHPoint2i mMeshSize;
    // addr of our corner node.
    GHPoint2i mMeshPos;
    // addr of the node we use as origin pos for x/z.
    GHPoint2i mMeshOrigin;
    // number of subdivisions of our big mesh.
    GHPoint2i mDivisions;
    // how to draw.
    GHMaterial* mMaterial;
	// desired number of verts per division
	// only applies if making an optimized mesh
	unsigned int mVertsPerDivision;
};
