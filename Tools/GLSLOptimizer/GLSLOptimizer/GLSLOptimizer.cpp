// GLSLOptimizer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "GHWin32SystemServices.h"
#include "GHWin32Window.h"
#include "GHUtils\GHControllerMgr.h"
#include "GHShaderPreprocessor.h"
#include "GHPlatform\GHFileOpener.h"
#include "glsl_optimizer.h"
#include "GHPlatform\GHPrintfOutputPipe.h"

void initFileNames(std::vector<const char*>& vertexFileNames, std::vector<const char*>& pixelFileNames)
{
	pixelFileNames.push_back("ghbworldpixel.glsl");
	pixelFileNames.push_back("ghbworldreflectionpixel.glsl");
	pixelFileNames.push_back("ghbworldpixel_ue4packed.glsl");
	pixelFileNames.push_back("ghbworldpixel_ue4packed_emissive.glsl");
	pixelFileNames.push_back("ghbworldpixel_dynamic.glsl");
	pixelFileNames.push_back("fontpixel.glsl");
	pixelFileNames.push_back("AvatarFragmentShader.glsl");
	pixelFileNames.push_back("AvatarFragmentShaderPBS.glsl");
	pixelFileNames.push_back("ghblanepixel.glsl");
	pixelFileNames.push_back("ghbmattepixel.glsl");
	pixelFileNames.push_back("pointerpixel.glsl");

	vertexFileNames.push_back("ghbworldvertex_dynamic.glsl");
	vertexFileNames.push_back("ghbworldvertex.glsl");
	vertexFileNames.push_back("ghbworldreflectionvertex.glsl");
	vertexFileNames.push_back("fontvertex.glsl");
	vertexFileNames.push_back("AvatarVertexShader.glsl");
	vertexFileNames.push_back("ghblanevertex.glsl");
	vertexFileNames.push_back("ghbmattevertex.glsl");

	pixelFileNames.push_back("ballpbrdecalpixel.glsl");
	pixelFileNames.push_back("ballpbrdualcolorpixel.glsl");
	pixelFileNames.push_back("ballpbrpixel.glsl");
	pixelFileNames.push_back("ballpbrpixeladditivelighting.glsl");
	vertexFileNames.push_back("ballpbrvertex.glsl");
	vertexFileNames.push_back("ballpbrdecalvertex.glsl");
	vertexFileNames.push_back("ballpbriblvertex.glsl");
	pixelFileNames.push_back("ballpbriblpixel.glsl");

	pixelFileNames.push_back("obstaclepbrpixel.glsl");
	vertexFileNames.push_back("obstaclepbrvertex.glsl");
}

void optimizeShader(const char* inName, const char* outName, glslopt_shader_type shaderType, GHShaderPreprocessor& preprocessor,
	glslopt_ctx* ctx, const GHFileOpener& fileOpener, const char* commandLineDefines)
{
	// load shader source.
	std::vector<const char*> processedBuffers;
	std::vector<char*> deletionBuffers;

	// push any gl directives to the front of the list.
	const char* glExtensions = "#extension GL_OES_standard_derivatives : enable\n#extension GL_EXT_shader_texture_lod : enable\n";
	processedBuffers.push_back(glExtensions);
	// Tell our shader code that we're using the optimizer (for GL_EXT_shader_texture_lod workaround and any others we wind up needing)
	const char* glDefines = "#define GH_USING_GLSL_OPTIMIZER\n";
	processedBuffers.push_back(glDefines);
	int kNumPreprocessorStrings = 2;
	
	if (commandLineDefines)
	{
		processedBuffers.push_back(commandLineDefines);
		kNumPreprocessorStrings++;
	}
	/*else
	{
		// test to iterate on low quality shaders.
		processedBuffers.push_back("#define SHADER_QUALITY 0\n");
		kNumPreprocessorStrings++;
	}*/

	preprocessor.preprocessShaderFile(inName, processedBuffers, deletionBuffers);

	if (processedBuffers.size() < (kNumPreprocessorStrings + 1))
	{
		GHDebugMessage::outputString("Failed to preprocess shader file %s", outName);
		return;
	}

	// turn shader source into a single buffer.
	size_t bufferSize = 0;
	for (auto iter = processedBuffers.begin(); iter != processedBuffers.end(); ++iter)
	{
		bufferSize += strlen(*iter);
	}
	char* mergeBuf = new char[bufferSize + 1];
	memset(mergeBuf, 0, bufferSize + 1);
	for (auto iter = processedBuffers.begin(); iter != processedBuffers.end(); ++iter)
	{
		//GHDebugMessage::outputString("%s", *iter);
		sprintf(mergeBuf, "%s%s", mergeBuf, *iter);
	}

	//GHDebugMessage::outputString("%s", mergeBuf);

	// optimize shader.
	glslopt_shader* shader = glslopt_optimize(ctx, shaderType, mergeBuf, 0);
	if (glslopt_get_status(shader)) {
		const char* newSource = glslopt_get_output(shader);
		char outFileName[1024];
		sprintf(outFileName, "output/%s", outName);
		GHFile* writeFile = fileOpener.openFile(outFileName, GHFile::FT_TEXT, GHFile::FM_WRITEONLY);
		if (!writeFile)
		{
			GHDebugMessage::outputString("Failed to open writeFile %s", outFileName);
		}
		else
		{
			writeFile->writeBuffer(newSource, strlen(newSource));
			writeFile->closeFile();
			delete writeFile;
		}
	}
	else {
		const char* errorLog = glslopt_get_log(shader);
		GHDebugMessage::outputString("Failed to optimize shader %s %s", outName, errorLog);
	}
	glslopt_shader_delete(shader);

	// clean up.
	delete[] mergeBuf;
	for (auto iter = deletionBuffers.begin(); iter != deletionBuffers.end(); ++iter)
	{
		delete[] * iter;
	}
}

// generate a [version]_[filename] optimized shader using #define SHADER_QUALITY
void generateQualityVersionShader(const char* fileName, glslopt_shader_type shaderType, GHShaderPreprocessor& preprocessor,
	glslopt_ctx* ctx, const GHFileOpener& fileOpener, const char* commandLineDefines, const char* version)
{
	const size_t kMaxShaderNameLen = 4096;
	char optName[kMaxShaderNameLen];
	char optDefines[kMaxShaderNameLen];
	sprintf(optName, "%s_%s", version, fileName);
	sprintf(optDefines, "%s#define SHADER_QUALITY %s\n", commandLineDefines, version);
	optimizeShader(fileName, optName, shaderType, preprocessor, ctx, fileOpener, optDefines);
}

int main(int argc,  // Number of strings in array argv
	char* argv[],   // Array of command-line argument strings
	char* envp[])   // Array of environment variable strings
{
	GHControllerMgr systemControllerMgr;
	GHWin32Window dummyWindow(0, 0, 0, 0, "", 0, 0, 0, 0);
	GHWin32SystemServices* systemServices = new GHWin32SystemServices(dummyWindow, systemControllerMgr);

	// redirect output to printf since this is a command line app.
	// this lets us see output when using it.
	GHPrintfOutputPipe printPipe;
	GHDebugMessage::init(printPipe);

	// first clear out the existing files
	system("del output\\*.glsl");

	GHShaderPreprocessor preprocessor(systemServices->getPlatformServices().getFileOpener());

	// todo: move this to config.
	systemServices->getPlatformServices().getFileOpener().addResourcePath("../../../data/iOSShaders/");
	systemServices->getPlatformServices().getFileOpener().addResourcePath("../../../../Bowling/data/GHBowlingiOS/");

	glslopt_ctx* ctx = glslopt_initialize(kGlslTargetOpenGLES20);

	GHDebugMessage::outputString("Ready to optimize shaders!");

	std::vector<const char*> vertexFileNames;
	std::vector<const char*> pixelFileNames;
	initFileNames(vertexFileNames, pixelFileNames);

	const size_t kMaxCommandLine = 4096;
	char commandLineDefines[kMaxCommandLine];
	commandLineDefines[0] = '\0';
	bool generateLowQuality = false;
	for (size_t i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], "-o"))
		{
			generateLowQuality = true;
		}
		else
		{
			::sprintf(commandLineDefines, "%s%s\n", commandLineDefines, argv[i]);
		}
	}

	for (auto nameIter = vertexFileNames.begin(); nameIter != vertexFileNames.end(); ++nameIter)
	{
		const char* fileName = *nameIter;
		GHDebugMessage::outputString("Optimizing %s", fileName);

		optimizeShader(fileName, fileName, kGlslOptShaderVertex, preprocessor, ctx, systemServices->getPlatformServices().getFileOpener(), commandLineDefines);
		if (generateLowQuality)
		{
			generateQualityVersionShader(fileName, kGlslOptShaderVertex, preprocessor, ctx, systemServices->getPlatformServices().getFileOpener(), commandLineDefines, "0");
		}
	}
	for (auto nameIter = pixelFileNames.begin(); nameIter != pixelFileNames.end(); ++nameIter)
	{
		const char* fileName = *nameIter;
		GHDebugMessage::outputString("Optimizing %s", fileName);

		optimizeShader(fileName, fileName, kGlslOptShaderFragment, preprocessor, ctx, systemServices->getPlatformServices().getFileOpener(), commandLineDefines);
		if (generateLowQuality)
		{
			generateQualityVersionShader(fileName, kGlslOptShaderFragment, preprocessor, ctx, systemServices->getPlatformServices().getFileOpener(), commandLineDefines, "0");
		}
	}

	glslopt_cleanup(ctx);
	return 1;
}
