// Copyright 2010 Golden Hammer Software
#include "GHAndroidOutputPipe.h"
#include <android/log.h>
#include <stdio.h>
#include <string.h>

void GHAndroidOutputPipe::outputString(const char* str)
{
#ifndef GHRETAIL
	int32_t len = (int32_t)strlen(str);
	const size_t MAX_ANDROID_LOG_LENGTH = 1000;
	if (len < MAX_ANDROID_LOG_LENGTH)
	{
		__android_log_print(ANDROID_LOG_INFO, "GHNative", "%s", str);
	}
	else
	{
		// need to chop up the message.
		char outputBuf[MAX_ANDROID_LOG_LENGTH + 1];
		const char* currPos = str;
		while (len > 0)
		{
			snprintf(outputBuf, MAX_ANDROID_LOG_LENGTH, "%s", currPos);
			len -= MAX_ANDROID_LOG_LENGTH;
			currPos += MAX_ANDROID_LOG_LENGTH-1;
			outputBuf[MAX_ANDROID_LOG_LENGTH] = 0;
			__android_log_print(ANDROID_LOG_INFO, "GHNative", "%s", outputBuf);
		}
	}
#endif
}

