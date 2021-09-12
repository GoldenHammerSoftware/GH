// Copyright 2010 Golden Hammer Software
#include "GHAndroidFileOpener.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHPlatform/GHFileC.h"
#include <unistd.h>
#include <cassert>
#include "GHPlatform/android/GHJNIMgr.h"
#include <android/asset_manager.h> 
#include <android/asset_manager_jni.h>

// Deprecated: now using ndk assetmgr
namespace {
	struct GHAndroidLoadedFileInfo
	{
		FILE* mFilePtr;
		long mLength;
		GHAndroidLoadedFileInfo(void)
        : mFilePtr(0), mLength(0) { }
	};
}

// Deprecated: now using ndk assetmgr
jfieldID getFileDescriptorID(JNIEnv& jniEnv)
{
	jclass fileDescriptorClass = jniEnv.FindClass("java/io/FileDescriptor");
	if(fileDescriptorClass == 0) {
		GHDebugMessage::outputString("Cannot find FileDescriptor class.");
	}
    
	jfieldID fieldID = jniEnv.GetFieldID(fileDescriptorClass, "descriptor", "I");
	if(fieldID == 0) {
		GHDebugMessage::outputString("Cannot find file descriptor field id.");
	}
    
	return fieldID;
}

// Deprecated: now using ndk assetmgr
extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_loadFile(JNIEnv* env, jobject thiz, jobject fileDescriptorObj, jlong startOffset, jlong length, jlong fileDataPtr)
{
	jfieldID fileDescriptorFieldID = getFileDescriptorID(*env);
	jint fd = env->GetIntField(fileDescriptorObj, fileDescriptorFieldID);
    
	// leaking newfd?
	int newfd = dup(fd);
	FILE* file = fdopen(newfd, "rb");
	if(!file) {
		GHDebugMessage::outputString("Unable to open file!");
	}
    
	fseek(file, startOffset, SEEK_SET);
    
	GHAndroidLoadedFileInfo* fileInfo = reinterpret_cast<GHAndroidLoadedFileInfo*>(fileDataPtr);
	fileInfo->mFilePtr = file;
	fileInfo->mLength = length;
}

GHAndroidFileOpener::GHAndroidFileOpener(GHJNIMgr& jniMgr, jobject jobj, std::vector<GHString>&& saveFileDirectories, jobject jAssetMgr)
	: mJNIMgr(jniMgr)
	, mSaveFileDirectories(std::move(saveFileDirectories))
	, mJObject(jobj)
	, mMethodID(0)
	, mJAssetMgr(jAssetMgr)
{
	// Deprecated: now using ndk assetmgr
	jclass cls = mJNIMgr.getJNIEnv().GetObjectClass(mJObject);
	if(cls == 0) {
		GHDebugMessage::outputString("GHAndroidFileOpener can't find class where getFileDescription method should exist\n.");
	}
	mMethodID = mJNIMgr.getJNIEnv().GetMethodID(cls, "getFileDescription", "(Ljava/lang/String;J)V");
	if(mMethodID == 0) {
		GHDebugMessage::outputString("GHAndroidFileOpener can't find getFileDescription method in Java.\n");
	}
}

GHFile* GHAndroidFileOpener::openPlatformFile(const char* filename, GHFile::FileType fileType, GHFile::FileMode fileMode) const
{
    const char* flags;
    if (fileMode == GHFile::FM_READONLY)
    {
        if (fileType == GHFile::FT_BINARY)
        {
            flags = "rb";
        }
        else
        {
            flags = "rt";
        }
    }
    else
    {
        if (fileType == GHFile::FT_BINARY)
        {
            flags = "wb";
        }
        else
        {
            flags = "wt";
        }
    }
    
    GHFile* ret = openFileFromSaveFileDirectory(filename, flags, fileType, fileMode);
    if (!ret && fileMode == GHFile::FM_READONLY)
    {
        ret = openFileFromAPK(filename, fileType, fileMode);
    }
    
    if (!ret)
    {
        //GHDebugMessage::outputString("Failed to open file %s for %s", filename, fileMode == GHFile::FM_READONLY ? "read" : "write");
    }
    
    return ret;
}

GHFile* GHAndroidFileOpener::openFileFromSaveFileDirectory(const char* filename, const char* flags, GHFile::FileType fileType, GHFile::FileMode fileMode) const
{
	size_t numSaveFileDirectories = mSaveFileDirectories.size();
	for (size_t i = 0; i < numSaveFileDirectories; ++i)
	{
		const GHString& saveFileDirectory = mSaveFileDirectories[i];

		const size_t BUF_LEN = 512;
		char buf[BUF_LEN];
		assert(saveFileDirectory.getCharLen() + strlen(filename) < BUF_LEN);
		sprintf(buf, "%s%s", saveFileDirectory.getChars(), filename);
		FILE* file = fopen(buf, flags);
		if (file) 
		{
			return new GHFileC(file, fileType, fileMode);
		}
	}
	return 0;    
}

GHFile* GHAndroidFileOpener::openFileFromAPK(const char* filename, GHFile::FileType fileType, GHFile::FileMode fileMode) const
{
	AAssetManager* mgr = AAssetManager_fromJava(&mJNIMgr.getJNIEnv(), mJAssetMgr);
	AAsset* loadedAsset = AAssetManager_open(mgr, filename, AASSET_MODE_UNKNOWN);
	if (!loadedAsset)
	{
	    // try adding .mp3 to the end.
	    // in days gone past we couldn't put an uncompressed file in assets without renaming to .mp3
	    // not sure if that's still the case but we have a lot of data following that standard.
	    char renameBuff[2048];
	    snprintf(renameBuff, 2048, "%s.mp3", filename);
        loadedAsset = AAssetManager_open(mgr, renameBuff, AASSET_MODE_UNKNOWN);
        if (!loadedAsset)
        {
            //GHDebugMessage::outputString("Failed to AAssetManager_open %s", filename);
            return 0;
        }
	}

	GHFile* ret = 0;

	off_t assetStart;
	off_t assetLen;
	int assetFD = AAsset_openFileDescriptor(loadedAsset, &assetStart, &assetLen);
	AAsset_close(loadedAsset);
	if (assetFD >= 0)
	{
		FILE* file = fdopen(assetFD, "rb");
		if (!file) {
			GHDebugMessage::outputString("Unable to open FILE for file descriptor.");
			return ret;
		}

		fseek(file, assetStart, SEEK_SET);
		ret = new GHFileC(file, fileType, fileMode, assetLen);
	}
	else
	{
		GHDebugMessage::outputString("Failed to get file descriptor for %s", filename);
	}

	return ret;

	/* old code that required using custom java code to open asset files.
	
	jstring javaFilename = mJNIMgr.getJNIEnv().NewStringUTF(filename);
	if(javaFilename == 0) {
		GHDebugMessage::outputString("GHAndroidFileOpener::openFileFromAPK: Java won't give us a string. OOM?");
		return 0;
	}
    
	GHAndroidLoadedFileInfo loadedFileInfo;
	mJNIMgr.getJNIEnv().CallVoidMethod(mJObject, mMethodID, javaFilename, reinterpret_cast<long>(&loadedFileInfo));
	FILE* retFILE = loadedFileInfo.mFilePtr;
    
	mJNIMgr.getJNIEnv().DeleteLocalRef(javaFilename);
    
	if(!retFILE) {
		return 0;
	}
    
	return new GHFileC(retFILE, fileType, fileMode, loadedFileInfo.mLength);
	*/
}
