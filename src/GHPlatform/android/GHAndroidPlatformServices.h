// Copyright 2019 Golden Hammer Software
#pragma once

#include "GHPlatform/GHPlatformServices.h"
#include "GHAndroidFileOpener.h"
#include "GHPlatform/null/GHNullFilePicker.h"
#include "GHAndroidThreadFactory.h"
#include "GHAndroidTimeCalculator.h"
#include "GHAndroidSocketMgr.h"
#include "GHAndroidOutputPipe.h"
#include <vector>
#include "GHString/GHString.h"

#include <jni.h>

class GHAndroidPlatformServices : public GHPlatformServices
{
public:
	GHAndroidPlatformServices(GHJNIMgr& jniMgr, jobject engineInterface, std::vector<GHString>&& saveFileDirectories, jobject jAssetMgr);

	virtual GHFileOpener& getFileOpener(void) override { return mFileOpener; }
	virtual const GHFilePicker& getFilePicker(void) const override { return mFilePicker; }
	virtual const GHThreadFactory& getThreadFactory(void) const override { return mThreadFactory; }
	virtual const GHTimeCalculator& getTimeCalculator(void) const override { return mTimeCalculator; }
	virtual const GHSocketMgr& getSocketMgr(void) const override { return mSocketMgr; }

private:
	GHAndroidFileOpener mFileOpener;
	GHNullFilePicker mFilePicker;
	GHAndroidThreadFactory mThreadFactory;
	GHAndroidTimeCalculator mTimeCalculator;
	GHAndroidSocketMgr mSocketMgr;
	GHAndroidOutputPipe mOutputPipe;
};
