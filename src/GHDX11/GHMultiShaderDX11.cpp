// Copyright Golden Hammer Software
#include "GHMultiShaderDX11.h"
#include <assert.h>

GHMultiShaderDX11::GHMultiShaderDX11(const int& testVal, GHShaderResource* defaultShader)
: mTestValue(testVal)
, mDefaultShader(0)
{
	GHRefCounted::changePointer((GHRefCounted*&)mDefaultShader, defaultShader);
	assert(mDefaultShader);
}

GHMultiShaderDX11::~GHMultiShaderDX11(void)
{
	GHRefCounted::changePointer((GHRefCounted*&)mDefaultShader, 0);
	std::map<int, GHShaderResource*>::iterator mapIter;
	for (mapIter = mShaders.begin(); mapIter != mShaders.end(); ++mapIter)
	{
		mapIter->second->release();
	}
}

void GHMultiShaderDX11::addShader(int value, GHShaderResource* shader)
{
	std::map<int, GHShaderResource*>::iterator mapIter;
	mapIter = mShaders.find(value);
	if (mapIter != mShaders.end()) {
		mapIter->second->release();
	}
	shader->acquire();
	mShaders[value] = shader;
}

void GHMultiShaderDX11::bind(void)
{
	std::map<int, GHShaderResource*>::iterator mapIter;
	mapIter = mShaders.find(mTestValue);
	if (mapIter != mShaders.end()) {
		mapIter->second->get()->bind();
	}
	else {
		mDefaultShader->get()->bind();
	}
}

void GHMultiShaderDX11::getBytecode(const char*& data, size_t& dataLen) const
{
	return mDefaultShader->get()->getBytecode(data, dataLen);
}

const GHShaderParamListDX11& GHMultiShaderDX11::getParamList(void) const
{
	return mDefaultShader->get()->getParamList();
}
