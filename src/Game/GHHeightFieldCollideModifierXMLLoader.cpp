// Copyright Golden Hammer Software
#include "GHHeightFieldCollideModifierXMLLoader.h"
#include "GHHeightFieldCollideModifier.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHBaseIdentifiers.h"
#include "GHHeightFieldProperties.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHXMLNode.h"
#include "GHXMLObjFactory.h"

GHHeightFieldCollideModifierXMLLoader::GHHeightFieldCollideModifierXMLLoader(const GHPhysicsSim& physics,
	const GHIdentifierMap<int>& enumStore, GHXMLObjFactory& objFactory)
: mPhysics(physics)
, mEnumStore(enumStore)
, mObjFactory(objFactory)
{
}

void* GHHeightFieldCollideModifierXMLLoader::create(const GHXMLNode& node, 
	GHPropertyContainer& extraData) const
{
	// <hfCollideMod worldHeight=1000 collisionLayer=SBRAYCOLLISION/>
	GHHeightField* hf = (GHHeightField*)extraData.getProperty(GHHeightFieldProperties::HEIGHTFIELD);
	if (!hf) {
		GHDebugMessage::outputString("HFCollideMod called with no height field");
		return 0;
	}
	const char* colStr = node.getAttribute("collisionLayer");
	if (!colStr) {
		GHDebugMessage::outputString("No collisionLayer specified for HFCollideMod");
		return 0;
	}
	const int* colId = mEnumStore.find(colStr);
	if (!colId) {
		GHDebugMessage::outputString("Could not find id for HFCollideMod collisionLayer");
		return 0;
	}
	float worldHeight = 4000;
	node.readAttrFloat("worldHeight", worldHeight);
	unsigned int edgeSmooth = 0;
	node.readAttrUInt("edgeSmooth", edgeSmooth);
	unsigned int colSmooth = 0;
	node.readAttrUInt("colSmooth", colSmooth);
	float colSmoothPct = 0.9f;
	node.readAttrFloat("colSmoothPct", colSmoothPct);
	float edgeSmoothPct = 0.9f;
	node.readAttrFloat("edgeSmoothPct", edgeSmoothPct);

	GHHeightField* noiseHF = 0;
	const GHXMLNode* noiseTop = node.getChild("collideNoise", false);
	if (noiseTop && noiseTop->getChildren().size())
	{
		noiseHF = (GHHeightField*)mObjFactory.load(*noiseTop->getChildren()[0], &extraData);
		if (noiseHF) {
			GHDebugMessage::outputString("Loaded noise heightfield for collide mod");
		}
	}

	GHHeightFieldCollideModifier* ret = new GHHeightFieldCollideModifier(*hf, mPhysics, worldHeight, *colId, 
		edgeSmooth, edgeSmoothPct, colSmooth, colSmoothPct, noiseHF);
	return ret;
}
