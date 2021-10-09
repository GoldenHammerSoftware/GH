#include "GHShaderParamListLoader.h"
#include "Base/GHXMLSerializer.h"
#include "Base/GHXMLNode.h"

GHShaderParamListLoader::GHShaderParamListLoader(GHXMLSerializer& xmlSerializer, const GHIdentifierMap<int>& enumStore)
	: mXMLSerializer(xmlSerializer)
	, mEnumStore(enumStore)
{
}

static void loadSharedArguments(GHShaderParamList::Param& param, const GHXMLNode& node, const GHIdentifierMap<int>& enumStore)
{
	param.mCBType = GHMaterialCallbackType::CT_PERFRAME;
	const char* cbTypeStr = node.getAttribute("cbType");
	if (cbTypeStr) {
		const int* cbTypePtr = enumStore.find(cbTypeStr);
		if (cbTypePtr) {
			param.mCBType = (GHMaterialCallbackType::Enum)*cbTypePtr;
		}
	}

	const char* semanticStr = node.getAttribute("semantic");
	param.mSemantic.setConstChars(semanticStr, GHString::CHT_COPY);

	node.readAttrUInt("count", param.mCount);
}

GHResourcePtr<GHShaderParamList>* GHShaderParamListLoader::loadParams(const char* shaderName) const
{
	GHShaderParamList* ret = new GHShaderParamList();
	GHResourcePtr<GHShaderParamList>* retPtr = new GHResourcePtr<GHShaderParamList>(ret);

	// we're looking for shadername+params.xml
	char paramFileName[2048];
	assert(strlen(shaderName) < 2030);

	const char* extStart = ::strrchr(shaderName, '.');
	if (!extStart) {
		::snprintf(paramFileName, 1024, "%sparams.xml", shaderName);
	}
	else {
		::snprintf(paramFileName, extStart - shaderName + 1, "%s", shaderName);
		paramFileName[extStart - shaderName] = '\0';
		::snprintf(paramFileName, 1024, "%sparams.xml", paramFileName);
	}

	GHXMLNode* node = mXMLSerializer.loadXMLFile(paramFileName);
	if (!node) {
		return retPtr;
	}

	const GHXMLNode* texturesNode = node->getChild("textureParams", false);
	if (texturesNode)
	{
		const GHXMLNode::NodeList& childList = texturesNode->getChildren();
		for (unsigned int i = 0; i < childList.size(); ++i)
		{
			GHShaderParamList::Param param;
			param.mHandleType = GHMaterialParamHandle::HT_TEXTURE;
			childList[i]->readAttrUInt("register", param.mRegister);

			loadSharedArguments(param, *childList[i], mEnumStore);
			ret->addParam(param);
		}
	}
	const GHXMLNode* floatsNode = node->getChild("floatParams", false);
	if (floatsNode)
	{
		const GHXMLNode::NodeList& childList = floatsNode->getChildren();
		for (unsigned int i = 0; i < childList.size(); ++i)
		{
			GHShaderParamList::Param param;
			unsigned int sizeVals;
			childList[i]->readAttrUInt("sizeFloats", sizeVals);
			sizeVals *= sizeof(float);

			const char* typeAttr = childList[i]->getAttribute("type");
			//default to float. Treat bools as float too
			if (!typeAttr || !strcmp(typeAttr, "float") || !strcmp(typeAttr, "bool"))
			{
				param.mHandleType = GHMaterialParamHandle::calcHandleType(sizeVals);
			}
			else
			{
				if (!strcmp(typeAttr, "int"))
				{
					param.mHandleType = GHMaterialParamHandle::calcIntHandleType(sizeVals);
				}
				else
				{
					//default to float anyway
					assert(false && "invalid type for floatParam (supported: float, bool, int)");
					param.mHandleType = GHMaterialParamHandle::calcHandleType(sizeVals);
				}
			}

			loadSharedArguments(param, *childList[i], mEnumStore);
			ret->addParam(param);
		}
	}
	ret->calcParamOffsets();
	return retPtr;
}
