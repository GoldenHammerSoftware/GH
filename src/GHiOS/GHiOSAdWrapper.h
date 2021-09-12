// Copyright 2010 Golden Hammer Software
#pragma once

class GHEventMgr;
//class YrgOrientation;
class GHAdHandlingDelegate;
class GHTimeVal;

class GHiOSAdWrapper 
{
public:
    virtual ~GHiOSAdWrapper(void) { }
	virtual void initialize(GHAdHandlingDelegate* adHandlingDelegate, int adID) = 0;
    virtual void setOrientation(bool isPortrait) = 0;
	virtual void update(const GHTimeVal& timeVal) = 0;
	virtual void setActive(bool active) = 0;
};
