// Copyright 2010 Golden Hammer Software
#pragma once

// A widget to check for and print out any OGL errors
class GHGLErrorReporter
{
public:
	static bool checkAndReportError(const char* errInfo = 0);
};
