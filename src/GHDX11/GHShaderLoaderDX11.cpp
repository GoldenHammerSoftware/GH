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
#include "GHMaterialCallbackType.h"
#include "GHShaderConcreteDX11.h"
#include "GHMultiShaderDX11.h"

GHShaderLoaderDX11::GHShaderLoaderDX11(GHFileOpener& fileOpener, GHRenderDeviceDX11& device,
									   GHShaderDX11*& activeVS, const GHShaderParamListLoader& paramListLoader,
									   const int& graphicsQuality, const char* shaderDirectory,
									   GHEventMgr& eventMgr)
: mFileOpener(fileOpener)
, mDevice(device)
, mActiveVS(activeVS)
, mParamListLoader(paramListLoader)
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
	GHResourcePtr<GHShaderParamList>* params = mParamListLoader.loadParams(filename);
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

GHShaderDX11* GHShaderLoaderDX11::loadVS(const char* shaderName, GHResourcePtr<GHShaderParamList>* params) const
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

GHShaderDX11* GHShaderLoaderDX11::loadPS(const char* shaderName, GHResourcePtr<GHShaderParamList>* params) const
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
