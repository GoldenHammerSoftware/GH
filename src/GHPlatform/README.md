# GHPlatform

GHPlatform provides a way to use common OS features for iOS, win32, uwp, android, and OSX in a platform independent way.  It also contains some C++ extensions.  It contains code we never want to have to write again, so it makes as few architectural decisions as possible.

## Dependencies
https://github.com/GoldenHammerSoftware/GHString - Install adjacent to where you install GHPlatform.  
  
If any additional dependencies are found mail dev@goldenhammersoftware.com and we'll either remove the dependency or bring that code into GHPlatform.

## How to use

*  Add the folder where GHPlatform lives to your #includes directories.  Ie if you are in src/GHPlatform/ then add src/ to your includes search path.
*  Check for a platform-specific library project.  There might be one in GHPlatform/ios/GHPlatform/GHPlatform.xcodeproj.  If one exists then add that to your project.
*  If we don't have a library project then add the cpp and header files in GHPlatform to your project.  Also add the GHPlatform/[platform] and GHPlatform/null files.
*  In platform code instantiate the appropriate GHPlatformServices.  Ie for win32 instantiate a GHPlatformServicesWin32 inside your winmain.
*  Have your code ask the GHPlatformServices interface for the platform independent factories.
*  Use any platform independent classes directly.

## GHPlatformServices

A central handle for the platform-specific implementation of interfaces that require platform code.  
  
On win32:  
```
#include "GHPlatform/win32/GHWin32PlatformServices.h"

main()
{
	// There should be exactly one instance of GHPlatformServices.
	// It should live for the lifetime of your program.
	// On android use GHAndroidPlatformServices instead etc.
	GHWin32PlatformServices platformServices;

	GHFile* file = platformServices.getFileOpener().openFile("blah.xml");
}
```

## Debug output

Printf does not always do what you would expect on every platform.  Some platforms have their own debug output function.

```
#include "GHPlatform/GHDebugMessage.h"

void outputSimpleDebugMessage(void)
{
	GHDebugMessage::outputString("something");
}
```

## File access

*  GHFileOpener - Platform independent file opener.  Add resource paths to look in multiple directories.
*  GHFile - Platform independent file access.
*  GHFileC / GHFOpenFileOpener - fopen implementation of file access.
*  GHXorFile - A simple xor encrypted file wrapper for mild security.

```
#include "GHPlatform/GHFileOpener.h"
#include "GHPlatform/GHFile.h"
#include "GHPlatform/GHDebugMessage.h"

void openSomeFile(const GHFileOpener& fileOpener)
{
	GHFile* file = fileOpener.openPlatformFile("blah.txt", GHFile::FT_TEXT, GHFile::FM_READONLY);
	if (!file)
	{
		GHDebugMessage::outputString("Failed to open blah.txt");
		return;
	}
	// Grab the file into memory.
	file->readIntoBuffer();
	// Get access to that file buffer memory.
	char* buffer;
	size_t bufferSize;
	file->getFileBuffer(buffer, bufferSize);

	// Do something with the file buffer here.

	// file currently owns the file buffer so deleting it will clean up.
	// If we had called file->releaseFileBuffer() we would have to manage the memory separately.
	delete file;
}
```

## Bytestream

Bytestream is a way to efficiently iterate a stream of data.  We've used this in beat line to process streaming song data.  It can be used as an interface for dealing with data that could come from either a file on disk or a streaming internet feed.

## Threads

*  GHThreadFactory - Platform interface for creating all things threaded.
*  GHRunnable - Interface for something that can be run by a thread.
*  GHThread - Manages running a GHRunnable in a platform thread.
*  GHThreadSleeper - A way to make a thread pause.
*  GHThreadSignal - A way to use signals to pause worker threads until an event happens.

```
#include "GHPlatform/GHThreadFactory.h"
#include "GHPlatform/GHThread.h"
#include "GHPlatform/GHRunnable.h"
#include "GHPlatform/GHDebugMessage.h"

class SpamRunnable : public GHRunnable
{
public:
	virtual void run(void)
	{
		while (true)
		{
			GHDebugMessage::outputString("Spam!");
			// infinite looping.
		}
}

void causeSpam(GHThreadFactory& threadFactory)
{
	// This will spam to the debug console constantly forever.
	// Production code should clean up these pointers when done.

	GHRunnable* runnableToLeak = new SpamRunnable();
	GHThread* threadToLeak = threadFactory.createThread();
	threadToLeak->runThread(runnableToLeak);
}
```

## Sockets

*  GHSocket - Interface for sending/receiving on a platform socket.
*  GHSocketMgr - Creates sockets for the current platform.

## Time

*  GHTimeVal - Represents a point in time.
*  GHTimeCalculator - Platform interface to get the current time.
*  GHTimeUpdater - Controller for incrementing a GHTimeVal to represent the new most current point in time.

```
#include "GHPlatform/GHTimeVal.h"
#include "GHPlatform/GHTimeCalculator.h"
#include "GHPlatform/GHTimeUpdater.h"
#include "GHPlatform/GHPlatformServices.h"
#include "GHPlatform/GHDebugMessage.h"

class ExampleTimeClass
{
public:
	ExampleTimeClass(GHPlatformServices& platformServices)
	: mTimeUpdater(platformServices.getTimeCalculator(), mTimeVal)
	{
		// Tell the time updater that it is active (not paused).
		// This is not necessary if using a GHControllerMgr.
		mTimeUpdater.activate();
	}

	void tick(void)
	{
		// Tell the time updater to process a new frame.  This updates mTimeVal.
		mTimeUpdater.update();

		GHDebugMessage::outputString("Current time info %f (%f since last frame)", mTimeVal.getTime(), mTimeVal.getTimePassed());
	}

private:
	GHTimeVal mTimeVal;
	GHTimeUpdater mTimeUpdater;
};
```

## C++ utilities

*  GHDeletionHandle - A class with a virtual destructor for storage in an ownership list for later deletion.
*  GHTypedDeletionHandle<T> - A templated way to use any non-virtual object in an ownership list.
*  GHOwnershipList - A way to store and delete a bunch of components held by GHDeletionHandles.
*  GHPrioritizedList - A list of objects sorted by priority.
*  GHRefCounted - An interface for something that is meant to be used as a shared pointer that is deleted when no longer in use.
*  GHRefCountedType<T> - A way to store a non virtual object as a ref counted object.
*  GHRefCountedDeletionHandle - A way to store a GHRefCounted release() in an ownship list instead of a destructable object.
*  GHSTATICASSERT - A way to catch some compile time errors.
*  GHSTLUtil - Some handy STL functions.

