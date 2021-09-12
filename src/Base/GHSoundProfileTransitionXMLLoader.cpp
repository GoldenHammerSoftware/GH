#include "GHSoundProfileTransitionXMLLoader.h"
#include "GHSoundProfileTransition.h"
#include "GHXMLNode.h"

GHSoundProfileTransitionXMLLoader::GHSoundProfileTransitionXMLLoader(GHSoundVolumeMgr& volumeMgr, GHStringIdFactory& hashTable)
	: mVolumeMgr(volumeMgr)
	, mIdFactory(hashTable)
{
}

void* GHSoundProfileTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	// this code is untested.  if tested, remove this comment.
	GHSoundProfile profile;

	const GHXMLNode* volumesNode = node.getChild("volumes", false);
	if (volumesNode)
	{
		for (size_t i = 0; i < volumesNode->getChildren().size(); ++i)
		{
			const GHXMLNode* currVolNode = volumesNode->getChildren()[i];
			GHIdentifier cat = 0;
			currVolNode->readAttrIdentifier("category", cat, mIdFactory);
			float volume = 1.0f;
			currVolNode->readAttrFloat("volume", volume);
			profile.setVolume(cat, volume);
		}
	}
	GHSoundProfileTransition* ret = new GHSoundProfileTransition(mVolumeMgr, profile);
	return ret;
}
