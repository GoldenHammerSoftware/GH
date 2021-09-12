#pragma once

#include "GHShaderPreprocessor.h"
#include "GHDXC.h"
#include "GHShaderType.h"
#include "GHSpirvCross.h"

class GHSystemServices;
class GHXMLNode;

class GHShaderConverter
{
public:
	GHShaderConverter(GHSystemServices& systemServices);

	void runConfig(const char* configName);
	void convertShader(const char* hlslName, GHShaderType shaderType, bool convertGlsl, bool convertMsl);

private:
	void convertShaders(const GHXMLNode* shadersNode, GHShaderType shaderType);
	void preprocessShader(const char* shaderName, std::vector<const char*>& processedBuffers, std::vector<char*>& deletionBuffers);
	void writeShader(const char* shaderOutName, const std::vector<const char*>& processedBuffers, const std::vector<const char*>& modBuffers);

private:
	GHSystemServices& mSystemServices;
	GHShaderPreprocessor mPreprocessor;
	GHDXC mDXC;
	GHSpirvCross mSpirvCross;
};

