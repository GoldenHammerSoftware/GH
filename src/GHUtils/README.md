# GHUtils

GHUtils is a dumping ground for useful C++ utilities.

## Dependencies
https://github.com/GoldenHammerSoftware/GHPlatform - Install adjacent to where you install GHUtils.  
https://github.com/GoldenHammerSoftware/GHString - Install adjacent to where you install GHUtils.  
  
If any additional dependencies are found mail dev@goldenhammersoftware.com and we'll either remove the dependency or bring that code into GHPlatform.

## How to use

*  Add the folder where GHUtils lives to your #includes directories.  Ie if you are in src/GHUtils/ then add src/ to your includes search path.
*  Instantiate a GHUtilsServices.
*  Use any classes directly.

## GHUtilsServices

Handles any setup/teardown for the library.

## Property 

*  GHPropertyContainer - A way to store arbitrary data on an object without knowing about it at compile time.  This is used as the data packet for messages for example.
*  GHProperty - An arbitrary chunk of data.  This can be a straight numeric type, a pointer to an object with a known life span, or a ref counted object.

## Message

*  GHMessage - An arbitrary chunk of data with an identifier.
*  GHMessageHandler - Interface for something that listens for a message.
*  GHEventMgr - A registry of listeners for messages by type.
*  GHMessageQueue - A thread-safe way to send out messages without an event manager dependency.

## Controller

*  GHController - Interface for something that has an update function.
*  GHControllerMgr - Manages a list of things that should update.  Activates/deactivates/sorts/calls-update.

## State machine

GHStateMachine manages switching between various states.  Each state has a list of transitions in and a list of transitions out.  We use it for things like switching from in-game to paused, with the paused transitions showing a gui and the in-game transitions pushing the physics updater and input controls.

*  GHStateMachine - A list of transitions for each state.
*  GHTransition - Interface for something that does something when entering or leaving a specific state.
*  GHControllerTransition - A transition that adds or removes a controller from a controller manager.

## Profiler

A lightweight way to get performance information about your code.  The macros are no-ops if GHRETAIL is defined.

*  Put GHPROFILESCOPE("arbitrary id string", GHString::CHT_REFERENCE) inside full functions or scope.
*  To profile within a scope use GHPROFILEBEGIN("string") and GHPROFILEEND("string").
*  Run a profile by putting GHPROFILESTART and GHPROFILESTOP somewhere.
*  Output the results to debug print by using GHPROFILEOUTPUT.
*  Reset the profile with GHPROFILECLEAR.

```
#include "GHUtils/GHProfiler.h"

void contrivedProfilingFunction(void)
{
	// Clear out the data from any previous profiling.
	GHPROFILECLEAR
	// Start a new profile.
	GHPROFILESTART

	{
		// Profile the time spent inside this scope.
		GHPROFILESCOPE("dumbFunction - Block 1", GHString::CHT_REFERENCE)
	}
	// Start profiling code outside of a scope.
	GHPROFILEBEGIN("dumbFunction - Block 2")

	// Finish profiling code outside of a scope.
	// The text must match the text used in GHPROFILEBEGIN.
	GHPROFILEEND("dumbFunction - Block 2") 

	// Finish profiling.
	GHPROFILESTOP
	// Dump the results to GHDebugMessage::outputString.
	GHPROFILEOUTPUT
}
```

## Resource

Wrapper for things loaded from disk.  These are ref counted and allow sharing.

*  GHResource - Base class for a thing that can be loaded from a GHResourceFactory.
*  GHResourcePtr<T> - Templated way to treat a non-virtual object as a GHResource.
*  GHResourceLoader - A loading function for a specific type of resource.
*  GHResourceFactory - A list of loaders for file extensions.  Call getUniqueResource or getCacheResource depending on whether you want a shared copy.
