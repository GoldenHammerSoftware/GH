// Copyright 2019 Golden Hammer Software
#pragma once

#include "GHPlatform/GHPlatformServices.h"
#include "GHiOSFileOpener.h"
#include "GHPlatform/null/GHNullFilePicker.h"
#include "GHCFSocketMgr.h"
#include "GHiOSThreadFactory.h"
#include "GHiOSTimeCalculator.h"
#include "GHiOSOutputPipe.h"

class GHiOSPlatformServices : public GHPlatformServices
{
public:
    GHiOSPlatformServices(void);
    
    virtual GHFileOpener& getFileOpener(void) override { return mFileOpener; }
    virtual const GHFilePicker& getFilePicker(void) const override { return mFilePicker; }
    virtual const GHThreadFactory& getThreadFactory(void) const override { return mThreadFactory; }
    virtual const GHTimeCalculator& getTimeCalculator(void) const override { return mTimeCalculator; }
    virtual const GHSocketMgr& getSocketMgr(void) const override{ return mSocketMgr; }
    
private:
    GHiOSFileOpener mFileOpener;
    GHNullFilePicker mFilePicker;
    GHiOSThreadFactory mThreadFactory;
    GHiOSTimeCalculator mTimeCalculator;
    GHCFSocketMgr mSocketMgr;
    GHiOSOutputPipe mOutputPipe;
};
