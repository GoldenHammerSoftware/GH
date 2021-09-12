// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHFileOpener.h"
#include <jni.h>
#include "GHString/GHString.h"
#include <string>
#include <vector>

class GHJNIMgr;

// TODO: remove the jni from this and use ndk asset mgr.
class GHAndroidFileOpener : public GHFileOpener
{
public:
    GHAndroidFileOpener(GHJNIMgr& jniMgr, jobject jobj, std::vector<GHString>&& saveFileDirectories, jobject jAssetMgr);

protected:
	virtual GHFile* openPlatformFile(const char* filename, GHFile::FileType fileType, GHFile::FileMode fileMode) const;

    
    GHFile* openFileFromSaveFileDirectory(const char* filename, const char* flags, GHFile::FileType fileType, GHFile::FileMode fileMode) const;
	GHFile* openFileFromAPK(const char* filename, GHFile::FileType fileType, GHFile::FileMode fileMode) const;

    
private:
	std::vector<GHString> mSaveFileDirectories;

	GHJNIMgr& mJNIMgr;
	jobject mJAssetMgr;
	jobject mJObject;
	jmethodID mMethodID;
};
