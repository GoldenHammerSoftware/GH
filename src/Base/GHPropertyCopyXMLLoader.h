// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include <assert.h>

class GHStringIdFactory;

// Copy a prop from extra data and return it.
// <copyProp src=id/>
class GHPropertyCopyXMLLoader : public GHXMLObjLoader
{
public:
	GHPropertyCopyXMLLoader(GHStringIdFactory& hashTable);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	GHStringIdFactory& mIdFactory;
};
