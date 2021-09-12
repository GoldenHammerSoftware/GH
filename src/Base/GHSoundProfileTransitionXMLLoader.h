#pragma once
#include "GHXMLObjLoader.h"

class GHSoundVolumeMgr;
class GHStringIdFactory;

/*
<soundProfileTransition>
	<volumes>
		<volume category="snowboarder" volume="1.0"/>
	</volumes>
</soundProfileTransition>
*/
class GHSoundProfileTransitionXMLLoader : public GHXMLObjLoader
{
public:
	GHSoundProfileTransitionXMLLoader(GHSoundVolumeMgr& volumeMgr, GHStringIdFactory& hashTable);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	GHSoundVolumeMgr& mVolumeMgr;
	GHStringIdFactory& mIdFactory;
};
