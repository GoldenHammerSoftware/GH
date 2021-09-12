// Copyright 2010 Golden Hammer Software
#include "GHMaterialOGLSHLoader.h"
#include "GHMDesc.h"
#include "GHMaterialOGLSH.h"
#include "GHDebugMessage.h"
#include "GHOGLInclude.h"
#include "GHOGLShaderProgram.h"
#include "GHVertexComponent.h"
#include "GHFile.h"
#include "GHFileOpener.h"
#include "GHMaterialCallbackMgr.h"
#include <assert.h>
#include "GHProfiler.h"
#include "GHXMLObjLoaderGHM.h"

GHMaterialOGLSHLoader::GHMaterialOGLSHLoader(const GHFileOpener& fileOpener,
                                             const GHMaterialCallbackMgr& callbackMgr,
                                             GHXMLObjLoaderGHM& descLoader)
: mDescLoader(descLoader)
, mFileOpener(fileOpener)
, mCallbackMgr(callbackMgr)
{
}

void* GHMaterialOGLSHLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHMDesc* desc = (GHMDesc*)mDescLoader.create(node, extraData);
    if (!desc) return 0;

    GHOGLShaderProgram* shader = loadShaderProgram(desc->mVertexFile, desc->mPixelFile);
    if (!shader)
    {
        GHDebugMessage::outputString("Failed to load shader %s %s", desc->mVertexFile.getChars(), desc->mPixelFile.getChars());
        delete desc;
        return 0;
    }
    GHMaterialOGLSH* ret = new GHMaterialOGLSH(shader, desc);
    mCallbackMgr.createCallbacks(*ret);
    return ret;
}

void GHMaterialOGLSHLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    assert(false);
}

static int loadShader(GLenum type, const char* fileName, const GHFileOpener& fileOpener)
{
	int ret = 0;
	
	GHFile* file = fileOpener.openFile(fileName, GHFile::FT_TEXT, GHFile::FM_READONLY);
	if (!file) {
		GHDebugMessage::outputString("Failed to find shader file\n");
		return ret;
	}
	
	size_t bufferSize = 0;
	char* bufferStart = 0;
	if (file->readIntoBuffer() && file->getFileBuffer(bufferStart, bufferSize)) 
	{
		const char* buffer = bufferStart;
		
		ret = glCreateShader(type);
		if (ret != 0)
		{
			glShaderSource(ret, 1, (const GLchar**)&buffer, NULL);
			glCompileShader(ret);
			
			int success;
			glGetShaderiv(ret, GL_COMPILE_STATUS, &success);
			if (success == 0)
			{
				char errorMsg[2048];
				glGetShaderInfoLog(ret, sizeof(errorMsg), NULL, errorMsg);
				GHDebugMessage::outputString("%s compile error: %s\n", fileName, errorMsg);
				glDeleteShader(ret);
				ret = 0;
			}
		}
	}
	
	file->closeFile();
	delete file;
	return ret;
}

GHOGLShaderProgram* GHMaterialOGLSHLoader::loadShaderProgram(const char* vertexStr, const char* pixelStr) const
{
    if (!vertexStr || !pixelStr) return 0;
    GHPROFILESCOPE("loadShaderProgram", GHString::CHT_REFERENCE)
   
	int vertShader = loadShader(GL_VERTEX_SHADER, vertexStr, mFileOpener);
	int pixelShader = loadShader(GL_FRAGMENT_SHADER, pixelStr, mFileOpener);
	if (!vertShader || !pixelShader) return 0;
	
	int retId = glCreateProgram();
	if (retId == 0) return 0;
	
	glAttachShader(retId, vertShader);
	glAttachShader(retId, pixelShader);
	
	glBindAttribLocation(retId, GHVertexComponentShaderID::SI_POS, "position");
	glBindAttribLocation(retId, GHVertexComponentShaderID::SI_NORMAL, "normal");
	glBindAttribLocation(retId, GHVertexComponentShaderID::SI_DIFFUSE, "diffuse");
	glBindAttribLocation(retId, GHVertexComponentShaderID::SI_SPECULAR, "specular");
	glBindAttribLocation(retId, GHVertexComponentShaderID::SI_UV1, "uv1");
	glBindAttribLocation(retId, GHVertexComponentShaderID::SI_UV2, "uv2");
    glBindAttribLocation(retId, GHVertexComponentShaderID::SI_BONEIDX, "boneidx");
    glBindAttribLocation(retId, GHVertexComponentShaderID::SI_BONEWEIGHT, "boneweight");
	
	glLinkProgram(retId);
	int linked;
	glGetProgramiv(retId, GL_LINK_STATUS, &linked);
	if (linked == 0) 
	{
		glDeleteProgram(retId);
		GHDebugMessage::outputString("Failed to link shader program.");
		return 0;
	}
	
	return new GHOGLShaderProgram(retId);
}
