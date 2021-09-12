// Copyright Golden Hammer Software
#include "GHParticleSystemDescXMLLoader.h"
#include "GHParticleSystemDesc.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHXMLObjFactory.h"

GHParticleSystemDescXMLLoader::GHParticleSystemDescXMLLoader(GHXMLObjFactory& resourceFactory)
: mResourceFactory(resourceFactory)
{
}

void* GHParticleSystemDescXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHParticleSystemDesc* desc = new GHParticleSystemDesc;
	GHParticleSystemDescResource* ret = new GHParticleSystemDescResource(desc);
	populate(ret, node, extraData);
	return ret;
}

void GHParticleSystemDescXMLLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHParticleSystemDescResource* res = (GHParticleSystemDescResource*)obj;
	GHParticleSystemDesc* desc = res->get();
	GHPoint3 buffp3;
	buffp3 = desc->getGravity();
	node.readAttrFloatArr("gravity", buffp3.getArr(), 3);
	desc->setGravity(buffp3);

	float spawnRate = desc->getSpawnRate();
	node.readAttrFloat("spawnRate", spawnRate);
	desc->setSpawnRate(spawnRate);

	float spawnRateRandom = desc->getSpawnRateRandom();
	node.readAttrFloat("spawnRateRandom", spawnRateRandom);
	desc->setSpawnRateRandom(spawnRateRandom);

	unsigned int maxParticles = desc->getMaxParticles();
	node.readAttrUInt("maxParticles", maxParticles);
	desc->setMaxParticles(maxParticles);

	float particleLifetime = desc->getParticleLifetime();
	node.readAttrFloat("particleLifetime", particleLifetime);
	desc->setParticleLifetime(particleLifetime);

	float particleLifetimeRand = desc->getParticleLifetimeRand();
	node.readAttrFloat("particleLifetimeRand", particleLifetimeRand);
	desc->setParticleLifetimeRand(particleLifetimeRand);

	float initialPitch = desc->getInitialPitch();
	node.readAttrFloat("initialPitch", initialPitch);
	desc->setInitialPitch(initialPitch);

	float initialPitchRand = desc->getInitialPitchRand();
	node.readAttrFloat("initialPitchRand", initialPitchRand);
	desc->setInitialPitchRand(initialPitchRand);

	float initialYaw = desc->getInitialYaw();
	node.readAttrFloat("initialYaw", initialYaw);
	desc->setInitialYaw(initialYaw);

	float initialYawRand = desc->getInitialYawRand();
	node.readAttrFloat("initialYawRand", initialYawRand);
	desc->setInitialYawRand(initialYawRand);

	float initialVel = desc->getInitialVel();
	node.readAttrFloat("initialVel", initialVel);
	desc->setInitialVel(initialVel);

	float initialVelRand = desc->getInitialVelRand();
	node.readAttrFloat("initialVelRand", initialVelRand);
	desc->setInitialVelRand(initialVelRand);

	float inheritVelPct = desc->getInheritVelPct();
	node.readAttrFloat("inheritVelPct", inheritVelPct);
	desc->setInheritVelPct(inheritVelPct);

	float particleRadius = desc->getParticleRadius();
	node.readAttrFloat("particleRadius", particleRadius);
	desc->setParticleRadius(particleRadius);

	float radiusGrowth = desc->getRadiusGrowth();
	node.readAttrFloat("radiusGrowth", radiusGrowth);
	desc->setRadiusGrowth(radiusGrowth);

	float alphaMod = desc->getAlphaMod();
	node.readAttrFloat("alphaMod", alphaMod);
	desc->setAlphaMod(alphaMod);

	float initialAlpha = desc->getInitialAlpha();
	node.readAttrFloat("initialAlpha", initialAlpha);
	desc->setInitialAlpha(initialAlpha);

	float rotation = desc->getRotation();
	node.readAttrFloat("rotation", rotation);
	desc->setRotation(rotation);

	float rotationRand = desc->getRotationRandom();
	node.readAttrFloat("rotationRand", rotationRand);
	desc->setRotationRandom(rotationRand);

	float initialRotationRand = desc->getInitialRotationRandom();
	node.readAttrFloat("initialRotationRand", initialRotationRand);
	desc->setInitialRotationRandom(initialRotationRand);

	float lifetime = desc->getLifetime();
	node.readAttrFloat("lifetime", lifetime);
	desc->setLifetime(lifetime);

	float friction = desc->getFriction();
	node.readAttrFloat("friction", friction);
	desc->setFriction(friction);

	bool isWorldSpace = desc->getIsWorldSpace();
	node.readAttrBool("isWorldSpace", isWorldSpace);
	desc->setIsWorldSpace(isWorldSpace);

	bool billboardUp = desc->getBillboardUp();
	node.readAttrBool("billboardUp", billboardUp);
	desc->setBillboardUp(billboardUp);

	buffp3 = desc->getSpawnPosRandom();
	node.readAttrFloatArr("spawnPosRandom", buffp3.getArr(), 3);
	desc->setSpawnPosRandom(buffp3);

	buffp3 = desc->getSpawnPosOffset();
	node.readAttrFloatArr("spawnPosOffset", buffp3.getArr(), 3);
	desc->setSpawnPosOffset(buffp3);

	const GHXMLNode* matNode = node.getChild("material", false);
	if (matNode && matNode->getChildren().size())
	{
		GHMaterial* mat = (GHMaterial*)mResourceFactory.load(*matNode->getChildren()[0], &extraData);
		if (!mat)
		{
			GHDebugMessage::outputString("Failed to load mat for particle system");
		}
		desc->setMaterial(mat);
	}
	else
	{
		GHDebugMessage::outputString("No material node found.");
	}
}
