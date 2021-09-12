#include "GHShaderConverter.h"
#include "GHSystemServices.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHXMLNode.h"

GHShaderConverter::GHShaderConverter(GHSystemServices& systemServices)
	: mSystemServices(systemServices)
	, mPreprocessor(systemServices.getPlatformServices().getFileOpener())
{
}

void GHShaderConverter::runConfig(const char* configName)
{
	GHXMLNode* configNode = mSystemServices.getXMLSerializer().loadXMLFile(configName);
	if (!configNode)
	{
		GHDebugMessage::outputString("Failed to load shader config");
		return;
	}
	GHXMLNode* pathsNode = configNode->getChild("paths", false);
	if (pathsNode)
	{
		// paths relative to working dir.
		for (size_t i = 0; i < pathsNode->getChildren().size(); ++i)
		{
			const char* path = pathsNode->getChildren()[i]->getAttribute("dir");
			if (path)
			{
				mSystemServices.getPlatformServices().getFileOpener().addResourcePath(path);
			}
		}
	}
	mSystemServices.getPlatformServices().getFileOpener().addResourcePath("./");

	GHXMLNode* pixelsNode = configNode->getChild("pixelShaders", false);
	convertShaders(pixelsNode, GHShaderType::ST_PIXEL);
	GHXMLNode* vertexNode = configNode->getChild("vertexShaders", false);
	convertShaders(vertexNode, GHShaderType::ST_VERTEX);

	delete configNode;
}

void GHShaderConverter::convertShaders(const GHXMLNode* shadersNode, GHShaderType shaderType)
{
	if (!shadersNode)
	{
		return;
	}
	for (size_t i = 0; i < shadersNode->getChildren().size(); ++i)
	{
		GHXMLNode* shaderNode = shadersNode->getChildren()[i];
		const char* shaderName = shaderNode->getAttribute("name");
		if (!shaderName) continue;
		GHDebugMessage::outputString("Converting pixel %s", shaderName);

		bool convertGlsl = true;
		shaderNode->readAttrBool("convertGlsl", convertGlsl);
		bool convertMsl = true;
		shaderNode->readAttrBool("convertMsl", convertMsl);

		convertShader(shaderName, shaderType, convertGlsl, convertMsl);
	}
}

void GHShaderConverter::convertShader(const char* hlslName, GHShaderType shaderType, bool convertGlsl, bool convertMsl)
{
	char hlslMergedName[2048];
	snprintf(hlslMergedName, 2048, "shader/hlslout/%s", hlslName);

	std::vector<const char*> processedBuffers;
	std::vector<char*> deletionBuffers;
	preprocessShader(hlslName, processedBuffers, deletionBuffers);

	// if this shader supports SHADER_QUALITY then generate a low version.
	bool hasLowVersion = false;
	for (size_t i = 0; i < processedBuffers.size(); ++i)
	{
		if (strstr(processedBuffers[i], "SHADER_QUALITY"))
		{
			hasLowVersion = true;
			break;
		}
	}
	char lowMergedName[2048];
	char lowName[2048];
	if (hasLowVersion)
	{
		snprintf(lowMergedName, 2048, "shader/hlslout/0_%s", hlslName);
		snprintf(lowName, 2048, "0_%s", hlslName);
	}

	if (convertMsl)
	{
		std::vector<const char*> modBuffers;
		modBuffers.push_back("#define GH_METAL 1\n");
		writeShader(hlslMergedName, processedBuffers, modBuffers);

		mDXC.compileShaderToSpirv(mSystemServices.getPlatformServices().getFileOpener(),
			mSystemServices.getXMLSerializer(), hlslName, shaderType);

		mSpirvCross.compileShaderToMSL(mSystemServices.getPlatformServices().getFileOpener(), hlslName, shaderType);

		if (hasLowVersion)
		{
			modBuffers.push_back("#define SHADER_QUALITY 0\n");
			writeShader(lowMergedName, processedBuffers, modBuffers);

			mDXC.compileShaderToSpirv(mSystemServices.getPlatformServices().getFileOpener(),
				mSystemServices.getXMLSerializer(), lowName, shaderType);

			mSpirvCross.compileShaderToMSL(mSystemServices.getPlatformServices().getFileOpener(), lowName, shaderType);
		}
	}
	if (convertGlsl)
	{
		std::vector<const char*> modBuffers;
		modBuffers.push_back("#define GH_GLSL 1\n");
		writeShader(hlslMergedName, processedBuffers, modBuffers);

		mDXC.compileShaderToSpirv(mSystemServices.getPlatformServices().getFileOpener(),
			mSystemServices.getXMLSerializer(), hlslName, shaderType);

		mSpirvCross.compileShaderToGLES(mSystemServices.getPlatformServices().getFileOpener(), hlslName, shaderType);

		if (hasLowVersion)
		{
			modBuffers.push_back("#define SHADER_QUALITY 0\n");
			writeShader(lowMergedName, processedBuffers, modBuffers);

			mDXC.compileShaderToSpirv(mSystemServices.getPlatformServices().getFileOpener(),
				mSystemServices.getXMLSerializer(), lowName, shaderType);

			mSpirvCross.compileShaderToGLES(mSystemServices.getPlatformServices().getFileOpener(), lowName, shaderType);
		}
	}

	for (size_t i = 0; i < deletionBuffers.size(); ++i)
	{
		delete[] deletionBuffers[i];
	}
}

void GHShaderConverter::preprocessShader(const char* shaderName, std::vector<const char*>& processedBuffers, std::vector<char*>& deletionBuffers)
{
	mPreprocessor.preprocessShaderFile(shaderName, processedBuffers, deletionBuffers);
}

void GHShaderConverter::writeShader(const char* shaderOutName, const std::vector<const char*>& processedBuffers, const std::vector<const char*>& modBuffers)
{
	GHFile* outFile = mSystemServices.getPlatformServices().getFileOpener().openFile(shaderOutName, GHFile::FT_TEXT, GHFile::FM_WRITEONLY);
	if (!outFile)
	{
		GHDebugMessage::outputString("Failed to open hlslout file");
		return;
	}

	for (size_t i = 0; i < modBuffers.size(); ++i)
	{
		outFile->writeBuffer(modBuffers[i], strlen(modBuffers[i]));
	}
	for (size_t i = 0; i < processedBuffers.size(); ++i)
	{
		outFile->writeBuffer(processedBuffers[i], strlen(processedBuffers[i]));
	}
	outFile->closeFile();
	delete outFile;
}
