// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHStringIdFactoryDJB2.h"
#include "GHString/GHIdentifierMap.h"

class GHFileOpener;
class GHFilePicker;
class GHThreadFactory;
class GHTimeCalculator;
class GHSocketMgr;
class GHWindow;

// The interface for creating platform-specific factories.
class GHPlatformServices
{
public:
	GHPlatformServices(void);
	virtual ~GHPlatformServices(void);

	virtual GHFileOpener& getFileOpener(void) = 0;
	virtual const GHFilePicker& getFilePicker(void) const = 0;
	virtual const GHThreadFactory& getThreadFactory(void) const = 0;
	virtual const GHTimeCalculator& getTimeCalculator(void) const = 0;
	virtual const GHSocketMgr& getSocketMgr(void) const = 0;

	GHStringIdFactory& getIdFactory(void) { return mIdFactory; }
	GHIdentifierMap<int>& getEnumStore(void) { return mEnumStore; }

private:
	GHStringIdFactoryDJB2 mIdFactory;
	GHIdentifierMap<int> mEnumStore;
};

