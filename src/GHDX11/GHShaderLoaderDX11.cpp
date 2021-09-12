// Copyright Golden Hammer Software

//#define OUTPUT_SHADER_INSTRUCTION_COUNT
#ifdef OUTPUT_SHADER_INSTRUCTION_COUNT
	#define INITGUID
	#include "D3D11Shader.h"
	#include "D3Dcompiler.h"
	#include "GHPlatform/GHDebugMessage.h"
#endif

#include "GHShaderLoaderDX11.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHDX11Include.h"
#include "GHRenderDeviceDX11.h"
#include "GHShaderDX11.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHPropertiesDX11.h"
#include "GHXMLSerializer.h"
#include "GHXMLNode.h"
#include "GHMaterialCallbackType.h"
#include "GHShaderConcreteDX11.h"
#include "GHMultiShaderDX11.h"

GHShaderLoaderDX11::GHShaderLoaderDX11(GHFileOpener& fileOpener, GHRenderDeviceDX11& device,
									   GHShaderDX11*& activeVS, GHXMLSerializer& xmlSerializer,
									   const GHIdentifierMap<int>& enumStore,
									   const int& graphicsQuality, const char* shaderDirectory,
									   GHEventMgr& eventMgr)
: mFileOpener(fileOpener)
, mDevice(device)
, mActiveVS(activeVS)
, mXMLSerializer(xmlSerializer)
, mEnumStore(enumStore)
, mGraphicsQuality(graphicsQuality)
, mShaderDirectory(shaderDirectory, GHString::CHT_COPY)
, mEventMgr(eventMgr)
{
}

GHResource* GHShaderLoaderDX11::loadFile(const char* filename, GHPropertyContainer* extraData)
{
	if (!extraData) return 0;
	int shaderType = extraData->getProperty(GHPropertiesDX11::SHADERTYPE);
	GHShaderDX11* retRaw = 0;
	GHResourcePtr<GHShaderParamListDX11>* params = loadParams(filename);
    params->acquire();
    
	char lowName[512];
	::sprintf(lowName, "%s0-%s", mShaderDirectory.getChars(), filename);
	GHShaderDX11* lowShader = 0;
	char medName[512];
	::sprintf(medName, "%s1-%s", mShaderDirectory.getChars(), filename);
	GHShaderDX11* medShader = 0;
	char baseName[512];
	::sprintf(baseName, "%s%s", mShaderDirectory.getChars(), filename);

	if (shaderType == 0) {
		retRaw = loadVS(baseName, params);
		lowShader = loadVS(lowName, params);
		medShader = loadVS(medName, params);
	}
	else {
		retRaw = loadPS(baseName, params);
		lowShader = loadPS(lowName, params);
		medShader = loadPS(medName, params);
	}
    // now that params has been added to any shaders, we can release our local handle.
    params->release();

	if (!retRaw) {
		GHDebugMessage::outputString("Failed to load shader %s", filename);
		if (lowShader || medShader) {
            if (lowShader) delete lowShader;
            if (medShader) delete medShader;
        }
		return 0;
	}
	if (lowShader || medShader)
	{
		GHMultiShaderDX11* retMulti = new GHMultiShaderDX11(mGraphicsQuality, new GHShaderResource(retRaw));
		GHShaderResource* medres = 0;
		if (medShader) medres = new GHShaderResource(medShader);
		if (lowShader) retMulti->addShader(0, new GHShaderResource(lowShader));
		else if (medShader) retMulti->addShader(0, medres);
		if (medShader) retMulti->addShader(1, medres);
		return new GHShaderResource(retMulti);
	}
	return new GHShaderResource(retRaw);
}

GHShaderDX11* GHShaderLoaderDX11::loadVS(const char* shaderName, GHResourcePtr<GHShaderParamListDX11>* params) const
{
	GHFile* file = mFileOpener.openFile(shaderName, GHFile::FT_BINARY, GHFile::FM_READONLY);
	if (!file) {
		return 0;
	}
	char* fileBuf = 0;
	size_t fileLen = 0;
	file->readIntoBuffer();
	file->getFileBuffer(fileBuf, fileLen);
	file->releaseFileBuffer();

#ifdef OUTPUT_SHADER_INSTRUCTION_COUNT
	ID3D11ShaderReflection* pReflector = NULL;
	D3DReflect(fileBuf, fileLen, IID_ID3D11ShaderReflection, (void**)&pReflector);
	D3D11_SHADER_DESC shaderDesc;
	pReflector->GetDesc(&shaderDesc);
	GHDebugMessage::outputString("Shader %s instruction count %d", shaderName, shaderDesc.InstructionCount);
#endif

	GHShaderConcreteDX11* ret = new GHShaderConcreteDX11(mDevice, fileBuf, fileLen, mActiveVS, params, mEventMgr, shaderName);
	if (!ret->initAsVS())
	{
		GHDebugMessage::outputString("Failed to compile VS %s", shaderName);
		delete ret;
		ret = 0;
	}

	delete file;
	return ret;
}

GHShaderDX11* GHShaderLoaderDX11::loadPS(const char* shaderName, GHResourcePtr<GHShaderParamListDX11>* params) const
{
	GHFile* file = mFileOpener.openFile(shaderName, GHFile::FT_BINARY, GHFile::FM_READONLY);
	if (!file) {
		return 0;
	}
	char* fileBuf = 0;
	size_t fileLen = 0;
	file->readIntoBuffer();
	file->getFileBuffer(fileBuf, fileLen);
	file->releaseFileBuffer();

#ifdef OUTPUT_SHADER_INSTRUCTION_COUNT
	ID3D11ShaderReflection* pReflector = NULL;
	D3DReflect(fileBuf, fileLen, IID_ID3D11ShaderReflection, (void**)&pReflector);
	D3D11_SHADER_DESC shaderDesc;
	pReflector->GetDesc(&shaderDesc);
	GHDebugMessage::outputString("Shader %s instruction count %d", shaderName, shaderDesc.InstructionCount);
#endif

	GHShaderConcreteDX11* ret = new GHShaderConcreteDX11(mDevice, fileBuf, fileLen, mActiveVS, params, mEventMgr, shaderName);
	if (!ret->initAsPS())
	{
		GHDebugMessage::outputString("Failed to compile PS %s", shaderName);
		delete ret;
		ret = 0;
	}

	delete file;
	return ret;
}

GHResourcePtr<GHShaderParamListDX11>* GHShaderLoaderDX11::loadParams(const char* shaderName) const
{
	GHShaderParamListDX11* ret = new GHShaderParamListDX11();
	GHResourcePtr<GHShaderParamListDX11>* retPtr = new GHResourcePtr<GHShaderParamListDX11>(ret);

	// we're looking for shadername+params.xml
	char paramFileName[2048];
	assert(strlen(shaderName) < 2030);

	const char* extStart = ::strrchr(shaderName, '.');
	if (!extStart) {
		::snprintf(paramFileName, 1024, "%sparams.xml", shaderName);
	}
	else {
		::snprintf(paramFileName, extStart-shaderName+1, "%s", shaderName);
		paramFileName[extStart-shaderName] = '\0';
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
			GHShaderParamListDX11::Param param;
			param.mHandleType = GHMaterialParamHandle::HT_TEXTURE;
			childList[i]->readAttrUInt("register", param.mRegister);

			loadSharedArguments(param, *childList[i]);
			ret->addParam(param);
		}
	}
	const GHXMLNode* floatsNode = node->getChild("floatParams", false);
	if (floatsNode)
	{
		const GHXMLNode::NodeList& childList = floatsNode->getChildren();
		for (unsigned int i = 0; i < childList.size(); ++i)
		{
			GHShaderParamListDX11::Param param;
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

			loadSharedArguments(param, *childList[i]);
			ret->addParam(param);
		}
	}
	ret->calcParamOffsets();
	return retPtr;
}

void GHShaderLoaderDX11::loadSharedArguments(GHShaderParamListDX11::Param& param, const GHXMLNode& node) const
{
	param.mCBType = GHMaterialCallbackType::CT_PERFRAME;
	const char* cbTypeStr = node.getAttribute("cbType");
	if (cbTypeStr) {
		const int* cbTypePtr = mEnumStore.find(cbTypeStr);
		if (cbTypePtr) {
			param.mCBType = (GHMaterialCallbackType::Enum)*cbTypePtr;
		}
	}

	const char* semanticStr = node.getAttribute("semantic");
	param.mSemantic.setConstChars(semanticStr, GHString::CHT_COPY);

	node.readAttrUInt("count", param.mCount);
}
