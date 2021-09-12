// Copyright Golden Hammer Software
#pragma once

#include <vector>

// a set of arguments we pass to the GHWin32AppLauncher
class GHWin32LaunchCfg
{
public:
	GHWin32LaunchCfg(void) : mWindowName("GHWindow"), mAllowFullscreen(true), mIconId(0) {}

public:
	// display name we pass to the win32 window
	const char* mWindowName;
	// ordered list of directories to look in for files.
	std::vector<const char*> mFileDirs;
	bool mAllowFullscreen;
	int mIconId;
};
