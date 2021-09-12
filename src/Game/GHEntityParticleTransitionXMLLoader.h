// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHXMLObjFactory;
class GHTimeVal;
class GHViewInfo;
class GHVBFactory;
class GHStringIdFactory;

/*
<entityParticleTransition>
	<particle>
		<particleDesc ...>
			<material>
				<resource file=myparticlemat.xml/>
			</material>
		</particleDesc>
	</particle>
</entityParticleTransition>
*/
// loads a particle system and attaches it to an entity's model.
class GHEntityParticleTransitionXMLLoader : public GHXMLObjLoader
{
public:
	GHEntityParticleTransitionXMLLoader(GHXMLObjFactory& xmlFactory, const GHTimeVal& time,
		const GHViewInfo& viewInfo, GHVBFactory& vbFactory, GHStringIdFactory& hashTable);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	GHXMLObjFactory& mXMLFactory;
	const GHTimeVal& mTime;
	const GHViewInfo& mViewInfo;
	GHVBFactory& mVBFactory;
	GHStringIdFactory& mIdFactory;
};
