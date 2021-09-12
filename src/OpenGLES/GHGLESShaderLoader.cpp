// Copyright 2010 Golden Hammer Software
#include "GHGLESShaderLoader.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHGLESShader.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGLESInclude.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHGLESIdentifiers.h"
#include <assert.h>
#include "GHGLESErrorReporter.h"
#include "GHShaderPreprocessor.h"

GHGLESShaderLoader::GHGLESShaderLoader(const GHFileOpener& fileOpener, GHEventMgr& eventMgr)
: mFileOpener(fileOpener)
, mEventMgr(eventMgr)
{
}

GHResource* GHGLESShaderLoader::loadFile(const char* fileName, GHPropertyContainer* extraData)
{
    assert(extraData);
    GLenum type = (unsigned int)extraData->getProperty(GHGLESIdentifiers::SHADERTYPE);
    if (type != GL_VERTEX_SHADER && type != GL_FRAGMENT_SHADER) {
    	int brkpt = 1;
    	brkpt++;
    }
    
    assert( (type == GL_VERTEX_SHADER) || (type == GL_FRAGMENT_SHADER) );
    
	// first load the default version of the shader.
	bool expectsDXLayout = false;
    unsigned int shader = loadShader(fileName, type, false, expectsDXLayout);
    if (shader == 0) {
        return 0;
    }
    GHGLESShader* ret = new GHGLESShader(shader, *this, mEventMgr, fileName, type);
	ret->setExpectsDXLayout(expectsDXLayout);

	// then look for any quality-specific versions.
	char qualityName[2048];
	sprintf(qualityName, "%d_%s", 0, fileName);
	unsigned int lowGLShader = loadShader(qualityName, type, true, expectsDXLayout);
	if (lowGLShader != 0)
	{
		GHGLESShader* lowGHShader = new GHGLESShader(lowGLShader, *this, mEventMgr, qualityName, type);
		lowGHShader->setExpectsDXLayout(expectsDXLayout);
		ret->addQualityVersion(0, lowGHShader);
	}

	return ret;
}

unsigned int GHGLESShaderLoader::loadShader(const char* fileName, GLenum type, bool noSpam, bool& expectsDXLayout)
{
    unsigned int ret = 0;

	unsigned int retId = glCreateShader(type);
	if (retId == 0)
	{
		if (!noSpam) GHDebugMessage::outputString("Failed to glCreateShader %s.  Out of handles?", fileName);
		return 0;
	}

	std::vector<const char*> processedBuffers;
	std::vector<char*> deletionBuffers;

	GHShaderPreprocessor preprocessor(mFileOpener);
	preprocessor.preprocessShaderFile(fileName, processedBuffers, deletionBuffers);

	char versionBuf[512];
	if (processedBuffers.size())
	{
		std::vector<const char*> modifiedPBs;
		modifiedPBs.reserve(processedBuffers.size() + 2);
		// push any gl directives to the front of the list after the version.
		const char* glExtensions = "#extension GL_OES_standard_derivatives : enable\n#extension GL_EXT_shader_texture_lod : enable\n";

		// if the first line is #version then pop it into the first buffer.
		// version has to be the first line in the list, so if we want any run-time compile flags we need to identify it.
		char* versionStart = (char*)strstr(processedBuffers[0], "#version");
		char* newlineStart = (char*)strchr(processedBuffers[0], '\n');
		size_t firstProcessedIdx = 0;
		if (versionStart)
		{
			memset(versionBuf, 0, 512);
			char* versionStr = (char*)processedBuffers[0];
			const char* rebuffer = newlineStart + 1;
			*newlineStart = '\0';
			sprintf(versionBuf, "%s\n", versionStr);

			modifiedPBs.push_back(versionBuf);
			modifiedPBs.push_back(glExtensions);
			modifiedPBs.push_back(rebuffer);
			firstProcessedIdx = 1;

			// assuming that if the shader specifies version then it's one of our new shaders and uses dx layout.
			// kinda a hack but eventually all shaders will be the new style
			expectsDXLayout = true;
		}
		else
		{
			modifiedPBs.push_back(glExtensions);
			expectsDXLayout = false;
		}

		for (size_t i = firstProcessedIdx; i < processedBuffers.size(); ++i)
		{
			modifiedPBs.push_back(processedBuffers[i]);
		}
		processedBuffers = modifiedPBs;
	}

#ifdef GH_IOS
	const char* glDefines = "#define GH_IOS\n";
	processedBuffers.push_back(glDefines);
	const int kNumPreprocessorStrings = 2;
#else
	const int kNumPreprocessorStrings = 1;
#endif

	if (processedBuffers.size() < (kNumPreprocessorStrings + 1))
	{
		if (!noSpam) GHDebugMessage::outputString("Failed to preprocess shader file %s", fileName);
		glDeleteShader(retId);
		return 0;
	}
	//GHDebugMessage::outputString("Loaded shader file %s", fileName);

//#define SPIT_OUT_SHADER_CODE 1
#ifdef SPIT_OUT_SHADER_CODE
	GHDebugMessage::outputString("Shader code for %s", fileName);
	for (int i = 0; i < processedBuffers.size(); ++i)
	{
		GHDebugMessage::outputString("%s", processedBuffers[i]);
	}
#endif

	// reinterpret the vec<char> as an array of GLchar for passing to gl.
	GLchar** glBuffers = (GLchar**)&processedBuffers[0];

	{
		glShaderSource(retId, processedBuffers.size(), glBuffers, NULL);
		glCompileShader(retId);

		int success;
		glGetShaderiv(retId, GL_COMPILE_STATUS, &success);
		if (success == 0)
		{
			char errorMsg[2048];
			glGetShaderInfoLog(retId, sizeof(errorMsg), NULL, errorMsg);
			GHDebugMessage::outputString("%s compile error: %s\n", fileName, errorMsg);

			for (size_t i = 0; i < processedBuffers.size(); ++i)
			{
			    GHDebugMessage::outputString("%s", processedBuffers[i]);
			}

			glDeleteShader(retId);
			ret = 0;
		}
		ret = retId;
	}

	// clean up afterwards.
	for (size_t i = 0; i < deletionBuffers.size(); ++i)
	{
		delete [] deletionBuffers[i];
	}

	return ret;
}

