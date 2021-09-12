#include "GHWin32SystemServices.h"
#include "GHWin32Window.h"
#include "GHUtils\GHControllerMgr.h"
#include "GHShaderPreprocessor.h"
#include "GHPlatform\GHFileOpener.h"
#include "GHPlatform\GHPrintfOutputPipe.h"

#include "GHShaderConverter.h"

int main(int argc, char* argv[])
{
	GHControllerMgr systemControllerMgr;
	GHWin32Window dummyWindow(0, 0, 0, 0, "", 0, 0, 0, 0);
	GHWin32SystemServices* systemServices = new GHWin32SystemServices(dummyWindow, systemControllerMgr);

	// redirect output to printf since this is a command line app.
	// this lets us see output when using it.
	GHPrintfOutputPipe printPipe;
	GHDebugMessage::init(printPipe);

	GHDebugMessage::outputString("Welcome to GHShaderConverter.  We convert, you revert.");

	if (argc < 2)
	{
		GHDebugMessage::outputString("Specify a shader config xml file");
		return 1;
	}

	system("mkdir shader\\hlslout");
	system("mkdir shader\\spirvout");
	system("mkdir shader\\reflectionout");
	system("mkdir shader\\mslout");
	system("mkdir shader\\glslout");
	system("del /Q shader\\hlslout\\*");
	system("del /Q shader\\spirvout\\*");
	system("del /Q shader\\reflectionout\\*");
	system("del /Q shader\\mslout\\*");
	system("del /Q shader\\glslout\\*");

	GHShaderConverter converter(*systemServices);
	converter.runConfig(argv[1]);

	return 0;
}
