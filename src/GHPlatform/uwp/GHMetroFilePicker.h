// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHFilePicker.h"
#include <vector>
#include "GHString/GHString.h"

class GHMetroRandomAccessStream;
class GHMetroStorageFile;

class GHMetroFilePicker : public GHFilePicker
{
public:
	class PlatformResult : public GHRefCounted
	{
	public:
		PlatformResult(GHMetroRandomAccessStream* ras, GHMetroStorageFile* sf);
	private:
		~PlatformResult(void);
	public:
		GHMetroRandomAccessStream* mGHStream;
		GHMetroStorageFile* mGHStorageFile;
	};

public:
	virtual void pickFile(PickedCallback& callback, const std::vector<const char*> exts) const;
	virtual void pickMultipleFiles(PickedCallback& callback, const std::vector<const char*> exts) const;

private:
	// initialize a windows file picker.
	Windows::Storage::Pickers::FileOpenPicker^ createWindowsPicker(const std::vector<const char*> exts) const;
	// let the game know about a file pick result.
	void triggerCallback(PickedCallback& callback, Windows::Storage::StorageFile^ file,
		Windows::Storage::Streams::IRandomAccessStream^ stream) const;
};
