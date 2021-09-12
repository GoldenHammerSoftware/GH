// Copyright 2010 Golden Hammer Software
#include "GHOALErrorReporter.h"
#include "GHOALInclude.h"
#include "GHPlatform/GHDebugMessage.h"
#include <stdio.h>

static const char* getALErrorString(ALenum err)
{
    switch(err)
    {
        case AL_NO_ERROR:
            return "AL_NO_ERROR";
			break;
			
        case AL_INVALID_NAME:
            return "AL_INVALID_NAME";
			break;
			
        case AL_INVALID_ENUM:
            return "AL_INVALID_ENUM";
			break;
			
        case AL_INVALID_VALUE:
            return "AL_INVALID_VALUE";
			break;
			
        case AL_INVALID_OPERATION:
            return "AL_INVALID_OPERATION";
			break;
			
        case AL_OUT_OF_MEMORY:
            return "AL_OUT_OF_MEMORY";
			break;
    };
	return "";
}

void GHOALErrorReporter::checkAndReportError(const char* errInfo)
{
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		char failMsg[200];
		if (errInfo) ::sprintf(failMsg, "OpenAL Error: %s - %s\n", errInfo, getALErrorString(error));
		else ::sprintf(failMsg, "OpenAL Error: unknown caller - \n"); 
		GHDebugMessage::outputString(failMsg);
	}
}

