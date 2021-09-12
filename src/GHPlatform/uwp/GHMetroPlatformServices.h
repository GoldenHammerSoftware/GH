// Copyright 2019 Golden Hammer Software
#pragma once

#include "GHPlatform/GHPlatformServices.h"

#include "GHMetroFileOpener.h"
#include "GHMetroFilePicker.h"
#include "GHMetroThreadFactory.h"
#include "GHMetroTimeCalculator.h"
#include "GHMetroSocketMgr.h"
#include "GHMetroOutputPipe.h"

class GHMetroPlatformServices : public GHPlatformServices
{
public:
	GHMetroPlatformServices(void);
	~GHMetroPlatformServices(void);

	virtual GHFileOpener& getFileOpener(void) override { return mFileOpener; }
	virtual const GHFilePicker& getFilePicker(void) const override { return mFilePicker; }
	virtual const GHThreadFactory& getThreadFactory(void) const override { return mThreadFactory; }
	virtual const GHTimeCalculator& getTimeCalculator(void) const override { return mTimeCalculator; }
	virtual const GHSocketMgr& getSocketMgr(void) const override { return mSocketMgr; }

private:
	GHMetroFileOpener mFileOpener;
	GHMetroFilePicker mFilePicker;
	GHMetroThreadFactory mThreadFactory;
	GHMetroTimeCalculator mTimeCalculator;
	GHMetroSocketMgr mSocketMgr;
	GHMetroOutputPipe mOutputPipe;

};
