// Copyright 2010 Golden Hammer Software
#include "GHMetroFileOpener.h"
#include "GHMetroFile.h"
#include "GHPlatform/GHDebugMessage.h"

#include <wrl.h>
using namespace Microsoft::WRL;  
using namespace Windows::Storage;

GHMetroFileOpener::GHMetroFileOpener(const wchar_t* readSubPath, const wchar_t* writeSubPath)
{
	auto localFolder = Windows::Storage::ApplicationData::Current->LocalFolder;
	auto installFolder = Windows::ApplicationModel::Package::Current->InstalledLocation;
	
	mReadPath = Platform::String::Concat(installFolder->Path, ref new Platform::String(readSubPath));
	mWritePath = Platform::String::Concat(localFolder->Path, ref new Platform::String(writeSubPath));
}

bool GHMetroFileOpener::createFilePath(const char* filename, bool forWrite, wchar_t* ret, size_t retSize) const
{
	Platform::String^ path;
	if (forWrite) path = mWritePath;
	else path = mReadPath;

	swprintf_s(ret, retSize, path->Data());
	MultiByteToWideChar(CP_ACP, 0, filename, -1, ((wchar_t*)ret)+path->Length(), retSize-path->Length());
	return true;
}

GHMetroFile* GHMetroFileOpener::createFile(Platform::String^ path, const char* filename, GHFile::FileMode fileMode) const
{
	const int BUF_SZ = 512;
	wchar_t filenameBuffer[BUF_SZ];
	swprintf_s(filenameBuffer, BUF_SZ, path->Data());

	MultiByteToWideChar(CP_ACP, 0, filename, -1, ((wchar_t*)filenameBuffer)+path->Length(), BUF_SZ-path->Length());

	CREATEFILE2_EXTENDED_PARAMETERS extendedParams = {0}; 
    extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS); 
    extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL; 
    extendedParams.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN; 
    extendedParams.dwSecurityQosFlags = SECURITY_ANONYMOUS; 
    extendedParams.lpSecurityAttributes = nullptr; 
    extendedParams.hTemplateFile = nullptr; 

	HANDLE fileHandle = 0;
	if (fileMode == GHFile::FM_READONLY)
	{
		fileHandle = CreateFile2(filenameBuffer, 
									GENERIC_READ,
									FILE_SHARE_READ,
									OPEN_EXISTING,
									&extendedParams);
	}
	else if (fileMode == GHFile::FM_WRITEONLY)
	{
		fileHandle = CreateFile2(filenameBuffer, 
									GENERIC_WRITE,
									0,
									CREATE_ALWAYS,
									&extendedParams);
	}
	else if (fileMode == GHFile::FM_READWRITE)
	{
		fileHandle = CreateFile2(filenameBuffer, 
									GENERIC_READ | GENERIC_WRITE,
									0,
									CREATE_NEW,
									&extendedParams);
	}
	if (!fileHandle || fileHandle == INVALID_HANDLE_VALUE) {
		return 0;
	}
	GHMetroFile* ret = new GHMetroFile(fileHandle, filename);
	if (ret && !ret->isValid()) {
		delete ret;
		ret = 0;
	}

	return ret;
}

GHFile* GHMetroFileOpener::openPlatformFile(const char* filename, GHFile::FileType fileType, GHFile::FileMode fileMode) const
{	
	GHMetroFile* ret = 0;

	// first look in our write override directory
	ret = createFile(mWritePath, filename, fileMode);

	// if not found and readonly, check the bundle.
	if (!ret && fileMode == GHFile::FM_READONLY)
	{
		ret = createFile(mReadPath, filename, fileMode);
	}
	else if (ret) { GHDebugMessage::outputString("Loaded serialized %s", filename); }

	/*
	if (!ret)
	{
		GHDebugMessage::outputString("Failed to open file %s", filename);
	}
	*/

	return ret;
}
