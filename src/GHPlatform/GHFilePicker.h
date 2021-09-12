// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHRefCounted.h"
#include "GHString/GHString.h"
#include <vector>

class GHPropertyContainer;

// interface for allowing the user to select a file.
class GHFilePicker 
{
public:
	// interface for getting notifications that a file was picked
	class PickedCallback
	{
	public:
		class Result
		{
		public:
			Result(GHRefCounted* platformResult)
				: mPlatformResult(platformResult)
			{
				if (mPlatformResult) mPlatformResult->acquire();
			}
			Result(const Result& other)
			{
				*this = other;
				if (mPlatformResult) mPlatformResult->acquire();
			}
			~Result(void)
			{
				if (mPlatformResult) mPlatformResult->release();
			}

		public:
			GHString mFilePath;
			GHString mFileName;
			GHString mFileToken;
			// Arbitrary platform data.
			GHRefCounted* mPlatformResult{ 0 };
		};

		virtual ~PickedCallback(void) {}
		virtual void handleFilePicked(PickedCallback::Result& result) = 0;
	};

public:
	virtual ~GHFilePicker(void) {}

	// present the user with a file picker dialog and return the selected file name.
	virtual void pickFile(PickedCallback& callback, const std::vector<const char*> exts) const = 0;
	// present a file picker that can handle multi select.  repeatedly call the callback for each file.
	virtual void pickMultipleFiles(PickedCallback& callback, const std::vector<const char*> exts) const = 0;
};
