#pragma once

#include "GHShaderType.h"

// This makes the shader converter windows-only at least for now.
#include "GHPlatform/win32/GHWin32Include.h"
// adding contrib to include path and including dxc/inc/ directly to avoid conflicts with the windows kit.
#include "dxc/inc/dxcapi.h"
#include "dxc/inc/d3d12shader.h"
#include <atlbase.h> 

class GHFileOpener;
class GHXMLSerializer;

class GHDXC
{
public:
	GHDXC(void);
	~GHDXC(void);

	void compileShaderToSpirv(const GHFileOpener& fileOpener, const GHXMLSerializer& xmlSerializer,
		const char* hlslName, GHShaderType shaderType);

private:
	void createReflection(CComPtr<IDxcBlobEncoding> inBlob, const GHXMLSerializer& xmlSerializer, const char* hlslName, GHShaderType shaderType);
	void saveHLSLReflection(CComPtr< ID3D12ShaderReflection > pReflection, const GHXMLSerializer& xmlSerializer, const char* hlslName);

private:
	CComPtr<IDxcUtils> mDXCUtils;
	CComPtr<IDxcCompiler3> mDXCCompiler;
};