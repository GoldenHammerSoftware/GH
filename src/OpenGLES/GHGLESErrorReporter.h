// Copyright 2010 Golden Hammer Software
#pragma once

class GHGLESErrorReporter
{
public:
    static bool checkAndReportError(const char* errInfo = 0);
    static void setErrorReportingEnabled(bool enable);
};
