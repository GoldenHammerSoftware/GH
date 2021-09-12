// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHFileOpener;

// <hfSaveMod filename="blah.hff"/>
class GHHeightFieldHFFSaveModifierXMLLoader : public GHXMLObjLoader
{
public:
	GHHeightFieldHFFSaveModifierXMLLoader(const GHFileOpener& fileOpener);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	const GHFileOpener& mFileOpener;
};
