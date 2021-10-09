#pragma once

#include "GHShaderParamList.h"
#include "GHUtils/GHResource.h"
#include "GHString/GHIdentifierMap.h"

class GHXMLSerializer;

// Loads the params for a shader from xml for platforms that don't have reflection.
// There are some dx-related assumptions that would have to be checked if we use it on another platform.
class GHShaderParamListLoader
{
public:
	GHShaderParamListLoader(GHXMLSerializer& xmlSerializer, const GHIdentifierMap<int>& enumStore);

	GHResourcePtr<GHShaderParamList>* loadParams(const char* shaderName) const;

private:
	GHXMLSerializer& mXMLSerializer;
	const GHIdentifierMap<int>& mEnumStore;
};