// Copyright Golden Hammer Software
#include "GHHeightFieldMeshDesc.h"
#include "GHMaterial.h"
#include "GHXMLNode.h"
#include "GHXMLObjFactory.h"
#include "GHPlatform/GHDebugMessage.h"

GHHeightFieldMeshDesc::GHHeightFieldMeshDesc(void)
: mMeshSize(0,0)
, mMeshPos(0,0)
, mMeshOrigin(0,0)
, mDivisions(1,1)
, mMaterial(0)
, mVertsPerDivision(1000)
{
}

GHHeightFieldMeshDesc::GHHeightFieldMeshDesc(const GHHeightFieldMeshDesc& other)
: mMeshSize(0,0)
, mMeshPos(0,0)
, mMeshOrigin(0,0)
, mDivisions(1,1)
, mMaterial(0)
{
    *this = other;
}

GHHeightFieldMeshDesc::~GHHeightFieldMeshDesc(void)
{
    GHRefCounted::changePointer((GHRefCounted*&)mMaterial, 0);
}

bool GHHeightFieldMeshDesc::loadXML(const GHXMLNode& node, GHXMLObjFactory& objFactory)
{
    if (!node.readAttrIntArr("size", mMeshSize.getArr(), 2)) {
        GHDebugMessage::outputString("No hf mesh size specified");
        return false;
    }
    if (!node.readAttrIntArr("pos", mMeshPos.getArr(), 2)) {
        GHDebugMessage::outputString("No hf mesh pos specified");
        return false;
    }
    node.readAttrIntArr("origin", mMeshOrigin.getArr(), 2);
	node.readAttrUInt("vertsPerDivision", mVertsPerDivision);

    if (!node.getChildren().size()) {
        GHDebugMessage::outputString("No material specified for hf mesh");
        return false;
    }
    
    node.readAttrIntArr("divisions", mDivisions.getArr(), 2);
    if (mDivisions[0] == 0 || mDivisions[1] == 0) {
        GHDebugMessage::outputString("Divisions can't be 0 for hf mesh");
        return false;
    }

    mMaterial = (GHMaterial*)objFactory.load(*node.getChildren()[0]);
    if (!mMaterial) {
        GHDebugMessage::outputString("Failed to load hf mat");
        return false;
    }
    mMaterial->acquire();
    
    return true;
}

GHHeightFieldMeshDesc& GHHeightFieldMeshDesc::operator =(const GHHeightFieldMeshDesc& other)
{
    mMeshSize = other.getMeshSize();
    mMeshPos = other.getMeshPos();
    mMeshOrigin = other.getMeshOrigin();
    mDivisions = other.getDivisions();
    GHRefCounted::changePointer((GHRefCounted*&)mMaterial, other.getMaterial());
	mVertsPerDivision = other.getVertsPerDivision();
    return *this;
}

