// Copyright 2019 Golden Hammer Software
#pragma once

#include "GHPlatform/GHPlatformServices.h"
#include "GHCocoaTimeCalculator.h"
#include "GHCocoaFileOpener.h"
#include "GHCocoaThreadFactory.h"
#include "GHPlatform/null/GHNullFilePicker.h"
#include "GHPlatform/null/GHNullSocketMgr.h"
#include "GHCocoaOutputPipe.h"

class GHCocoaPlatformServices : public GHPlatformServices
{
public:
    GHCocoaPlatformServices(void);
    
    virtual GHFileOpener& getFileOpener(void) { return mFileOpener; }
    virtual GHFilePicker& getFilePicker(void) { return mFilePicker; }
    virtual const GHThreadFactory& getThreadFactory(void) { return mThreadFactory; }
    virtual const GHTimeCalculator& getTimeCalculator(void) { return mTimeCalculator; }
    virtual GHSocketMgr& getSocketMgr(void) { return mSocketMgr; }
    
private:
    GHCocoaTimeCalculator mTimeCalculator;
    GHCocoaFileOpener mFileOpener;
    GHCocoaThreadFactory mThreadFactory;
    GHNullFilePicker mFilePicker;
    GHNullSocketMgr mSocketMgr;
    GHCocoaOutputPipe mOutputPipe;
};
