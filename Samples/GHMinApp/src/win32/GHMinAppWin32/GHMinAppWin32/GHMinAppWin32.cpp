#include "stdafx.h"
#include "GHMinAppWin32.h"
#include "GHWin32/GHWin32AppLauncher.h"
#include "MCApp.h"
#include "GHWin32/GHWin32LaunchCfg.h"

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	GHWin32LaunchCfg cfg;
	cfg.mWindowName = "GH Min App";

	// todo: change these dirs for a retail build.
	// settings relative to: ghroot\\gh\\ghminapp\\src\\win32\\SB2Win32\\SB2Win32
	cfg.mFileDirs.push_back("../../../../../../data/shared/");
	cfg.mFileDirs.push_back("../../../../data/minapp/");
	cfg.mFileDirs.push_back("../../../../../../data/DX11Shaders/");
	cfg.mFileDirs.push_back("../../../../../../data/CompiledShadersDX11/");

	GHWin32AppLauncher appLauncher(hInstance, hPrevInstance, lpCmdLine, nCmdShow, cfg);
	MCApp app(appLauncher.getSystemServices(), appLauncher.getRenderServices(), appLauncher.getGameServices());
	appLauncher.run(app);
	return 0;
}
