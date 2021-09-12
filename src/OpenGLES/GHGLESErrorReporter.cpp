// Copyright 2010 Golden Hammer Software
#include "GHGLESErrorReporter.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGLESInclude.h"
#include <stdio.h>

static bool sErrorReportingEnabled = true;
void GHGLESErrorReporter::setErrorReportingEnabled(bool enable)
{
    sErrorReportingEnabled = enable;
}

bool GHGLESErrorReporter::checkAndReportError(const char* errInfo)
{
	bool hitAnError = false;
	
	// the docs say glGetError is expensive.  disable for retail
#if 1
    if (!sErrorReportingEnabled) { return hitAnError; }
    
	GLenum err = GL_INVALID_VALUE;
	while (err != GL_NO_ERROR) {
		err = glGetError();
		if (err != GL_NO_ERROR) {
			char failMsg[200];
			if (errInfo) ::sprintf(failMsg, "OpenGL Error: %s - ", errInfo);
			else ::sprintf(failMsg, "OpenGL Error: unknown caller - "); 
			GHDebugMessage::outputString(failMsg);
			
			if (err == GL_INVALID_ENUM) {
				GHDebugMessage::outputString("GL_INVALID_ENUM");	
			}
			else if (err == GL_INVALID_VALUE) {
				GHDebugMessage::outputString("GL_INVALID_VALUE");	
			}
			else if (err == GL_INVALID_OPERATION) {
				GHDebugMessage::outputString("GL_INVALID_OPERATION");	
			}
			else if (err == GL_OUT_OF_MEMORY) {
				GHDebugMessage::outputString("GL_OUT_OF_MEMORY");	
			}
            else if (err == GL_INVALID_FRAMEBUFFER_OPERATION) {
                GHDebugMessage::outputString("GL_INVALID_FRAMEBUFFER_OPERATION");
            }
			else {
				GHDebugMessage::outputString("UNKNOWN ERROR: 0x%x", err);
			}
			hitAnError = true;
		}
	}
#endif	
	return !hitAnError;
}
