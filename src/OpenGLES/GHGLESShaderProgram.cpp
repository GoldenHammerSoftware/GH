// Copyright 2010 Golden Hammer Software
#include "GHGLESShaderProgram.h"
#include "GHGLESInclude.h"
#include "GHGLESShader.h"
#include <assert.h>
#include "GHUtils/GHProfiler.h"
#include "GHVertexComponentShaderID.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGLESShaderMgr.h"
#include "GHGLESErrorReporter.h"
#include "GHString/GHStringIdFactory.h"

GHGLESShaderProgram::GHGLESShaderProgram(unsigned int program, GHGLESShader* vertShader, 
	GHGLESShader* pixelShader, GHGLESShaderMgr& shaderMgr, const GHStringIdFactory& stringHasher,
	const int& graphicsQuality)
: mShaderProgram(program)
, mShaderMgr(shaderMgr)
, mStringHasher(stringHasher)
, mGraphicsQuality(graphicsQuality)
{
    assert(vertShader);
    assert(pixelShader);
    
    vertShader->acquire();
    mAttachedShaders.push_back(vertShader);
    pixelShader->acquire();
    mAttachedShaders.push_back(pixelShader);
    mShaderMgr.registerShaderProgram(this);

	// GHGLESMaterialSHLoader already linked the shaders.
	// grab the reflected uniform data of the default versions so we know what any quality version could support.
    cacheReflectedUniformData();

	// then bind the quality version instead.
	if (hasDifferentVersionForQuality())
	{
		// we need to start with a new program or else we'll get GL_INVALID_OPERATION
		glDeleteProgram(mShaderProgram);
		mShaderProgram = glCreateProgram();
		linkShaders(mShaderProgram, mAttachedShaders[0]->getVersionForQuality(mGraphicsQuality),
			mAttachedShaders[1]->getVersionForQuality(mGraphicsQuality));
		// do not cache the uniform data for the quality version.
	}
}

GHGLESShaderProgram::~GHGLESShaderProgram(void)
{
    mShaderMgr.unregisterShaderProgram(this);
    for (int i = 0; i < mAttachedShaders.size(); ++i) {
        glDetachShader(mShaderProgram, mAttachedShaders[i]->getId());
        mAttachedShaders[i]->release();
    }
    glDeleteProgram(mShaderProgram);
	GHGLESErrorReporter::checkAndReportError("glDeleteProgram");
}

void GHGLESShaderProgram::reloadShaders(void)
{
    for (int i = 0; i < mAttachedShaders.size(); ++i) {
        GHGLESShader* shader = mAttachedShaders[i];
        shader->reload();
    }
    mShaderProgram = glCreateProgram();
	if (mShaderProgram == 0) { GHDebugMessage::outputString("failed to reload shader"); return; }

	linkShaders(mShaderProgram, mAttachedShaders[0]->getVersionForQuality(mGraphicsQuality),
		mAttachedShaders[1]->getVersionForQuality(mGraphicsQuality));
	// do not recache the uniform data.  we use this stored data to reference what is in the default version
	// if the current version does not support a uniform.
	// this could cause problems if we were reloading a major change to the shader,
	//  but we're avoiding link default, cache, then link quality version.
	//cacheReflectedUniformData();

	GHGLESErrorReporter::checkAndReportError("reloadShaders");
}

void GHGLESShaderProgram::handleGraphicsQualityChanged(void)
{
	// we may have different versions for quality so link to the new ones.
	mShaderProgram = glCreateProgram();
	if (mShaderProgram == 0) { GHDebugMessage::outputString("failed to reload shader"); return; }

	linkShaders(mShaderProgram, mAttachedShaders[0]->getVersionForQuality(mGraphicsQuality),
		mAttachedShaders[1]->getVersionForQuality(mGraphicsQuality));
	// do not recache the uniform data.  we use this stored data to reference what is in the default version
	// if the current version does not support a uniform.
	// this could cause problems if we were reloading a major change to the shader,
	//  but we're avoiding link default, cache, then link quality version.
	//cacheReflectedUniformData();
}

static void bindVertexAttributes(unsigned int shaderId)
{
	// This is how the shader knows what pieces of data to use from the vb.
	// When we upload the data we bind offsets to GHVertexComponentShaderID locations.
	// The shaders are expected to use a consistent naming convention for those ids.
	glBindAttribLocation(shaderId, GHVertexComponentShaderID::SI_POS, "position");
	glBindAttribLocation(shaderId, GHVertexComponentShaderID::SI_NORMAL, "normal");
	glBindAttribLocation(shaderId, GHVertexComponentShaderID::SI_TANGENT, "tangent");
	glBindAttribLocation(shaderId, GHVertexComponentShaderID::SI_DIFFUSE, "diffuse");
	glBindAttribLocation(shaderId, GHVertexComponentShaderID::SI_SPECULAR, "specular");
	glBindAttribLocation(shaderId, GHVertexComponentShaderID::SI_UV1, "uv1");
	glBindAttribLocation(shaderId, GHVertexComponentShaderID::SI_UV2, "uv2");
	glBindAttribLocation(shaderId, GHVertexComponentShaderID::SI_BONEIDX, "boneIndices");
	glBindAttribLocation(shaderId, GHVertexComponentShaderID::SI_BONEWEIGHT, "boneWeights");
	GHGLESErrorReporter::checkAndReportError("linkShaders - glBindAttribLocation");
}

bool GHGLESShaderProgram::linkShaders(unsigned int& retId, GHGLESShader* vertShader, GHGLESShader* pixelShader)
{
	GHGLESErrorReporter::checkAndReportError("pre-linkShaders");

    GHPROFILEBEGIN("linkShaderProgram")
	glAttachShader(retId, vertShader->getId());
	GHGLESErrorReporter::checkAndReportError("linkShaders - glAttachShader vertex");
	glAttachShader(retId, pixelShader->getId());
	GHGLESErrorReporter::checkAndReportError("linkShaders - glAttachShader pixel");

	bindVertexAttributes(retId);

	glLinkProgram(retId);
	GHGLESErrorReporter::checkAndReportError("linkShaders - glLinkProgram");
	int linked;
	glGetProgramiv(retId, GL_LINK_STATUS, &linked);
    GHPROFILEEND("linkShaderProgram")
	if (linked == 0)
	{
        int maxLength;
        int length;
        glGetProgramiv(retId,GL_INFO_LOG_LENGTH,&maxLength);
		GHGLESErrorReporter::checkAndReportError("linkShaders - get GL_INFO_LOG_LENGTH");
        char* log = new char[maxLength+1];
        //::memcpy(log, 0, maxLength);
        glGetProgramInfoLog(retId,maxLength,&length,log);
		GHGLESErrorReporter::checkAndReportError("linkShaders - getProgramInfoLog");
        GHDebugMessage::outputString("glGetProgramInfoLog %s", log);
		delete[] log;

		glDeleteProgram(retId);
		// don't handle deleting the vs and ps here, let the caller decide.
        //vertShader->acquire(); vertShader->release();
        //pixelShader->acquire(); pixelShader->release();
		GHDebugMessage::outputString("Failed to link shader program.");
		GHGLESErrorReporter::checkAndReportError("linkShaders - failure");
		return false;
	}
	GHGLESErrorReporter::checkAndReportError("linkShaders - success");
    return true;
}

void GHGLESShaderProgram::getReflectedUniformData(const char* uniformName, const GHGLESShaderProgram::ReflectedUniformData*& outData) const
{
	GHIdentifier nameHash = mStringHasher.generateHash(uniformName);
	getReflectedUniformData(nameHash, outData);
}

void GHGLESShaderProgram::getReflectedUniformData(const GHIdentifier& uniformNameHash, const ReflectedUniformData*& outData) const
{
	auto iter = mReflectedUniformData.find(uniformNameHash);
	if (iter == mReflectedUniformData.end())
	{
		outData = 0;
	}
	else
	{
		outData = &iter->second;
	}
}

void GHGLESShaderProgram::cacheReflectedUniformData(void)
{
	mReflectedUniformData.clear();

	int numUniforms = -1;
	glGetProgramiv(mShaderProgram, GL_ACTIVE_UNIFORMS, &numUniforms);
	if (!GHGLESErrorReporter::checkAndReportError("glGetProgramiv"))
	{
		return;
	}

	constexpr int kNameBufSize = 512;
	char nameBuf[kNameBufSize] = { 0 };
	
	for (int i = 0; i < numUniforms; ++i)
	{
		nameBuf[0] = 0;
		GLsizei actualNameLength = 0; //will be the number of characters actually written to nameBuf.

		//From https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetActiveUniform.xhtml:
		//If one or more elements of an array are active, the name of the array is returned in name, the type is returned in type, 
		// and the size parameter returns the highest array element index used, plus one, as determined by the compiler and/or linker.
		// Only one active uniform variable will be reported for a uniform array...
		//The size of the uniform variable will be returned in size. Uniform variables other than arrays will have a size of 1. 
		// Structures and arrays of structures will be reduced as described earlier, such that each of the names returned will be 
		// a data type in the earlier list. If this reduction results in an array, the size returned will be as described for uniform arrays; 
		// otherwise, the size returned will be 1.

		GLint size = 1;
		GLenum type = -1;
		glGetActiveUniform(mShaderProgram, i, kNameBufSize, &actualNameLength, &size, &type, nameBuf);
		if (GHGLESErrorReporter::checkAndReportError("glGetActiveUniform"))
		{
			//Array names might be output as "myArray" or "myArray[0]" depending on driver implementation. We just want to hash "myArray"
			char* bracket = strchr(nameBuf, '[');
			if (bracket)
			{
				*bracket = 0;
			}

			char* lookupName = nameBuf;
			// If this is one of the new shaders with things in buffers we need to strip off the buffer name.
			char* dotChar = strchr(lookupName, '.');
			if (dotChar)
			{
				lookupName = dotChar + 1;
			}

			GHIdentifier nameHash = mStringHasher.generateHash(lookupName);
			mReflectedUniformData[nameHash] = ReflectedUniformData(size);
		}
	}
}

int GHGLESShaderProgram::getUniformSize(const char* uniformName)
{
	const GHGLESShaderProgram::ReflectedUniformData* reflectedData = 0;
	getReflectedUniformData(uniformName, reflectedData);
	int size = 1;
	if (reflectedData)
	{
		size = reflectedData->mSize;
	}
	return size;
}

int GHGLESShaderProgram::getUniformLocation(const char* uniformName)
{
	int ret = glGetUniformLocation(getShaderProgram(), uniformName);

	const char* bufferNames[] = {
		"Vertex_PerFrameBuffer",
		"Vertex_PerTransBuffer",
		"Vertex_PerEntBuffer",
		"Vertex_PerGeoBuffer",
		"Pixel_PerFrameBuffer",
		"Pixel_PerTransBuffer",
		"Pixel_PerEntBuffer",
		"Pixel_PerGeoBuffer"
	};
	const size_t numBufferNames = 8;

	if (ret == -1)
	{
		char bufferVarName[512];
		// try looking in buffers.
		for (int i = 0; i < numBufferNames; ++i)
		{
			sprintf(bufferVarName, "%s.%s", bufferNames[i], uniformName);
			ret = glGetUniformLocation(getShaderProgram(), bufferVarName);
			if (ret != -1)
			{
				break;
			}
		}
	}

	if (ret == -1)
	{
		// not found in current version, check our stored version.
		const GHGLESShaderProgram::ReflectedUniformData* reflectedData = 0;
		getReflectedUniformData(uniformName, reflectedData);
		if (reflectedData)
		{
			// we know it exists in the default version but it is not currently active.
			// return a special gh code -2 to tell callbacks it exists but don't set the value yet.
			return -2;
		}
	}
	return ret;
}

bool GHGLESShaderProgram::hasDifferentVersionForQuality(void)
{
	for (int i = 0; i < 2; ++i)
	{
		GHGLESShader* qualityVersion = mAttachedShaders[i]->getVersionForQuality(mGraphicsQuality);
		if (qualityVersion)
		{
			if (qualityVersion != mAttachedShaders[i])
			{
				return true;
			}
		}
	}
	return false;
}

bool GHGLESShaderProgram::getExpectsDXLayout(void) const
{
	if (!mAttachedShaders[0]) return false;
	return mAttachedShaders[0]->getExpectsDXLayout();
}
