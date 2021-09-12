#pragma once

#include "GHPlatform/GHPlatformServices.h"
#include "GHPlatform/GHFOpenFileOpener.h"
#include "GHWin32TimeCalculator.h"
#include "GHWin32ThreadFactory.h"
#include "GHWin32FilePicker.h"
#include "GHWinSockSocketMgr.h"
#include "GHWin32OutputPipe.h"

class GHWin32PlatformServices : public GHPlatformServices
{
public:
	GHWin32PlatformServices(HWND hwnd);
	~GHWin32PlatformServices(void);

	virtual GHFileOpener& getFileOpener(void) override { return mFileOpener; }
	virtual const GHFilePicker& getFilePicker(void) const override { return mFilePicker; }
	virtual const GHThreadFactory& getThreadFactory(void) const override { return mThreadFactory; }
	virtual const GHTimeCalculator& getTimeCalculator(void) const override { return mTimeCalculator; }
	virtual const GHSocketMgr& getSocketMgr(void) const override { return mSocketMgr; }

private:
	GHFOpenFileOpener mFileOpener;
	GHWin32FilePicker mFilePicker;
	GHWin32ThreadFactory mThreadFactory;
	GHWin32TimeCalculator mTimeCalculator;
	GHWinSockSocketMgr mSocketMgr;
	GHWin32OutputPipe mOutputPipe;
};

