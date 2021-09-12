// Copyright Golden Hammer Software
#pragma once

#include "GHShaderDX11.h"
#include <map>

// A class to wrap several shaders and treat them as the same from outside
// We switch which one gets applied based on a value that we reference
class GHMultiShaderDX11 : public GHShaderDX11
{
public:
	GHMultiShaderDX11(const int& testVal, GHShaderResource* defaultShader);
	~GHMultiShaderDX11(void);

	void addShader(int value, GHShaderResource* shader);

	void bind(void);
	void getBytecode(const char*& data, size_t& dataLen) const;
	const GHShaderParamListDX11& getParamList(void) const;

private:
	// The shader we use if we don't have one for the specified value.
	// This is also the shader we use for paramlist and bytecode.
	GHShaderResource* mDefaultShader;
	// The value we reference to decide which shader to use.
	const int& mTestValue;
	// The set of override shaders for when mTestValue is different.
	std::map<int, GHShaderResource*> mShaders;
};
