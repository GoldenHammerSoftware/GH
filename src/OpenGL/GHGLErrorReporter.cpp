// Copyright 2010 Golden Hammer Software
#include "GHGLErrorReporter.h"
#include "GHDebugMessage.h"
#include "GHOGLInclude.h"

bool GHGLErrorReporter::checkAndReportError(const char* errInfo)
{

	GLenum err = GL_INVALID_VALUE;
    int numErrors = 0;
	
    // don't infinite loop if we do something like call with no context.
    while (err != GL_NO_ERROR && numErrors < 5) 
    {
		err = glGetError();
		if (err != GL_NO_ERROR) {
			//GHDebugMessage::outputString("OpenGL Error: %s: %s", errInfo,
            //                             (const char*)gluErrorString(err));
			numErrors++;
		}
	}
	return numErrors > 0;
}
