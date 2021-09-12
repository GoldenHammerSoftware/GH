#include "GHDXC.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHXMLNode.h"
#include "GHXMLSerializer.h"
#include "GHMaterialCallbackType.h"

GHDXC::GHDXC(void)
{
	DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&mDXCUtils));
	DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&mDXCCompiler));
}

GHDXC::~GHDXC(void)
{
}

// https://github.com/microsoft/DirectXShaderCompiler/wiki/Using-dxc.exe-and-dxcompiler.dll
void GHDXC::compileShaderToSpirv(const GHFileOpener& fileOpener, const GHXMLSerializer& xmlSerializer,
    const char* hlslName, GHShaderType shaderType)
{
    char hlslSrcName[2048];
    snprintf(hlslSrcName, 2048, "shader/hlslout/%s", hlslName);

    GHFile* outFile = fileOpener.openFile(hlslSrcName, GHFile::FT_TEXT, GHFile::FM_READONLY);
    if (!outFile)
    {
        GHDebugMessage::outputString("Failed to open hlslout file for dxc");
        return;
    }
    outFile->readIntoBuffer();
    char* rawInBuffer;
    size_t inBufferSize;
    if (!outFile->getFileBuffer(rawInBuffer, inBufferSize))
    {
        GHDebugMessage::outputString("Failed to get file buffer for dxc in");
    }
    outFile->releaseFileBuffer();
    delete outFile;

    CComPtr<IDxcBlobEncoding> inBlob = nullptr;
    HRESULT blobResult = mDXCUtils->CreateBlob(rawInBuffer, inBufferSize, 0, &inBlob);
    if (blobResult != S_OK)
    {
        GHDebugMessage::outputString("Failed to create dxc blob");
        return;
    }

    createReflection(inBlob, xmlSerializer, hlslName, shaderType);

    const wchar_t* shaderTarget = (shaderType == GHShaderType::ST_PIXEL) ? L"ps_6_0" : L"vs_6_0";
    LPCWSTR pszArgs[] =
    {
        L"myshader.hlsl",            // Optional shader source file name for error reporting and for PIX shader source view.  
        L"-E", L"main",              // Entry point.
        L"-T", shaderTarget,         // Target.
        L"-Zi",                      // Enable debug information.
        L"-Qstrip_reflect",          // Strip reflection into a separate blob. 
        L"-spirv",
        L"-Zpr"                      // row/column major
    };

    DxcBuffer hlslSource;
    hlslSource.Ptr = inBlob->GetBufferPointer();
    hlslSource.Size = inBlob->GetBufferSize();
    hlslSource.Encoding = DXC_CP_ACP; // Assume BOM says UTF8 or UTF16 or this is ANSI text.

    CComPtr<IDxcResult> pDXCResults;
    mDXCCompiler->Compile(
        &hlslSource,            // Source buffer.
        pszArgs,                // Array of pointers to arguments.
        _countof(pszArgs),      // Number of arguments.
        nullptr,        // User-provided interface to handle #include directives (optional).
        IID_PPV_ARGS(&pDXCResults) // Compiler output status, buffer, and errors.
    );

    CComPtr<IDxcBlobUtf8> pErrors = nullptr;
    pDXCResults->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&pErrors), nullptr);
    // Note that d3dcompiler would return null if no errors or warnings are present.  
    // IDxcCompiler3::Compile will always return an error buffer, but its length will be zero if there are no warnings or errors.
    if (pErrors != nullptr && pErrors->GetStringLength() != 0)
    {
        wprintf(L"Warnings and Errors:\n%S\n", pErrors->GetStringPointer());
    }

    HRESULT hrStatus;
    pDXCResults->GetStatus(&hrStatus);
    if (FAILED(hrStatus))
    {
        GHDebugMessage::outputString("Failed to dxc compile file %s", hlslName);
        return;
    }

    CComPtr<IDxcBlob> pShader = nullptr;
    CComPtr<IDxcBlobUtf16> pShaderName = nullptr;
    pDXCResults->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pShader), &pShaderName);
    if (pShader != nullptr)
    {
        char spirvName[1024];
        sprintf(spirvName, "shader/spirvout/%s", hlslName);
        size_t nameLen = strlen(spirvName);
        spirvName[nameLen - 4] = 's';
        spirvName[nameLen - 3] = 'p';
        spirvName[nameLen - 2] = 'v';
        spirvName[nameLen - 1] = '\0';

        GHFile* outFile = fileOpener.openFile(spirvName, GHFile::FT_BINARY, GHFile::FM_WRITEONLY);
        if (outFile)
        {
            outFile->writeBuffer(pShader->GetBufferPointer(), pShader->GetBufferSize());
        }
        outFile->closeFile();
        delete outFile;
    }

    delete[] rawInBuffer;
}

void GHDXC::createReflection(CComPtr<IDxcBlobEncoding> inBlob, const GHXMLSerializer& xmlSerializer, const char* hlslName, GHShaderType shaderType)
{
    // we have to compile for hlsl to get reflection info.

    const wchar_t* shaderTarget = (shaderType == GHShaderType::ST_PIXEL) ? L"ps_6_0" : L"vs_6_0";
    LPCWSTR pszArgs[] =
    {
        L"myshader.hlsl",            // Optional shader source file name for error reporting and for PIX shader source view.  
        L"-E", L"main",              // Entry point.
        L"-T", shaderTarget,         // Target.
        L"-Zi",                      // Enable debug information.
        L"-Qstrip_reflect",          // Strip reflection into a separate blob. 
    };

    DxcBuffer hlslSource;
    hlslSource.Ptr = inBlob->GetBufferPointer();
    hlslSource.Size = inBlob->GetBufferSize();
    hlslSource.Encoding = DXC_CP_ACP; // Assume BOM says UTF8 or UTF16 or this is ANSI text.

    CComPtr<IDxcResult> pDXCResults;
    mDXCCompiler->Compile(
        &hlslSource,            // Source buffer.
        pszArgs,                // Array of pointers to arguments.
        _countof(pszArgs),      // Number of arguments.
        nullptr,        // User-provided interface to handle #include directives (optional).
        IID_PPV_ARGS(&pDXCResults) // Compiler output status, buffer, and errors.
    );

    CComPtr<IDxcBlobUtf8> pErrors = nullptr;
    pDXCResults->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&pErrors), nullptr);
    // Note that d3dcompiler would return null if no errors or warnings are present.  
    // IDxcCompiler3::Compile will always return an error buffer, but its length will be zero if there are no warnings or errors.
    if (pErrors != nullptr && pErrors->GetStringLength() != 0)
    {
        wprintf(L"Warnings and Errors:\n%S\n", pErrors->GetStringPointer());
    }

    HRESULT hrStatus;
    pDXCResults->GetStatus(&hrStatus);
    if (FAILED(hrStatus))
    {
        GHDebugMessage::outputString("Failed to dxc compile file %s", hlslName);
        return;
    }

    CComPtr<IDxcBlob> pReflectionData;
    pDXCResults->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&pReflectionData), nullptr);
    if (pReflectionData != nullptr)
    {
        // Create reflection interface.
        DxcBuffer ReflectionData;
        ReflectionData.Encoding = DXC_CP_ACP;
        ReflectionData.Ptr = pReflectionData->GetBufferPointer();
        ReflectionData.Size = pReflectionData->GetBufferSize();

        CComPtr< ID3D12ShaderReflection > pReflection;
        mDXCUtils->CreateReflection(&ReflectionData, IID_PPV_ARGS(&pReflection));

        saveHLSLReflection(pReflection, xmlSerializer, hlslName);
    }
}

void GHDXC::saveHLSLReflection(CComPtr<ID3D12ShaderReflection> pReflection, const GHXMLSerializer& xmlSerializer, const char* hlslName)
{
    GHXMLNode outNode;
    outNode.setName("shaderParams", GHString::CHT_REFERENCE);
    GHXMLNode* texturesNode = new GHXMLNode();
    texturesNode->setName("textureParams", GHString::CHT_REFERENCE);
    outNode.addChild(texturesNode);
    GHXMLNode* floatsNode = new GHXMLNode();
    floatsNode->setName("floatParams", GHString::CHT_REFERENCE);
    outNode.addChild(floatsNode);

    D3D12_SHADER_DESC desc;
    HRESULT descRes = pReflection->GetDesc(&desc);
    if (descRes != S_OK)
    {
        GHDebugMessage::outputString("Failed to read reflection");
    }

    /*// This reads the input to the shader.  vsOut if you are in the pixel shader.
    for (size_t paramIdx = 0; paramIdx < desc.InputParameters; ++paramIdx)
    {
        D3D12_SIGNATURE_PARAMETER_DESC paramDesc;
        pReflection->GetInputParameterDesc(paramIdx, &paramDesc);
        int brkpt = 1;
    }*/

    char valBuf[1024];
    std::vector<GHMaterialCallbackType::Enum> bufferBindTypes;
    for (size_t resIdx = 0; resIdx < desc.BoundResources; ++resIdx)
    {
        D3D12_SHADER_INPUT_BIND_DESC resDesc;
        pReflection->GetResourceBindingDesc(resIdx, &resDesc);

        if (resDesc.Type == D3D_SIT_CBUFFER)
        {
            bufferBindTypes.push_back((GHMaterialCallbackType::Enum)resDesc.BindPoint);
        }
        else if (resDesc.Type == D3D_SIT_TEXTURE)
        {
            GHXMLNode* texNode = new GHXMLNode();
            texNode->setName("texParam", GHString::CHT_REFERENCE);
            texturesNode->addChild(texNode);
            texNode->setAttribute("semantic", GHString::CHT_REFERENCE, resDesc.Name, GHString::CHT_COPY);
            sprintf(valBuf, "%d", resDesc.BindPoint);
            texNode->setAttribute("register", GHString::CHT_REFERENCE, valBuf, GHString::CHT_COPY);
            texNode->setAttribute("cbType", GHString::CHT_REFERENCE, "CT_PERFRAME", GHString::CHT_REFERENCE);
        }
        //else if (resDesc.Type == D3D_SIT_SAMPLER)
    }

    // buffers
    for (size_t bufferIdx = 0; bufferIdx < desc.ConstantBuffers; ++bufferIdx)
    {
        ID3D12ShaderReflectionConstantBuffer* bufferRfl = pReflection->GetConstantBufferByIndex(bufferIdx);
        D3D12_SHADER_BUFFER_DESC bufferDesc;
        HRESULT bufDescRes = bufferRfl->GetDesc(&bufferDesc);
        for (size_t varIdx = 0; varIdx < bufferDesc.Variables; ++varIdx)
        {
            ID3D12ShaderReflectionVariable* varRfl = bufferRfl->GetVariableByIndex(varIdx);
            D3D12_SHADER_VARIABLE_DESC varDesc;
            varRfl->GetDesc(&varDesc);

            //  <floatParam sizeFloats="1" cbType="CT_PERFRAME" semantic="LightMapScale"/>
            GHXMLNode* floatNode = new GHXMLNode();
            floatNode->setName("floatParam", GHString::CHT_COPY);
            floatsNode->addChild(floatNode);

            floatNode->setAttribute("semantic", GHString::CHT_REFERENCE, varDesc.Name, GHString::CHT_COPY);
            sprintf(valBuf, "%d", varDesc.Size / 4);
            floatNode->setAttribute("sizeFloats", GHString::CHT_REFERENCE, valBuf, GHString::CHT_COPY);

            const char* cbType = "CT_PERENT";
            if (bufferBindTypes[bufferIdx] == GHMaterialCallbackType::CT_PERFRAME)
            {
                cbType = "CT_PERFRAME";
            }
            else if(bufferBindTypes[bufferIdx] == GHMaterialCallbackType::CT_PERGEO)
            {
                cbType = "CT_PERGEO";
            }
            else if (bufferBindTypes[bufferIdx] == GHMaterialCallbackType::CT_PERTRANS)
            {
                cbType = "CT_PERTRANS";
            }
            floatNode->setAttribute("cbType", GHString::CHT_REFERENCE, cbType, GHString::CHT_COPY);
        }
    }

    char xmlName[1024];
    sprintf(xmlName, "shader/reflectionout/%s", hlslName);
    size_t nameLen = strlen(xmlName);
    xmlName[nameLen - 5] = '\0';
    sprintf(xmlName, "%sparams.xml", xmlName);
    xmlSerializer.writeXMLFile(xmlName, outNode);
}
