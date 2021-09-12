#pragma once

class GHiOSInitCallback
{
public:
    virtual ~GHiOSInitCallback(void) { }
    virtual void onInitSuccess(void) = 0;
};
