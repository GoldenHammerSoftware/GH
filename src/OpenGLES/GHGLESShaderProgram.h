// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHRefCounted.h"
#include <vector>
#include <map>

class GHGLESShader;
class GHGLESShaderMgr;
class GHStringIdFactory;
class GHIdentifier;

// A class to wrap a handle to an OGL pixel/vertex shader pair
class GHGLESShaderProgram : public GHRefCounted
{
public:
    GHGLESShaderProgram(unsigned int program, GHGLESShader* vertShader, GHGLESShader* pixelShader, 
		GHGLESShaderMgr& shaderMgr, const GHStringIdFactory& stringHasher, const int& graphicsQuality);
    
    unsigned int getShaderProgram(void) const { return mShaderProgram; }
    
    void reloadShaders(void);
	void handleGraphicsQualityChanged(void);

    static bool linkShaders(unsigned int& retId, GHGLESShader* vertShader, GHGLESShader* pixelShader);

	// find the uniform size in the currently linked shader program.
	// default to 1.
	int getUniformSize(const char* uniformName);
	// wrapper for glGetUniformLocation.  returns -1 if not found.
	int getUniformLocation(const char* uniformName);

	bool getExpectsDXLayout(void) const;

private:
	struct ReflectedUniformData
	{
		int mSize{ 1 };

		ReflectedUniformData(void) = default;
		ReflectedUniformData(int size)
			: mSize(size)
		{}
	};

	void cacheReflectedUniformData(void);
	void getReflectedUniformData(const char* uniformName, const ReflectedUniformData*& outData) const;
	void getReflectedUniformData(const GHIdentifier& uniformNameHash, const ReflectedUniformData*& outData) const;
	bool hasDifferentVersionForQuality(void);

protected:
    ~GHGLESShaderProgram(void);

private:
    unsigned int mShaderProgram;
    std::vector<GHGLESShader*> mAttachedShaders;
	std::map<GHIdentifier, ReflectedUniformData> mReflectedUniformData;
    GHGLESShaderMgr& mShaderMgr;
	const GHStringIdFactory& mStringHasher;
	const int& mGraphicsQuality;
};
