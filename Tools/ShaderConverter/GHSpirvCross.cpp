#include "GHSpirvCross.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHPlatform/GHFile.h"
#include "spirv_msl.hpp"
#include "spirv_glsl.hpp"
#include "GHMetal/GHMetalShaderStructs.h"
#include "GHMetal/GHMetalNameSanitizer.h"

static bool openSourceSpirvFile(const GHFileOpener& fileOpener, const char* hlslName,
    char*& outBuf, size_t& outBufSize)
{
    bool ret = true;
    char srcName[2048];
    snprintf(srcName, 2048, "shader/spirvout/%s", hlslName);
    size_t nameLen = strlen(srcName);
    srcName[nameLen - 4] = 's';
    srcName[nameLen - 3] = 'p';
    srcName[nameLen - 2] = 'v';
    srcName[nameLen - 1] = '\0';

    GHFile* file = fileOpener.openFile(srcName, GHFile::FT_BINARY, GHFile::FM_READONLY);
    if (!file)
    {
        GHDebugMessage::outputString("Failed to open spirvout file for spirvcross");
        return false;
    }
    file->readIntoBuffer();
    if (!file->getFileBuffer(outBuf, outBufSize))
    {
        GHDebugMessage::outputString("Failed to get file buffer for spirv in");
        ret = false;
    }
    file->releaseFileBuffer();
    delete file;
    return ret;
}

static void replaceVertexAttributesMSL(GHString& outStr)
{
    // search string for each attribute index.
    // this needs to match GHMTL_AttributeIndex
    const char* bindingKey[] =
    {
        "in_var_POSITION [[attribute(",
        "in_var_NORMAL [[attribute(",
        "in_var_TANGENT [[attribute(",
        "in_var_DIFFUSE [[attribute(",
        "in_var_SPECULAR [[attribute(",
        "in_var_BONEIDX [[attribute(",
        "in_var_UV [[attribute(",
        "in_var_UVTWO [[attribute(",

    };
    // the string we replace the original binding with.
    const char* bindingDest[] =
    {
        "GHMTL_AI_POS",
        "GHMTL_AI_NORMAL",
        "GHMTL_AI_TANGENT",
        "GHMTL_AI_DIFFUSE",
        "GHMTL_AI_SPECULAR",
        "GHMTL_AI_BONEIDX",
        "GHMTL_AI_UV0",
        "GHMTL_AI_UV1",
    };
    size_t numAttr = sizeof(bindingKey) / sizeof(bindingKey[0]);
    char bindingBuf[32];
    for (size_t i = 0; i < numAttr; ++i)
    {
        char* searchStart = outStr.getNonConstChars();
        char* attrStart = strstr(searchStart, bindingKey[i]);
        if (!attrStart) continue;
        // found an attr, find the start of the binding.
        char* bindingStart = attrStart + strlen(bindingKey[i]);
        const char* bindingEnd = strstr(bindingStart, ")");
        snprintf(bindingBuf, (bindingEnd - bindingStart) + 1, "%s", bindingStart);
        outStr.replace(bindingBuf, bindingDest[i], bindingStart);
    }
}

static void replaceBindingsMSL(GHString& outStr, GHShaderType shaderType)
{
    const char* bindingKey[] =
    {
        "type_PerFrameBuffer& PerFrameBuffer [[buffer(",
        "type_PerTransBuffer& PerTransBuffer [[buffer(",
        "type_PerGeoBuffer& PerGeoBuffer [[buffer(",
        "type_PerEntBuffer& PerEntBuffer [[buffer(",
    };
    const char* bindingDest[] =
    {
        "PERFRAME",
        "PERTRANS",
        "PERGEO",
        "PERENT",
    };
    size_t numAttr = sizeof(bindingKey) / sizeof(bindingKey[0]);
    char bindingBuf[32];
    char bindingReplace[256];
    for (size_t i = 0; i < numAttr; ++i)
    {
        char* searchStart = outStr.getNonConstChars();
        char* attrStart = strstr(searchStart, bindingKey[i]);
        if (!attrStart) continue;
        // found an attr, find the start of the binding.
        char* bindingStart = attrStart + strlen(bindingKey[i]);
        const char* bindingEnd = strstr(bindingStart, ")");
        snprintf(bindingBuf, (bindingEnd - bindingStart) + 1, "%s", bindingStart);
        if (shaderType == ST_VERTEX)
        {
            sprintf(bindingReplace, "GHMTL_BI_V%s", bindingDest[i]);
        }
        else
        {
            sprintf(bindingReplace, "GHMTL_BI_P%s", bindingDest[i]);
        }
        outStr.replace(bindingBuf, bindingReplace, bindingStart);
    }
}

void GHSpirvCross::compileShaderToMSL(const GHFileOpener& fileOpener, const char* hlslName, GHShaderType shaderType)
{
    char* rawInBuffer;
    size_t inBufferSize;
    if (!openSourceSpirvFile(fileOpener, hlslName, rawInBuffer, inBufferSize))
    {
        return;
    }

    spirv_cross::CompilerMSL mslCompiler((const uint32_t*)rawInBuffer, inBufferSize / 4);
    spirv_cross::CompilerMSL::Options options;
    options.platform = spirv_cross::CompilerMSL::Options::iOS;
    options.msl_version = spirv_cross::CompilerMSL::Options::make_msl_version(2, 0);

    mslCompiler.set_msl_options(options);
    std::string compiledString = mslCompiler.compile();

    GHString outStr(compiledString.c_str(), GHString::CHT_COPY);
    if (shaderType == GHShaderType::ST_VERTEX)
    {
        replaceVertexAttributesMSL(outStr);
    }
    // replace the generated "main0" with the shader name so they can coexist in the same mtl lib.
    char strippedName[512];
    GHMetalNameSanitizer::sanitizeName(hlslName, strippedName);
    outStr.replaceAll("main0", strippedName);
    replaceBindingsMSL(outStr, shaderType);

    // save the compiled msl file to disk.
    char mslName[1024];
    sprintf(mslName, "shader/mslout/%s", hlslName);
    size_t nameLen = strlen(mslName);
    mslName[nameLen - 4] = 'm';
    mslName[nameLen - 3] = 'e';
    mslName[nameLen - 2] = 't';
    mslName[nameLen - 1] = 'a';
    mslName[nameLen + 0] = 'l';
    mslName[nameLen + 1] = '\0';

    GHFile* outFile = fileOpener.openFile(mslName, GHFile::FT_TEXT, GHFile::FM_WRITEONLY);
    if (outFile)
    {
        const char* additionalHeader = "#include \"GHMetal/GHMetalShaderStructs.h\"\n";
        outFile->writeBuffer(additionalHeader, strlen(additionalHeader));
        outFile->writeBuffer(outStr.getChars(), outStr.getCharLen());
    }
    outFile->closeFile();
    delete outFile;

    delete[] rawInBuffer;
}

static void replaceVertexAttributesGLSL(GHString& outStr)
{
    // Vertex attributes have to match GHGLESShaderProgram::bindVertexAttributes()
    if (strstr(outStr.getChars(), "attribute vec3 in_var_POSITION") || 
        strstr(outStr.getChars(), "attribute vec4 in_var_POSITION") ||
        strstr(outStr.getChars(), "in vec3 in_var_POSITION") ||
        strstr(outStr.getChars(), "in vec4 in_var_POSITION"))
    {
        outStr.replaceAll("in_var_POSITION", "position");
    }
    if (strstr(outStr.getChars(), "attribute vec3 in_var_NORMAL") ||
        strstr(outStr.getChars(), "in vec3 in_var_NORMAL"))
    {
        outStr.replaceAll("in_var_NORMAL", "normal");
    }
    if (strstr(outStr.getChars(), "attribute vec3 in_var_TANGENT") ||
        strstr(outStr.getChars(), "in vec3 in_var_TANGENT"))
    {
        outStr.replaceAll("in_var_TANGENT", "tangent");
    }
    if (strstr(outStr.getChars(), "attribute vec4 in_var_DIFFUSE") ||
        strstr(outStr.getChars(), "in vec4 in_var_DIFFUSE"))
    {
        outStr.replaceAll("in_var_DIFFUSE", "diffuse");
    }
    if (strstr(outStr.getChars(), "attribute vec4 in_var_SPECULAR") ||
        strstr(outStr.getChars(), "in vec4 in_var_SPECULAR"))
    {
        outStr.replaceAll("in_var_SPECULAR", "specular");
    }
    if (strstr(outStr.getChars(), "attribute vec2 in_var_UVTWO") ||
        strstr(outStr.getChars(), "in vec2 in_var_UVTWO"))
    {
        outStr.replaceAll("in_var_UVTWO", "uv2");
    }
    if (strstr(outStr.getChars(), "attribute vec2 in_var_UV") ||
        strstr(outStr.getChars(), "in vec2 in_var_UV"))
    {
        outStr.replaceAll("in_var_UV", "uv1");
    }
    // find examples of these
    //glBindAttribLocation(shaderId, GHVertexComponentShaderID::SI_BONEIDX, "boneIndices");
    //glBindAttribLocation(shaderId, GHVertexComponentShaderID::SI_BONEWEIGHT, "boneWeights");
}

void GHSpirvCross::compileShaderToGLES(const GHFileOpener& fileOpener, const char* hlslName, GHShaderType shaderType)
{
    char* rawInBuffer;
    size_t inBufferSize;
    if (!openSourceSpirvFile(fileOpener, hlslName, rawInBuffer, inBufferSize))
    {
        return;
    }

    spirv_cross::CompilerGLSL glslCompiler((const uint32_t*)rawInBuffer, inBufferSize / 4);
    glslCompiler.build_combined_image_samplers();

    // The SPIR-V is now parsed, and we can perform reflection on it.
    spirv_cross::ShaderResources resources = glslCompiler.get_shader_resources();

    // rename the combined image samplers so we can find them at runtime.
    auto combSamplers = glslCompiler.get_combined_image_samplers();
    for (size_t combId = 0; combId < combSamplers.size(); ++combId)
    {
        auto combInf = combSamplers[combId];
        for (auto& resource : resources.separate_images)
        {
            if (resource.id == combInf.image_id)
            {
                glslCompiler.set_name(combInf.combined_id, resource.name);
                break;
            }
        }
    }

    spirv_cross::CompilerGLSL::Options options;
    options.es = true;
    options.emit_uniform_buffer_as_plain_uniforms = true;
    options.force_flattened_io_blocks = true;
    options.version = 300;

    glslCompiler.set_common_options(options);
    std::string compiledString = glslCompiler.compile();

    // couldn't figure out how to rename some stuff pre-compile, so do it as a string replace after.
    // copying into a ghstring is expensive but we don't really care here.
    // could make the ghstring replace functions static and take a char* instead if we want.
    GHString outStr(compiledString.c_str(), GHString::CHT_COPY);
    if (shaderType == GHShaderType::ST_VERTEX)
    {
        // rename our vertex attributes to match what the engine is expecting.
        replaceVertexAttributesGLSL(outStr);
        // avoid collisions with the pixel shader types.
        outStr.replaceAll("PerFrameBuffer", "Vertex_PerFrameBuffer");
        outStr.replaceAll("PerTransBuffer", "Vertex_PerTransBuffer");
        outStr.replaceAll("PerEntBuffer", "Vertex_PerEntBuffer");
        outStr.replaceAll("PerGeoBuffer", "Vertex_PerGeoBuffer");
    }
    else
    {
        outStr.replaceAll("PerFrameBuffer", "Pixel_PerFrameBuffer");
        outStr.replaceAll("PerTransBuffer", "Pixel_PerTransBuffer");
        outStr.replaceAll("PerEntBuffer", "Pixel_PerEntBuffer");
        outStr.replaceAll("PerGeoBuffer", "Pixel_PerGeoBuffer");
    }
    outStr.replaceAll("in_var_", "");
    outStr.replaceAll("out_var_", "");
    outStr.replaceAll("layout(location = 0) ", "");
    outStr.replaceAll("layout(location = 1) ", "");
    outStr.replaceAll("layout(location = 2) ", "");
    outStr.replaceAll("layout(location = 3) ", "");
    outStr.replaceAll("layout(location = 4) ", "");
    outStr.replaceAll("layout(location = 5) ", "");
    outStr.replaceAll("layout(location = 6) ", "");
    outStr.replaceAll("layout(location = 7) ", "");
    outStr.replaceAll("layout(location = 8) ", "");
    outStr.replaceAll("layout(location = 9) ", "");

    // save the compiled msl file to disk.
    char mslName[1024];
    sprintf(mslName, "shader/glslout/%s", hlslName);
    size_t nameLen = strlen(mslName);
    mslName[nameLen - 4] = 'g';
    mslName[nameLen - 3] = 'l';
    mslName[nameLen - 2] = 's';
    mslName[nameLen - 1] = 'l';
    mslName[nameLen] = '\0';

    char* outBuf = outStr.getNonConstChars();

    GHFile* outFile = fileOpener.openFile(mslName, GHFile::FT_TEXT, GHFile::FM_WRITEONLY);
    if (outFile)
    {
        const char* shaderStr = outBuf;
        size_t shaderLen = strlen(shaderStr);
        outFile->writeBuffer(shaderStr, shaderLen);
    }
    outFile->closeFile();
    delete outFile;

    delete[] rawInBuffer;
}
