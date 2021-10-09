// Copyright Golden Hammer Software
#pragma once

#include "Base/GHXMLObjLoader.h"

class GHXMLObjFactory;

/*
<particleDesc
	gravity="0 -1 0"
	spawnRate=0.5
	spawnRateRandom=0.1
	maxParticles=200
	particleLifetime=5
	particleLifetimeRand=0
	initialPitch=0
	initialPitchRand=0
	initialYaw=0
	initialYawRand=0
	initialVel=1
	initialVelRand=0
	inheritVelPct=0
	particleRadius=1
	radiusGrowth=0
	alphaMod=0
	initialAlpha=1
	rotation=0
	rotationRand=0
	lifetime=0
	friction=0
	isWorldSpace=true
	billboardUp=false
	spawnPosRandom="0 0 0"
	spawnPosOffset="0 0 0"
	>
		<material>
			<resource file=myparticlemat.xml/>
		</material>
</particleDesc>
*/
class GHParticleSystemDescXMLLoader : public GHXMLObjLoader
{
public:
	GHParticleSystemDescXMLLoader(GHXMLObjFactory& resourceFactory);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;

private:
	GHXMLObjFactory& mResourceFactory;
};
