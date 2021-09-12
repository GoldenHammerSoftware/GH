// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include "GHString/GHIdentifierMap.h"

class GHPhysicsSim;
class GHXMLObjFactory;

// <hfCollideMod worldHeight=1000 collisionLayer=SBRAYCOLLISION edgeSmooth=5 colSmooth=1 colSmoothPct="0.9" edgeSmoothPct="0.9">
//	<collideNoise>
//		<heightField size = "4096 4096" nodeDist = "2" maxHeight = "5">
//			<hfseg texture = "noise.hff" pos = "0 0" / >
//		</heightField>
//	</collideNoise>
// </hfCollideMod>
class GHHeightFieldCollideModifierXMLLoader : public GHXMLObjLoader
{
public:
	GHHeightFieldCollideModifierXMLLoader(const GHPhysicsSim& physics, 
		const GHIdentifierMap<int>& enumStore, GHXMLObjFactory& objFactory);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	const GHPhysicsSim& mPhysics;
	const GHIdentifierMap<int>& mEnumStore;
	GHXMLObjFactory& mObjFactory;
};
