#pragma once

#include "GHShaderType.h"

class GHFileOpener;

class GHSpirvCross
{
public:
	void compileShaderToMSL(const GHFileOpener& fileOpener, const char* hlslName, GHShaderType shaderType);
	void compileShaderToGLES(const GHFileOpener& fileOpener, const char* hlslName, GHShaderType shaderType);
};