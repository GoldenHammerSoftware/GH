// Copyright 2010 Golden Hammer Software
#include "GHMetroSoundFinder.h"
#include <stdio.h>
#include "GHPlatform/uwp/GHMetroIdentifiers.h"
#include "GHPlatform/uwp/GHMetroRandomAccessStream.h"
#include <ppltasks.h>
#include "GHSoundIDInfo.h"
#include "GHBaseIdentifiers.h"
#include <ppltasks.h>

#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHUtils/GHUtilsIdentifiers.h"
#include "GHPlatform/uwp/GHMetroStorageFile.h"
#include "GHPlatform/uwp/GHMetroFile.h"
#include "GHPlatform/uwp/GHMetroFileOpener.h"
#include "GHPlatform/uwp/GHMetroFilePicker.h"

static void createFilePath(const char* filename, wchar_t* ret, size_t retSize)
{
	Platform::String^ path = L"";
	// todo: avoid overflow.
	swprintf_s(ret, retSize, path->Data());
	MultiByteToWideChar(CP_ACP, 0, filename, -1, ((wchar_t*)ret) + path->Length(), retSize - path->Length());
}

GHMetroSoundFinder::GHMetroSoundFinder(const GHMetroFileOpener& fileOpener)
: mFileOpener(fileOpener)
{
}

void GHMetroSoundFinder::collectSoundID(Windows::Storage::StorageFile^ storageFile, GHSoundIDInfo& retInfo) const
{
	auto dispatcher = Windows::UI::Core::CoreWindow::GetForCurrentThread();
	// we can't do this stuff on the ui thread.  null dispatcher means not ui thread.
	if (dispatcher != nullptr)
	{
		return;
	}

	if (storageFile == nullptr) {
		GHDebugMessage::outputString("collectSoundID called with null storageFile");
		return;
	}

	try {
		GHSoundIDInfo* soundInfo = &retInfo;
		auto task = concurrency::create_task(storageFile->Properties->GetMusicPropertiesAsync()).then(
			[this, soundInfo](Windows::Storage::FileProperties::MusicProperties^ soundId)
		{
			if (soundId == nullptr) {
				GHDebugMessage::outputString("No metadata found for sound");
				return;
			}
			GHSoundIDInfo& ghID = *((GHSoundIDInfo*)soundInfo);

			Platform::String^ artistStr = soundId->Artist;
			std::wstring artistW(artistStr->Begin());
			std::string artistC(artistW.begin(), artistW.end());
			ghID.setArtistName(artistC.c_str());

			Platform::String^ songStr = soundId->Title;
			std::wstring songW(songStr->Begin());
			std::string songC(songW.begin(), songW.end());
			ghID.setSongName(songC.c_str());

			Platform::String^ albumStr = soundId->Album;
			std::wstring albumW(albumStr->Begin());
			std::string albumC(albumW.begin(), albumW.end());
			ghID.setAlbumName(albumC.c_str());
		});
		task.wait();

		char thumbSaveNameBuf[1024];
		char* thumbSaveName = &thumbSaveNameBuf[0];
		snprintf(thumbSaveName, 1024, "%s_%s_%s.bmp", soundInfo->getArtistName(), soundInfo->getAlbumName(), soundInfo->getSongName());
		const GHFileOpener* fileOpenerArg = &mFileOpener;

		const Windows::Storage::FileProperties::ThumbnailMode thumbnailMode = Windows::Storage::FileProperties::ThumbnailMode::MusicView;
		const size_t thumbSize = 128;
		auto thumbTask = concurrency::create_task(storageFile->GetThumbnailAsync(thumbnailMode, thumbSize)).then(
			[this, storageFile, thumbSize, thumbSaveName, fileOpenerArg, soundInfo](Windows::Storage::FileProperties::StorageItemThumbnail^ thumbnail)
		{
			if (thumbnail != nullptr)
			{
				if (thumbnail->Type == Windows::Storage::FileProperties::ThumbnailType::Image)
				{
					Windows::Storage::Streams::Buffer^ MyBuffer = ref new Windows::Storage::Streams::Buffer(thumbnail->Size);
					auto thumbReadTask = concurrency::create_task(thumbnail->ReadAsync(MyBuffer, MyBuffer->Capacity, Windows::Storage::Streams::InputStreamOptions::None)).then(
						[this, thumbSaveName, fileOpenerArg, soundInfo](Windows::Storage::Streams::IBuffer^ ibuffer)
					{
						GHFile* saveFile = fileOpenerArg->openFile(thumbSaveName, GHFile::FT_BINARY, GHFile::FM_WRITEONLY);
						if (saveFile)
						{
							auto dataReader = Windows::Storage::Streams::DataReader::FromBuffer(ibuffer);
							for (int i = 0; i < ibuffer->Length; ++i)
							{
								unsigned char theByte = dataReader->ReadByte();
								saveFile->writeBuffer(&theByte, 1);
							}
							soundInfo->setPictureName(thumbSaveName);
							delete saveFile;
						}
						int brkpt = 1;
						brkpt++;
					});
					thumbReadTask.wait();
				}
			}
		});
		thumbTask.wait();

	}
	catch (Platform::Exception^ exception)
	{
		GHDebugMessage::outputString("Exception getting MusicProperties.");
	}
}

bool GHMetroSoundFinder::createSoundReader(const char* filename, const GHPropertyContainer* extraData, 
	Microsoft::WRL::ComPtr<IMFSourceReader>& ret, GHSoundIDInfo& retInfo) const
{
	Windows::Storage::StorageFile^ storageFile = nullptr;
	size_t filenameSize = 0;
	const int BUF_SZ = 512;
	wchar_t filenameBuffer[BUF_SZ];
	Windows::Storage::StorageFile^ futureAccessFile = nullptr;

	auto dispatcher = Windows::UI::Core::CoreWindow::GetForCurrentThread();

	if (extraData)
	{
		// probably not the best dependency to get PlatformResult.
		GHMetroFilePicker::PlatformResult* platResult = (GHMetroFilePicker::PlatformResult*)extraData->getProperty(GHUtilsIdentifiers::FILEPLATFORMDESC);

		GHMetroRandomAccessStream* ghStream = 0;
		if (platResult) ghStream = platResult->mGHStream;
		Windows::Storage::Streams::IRandomAccessStream^ stream = nullptr;
		if (ghStream) {
			stream = ghStream->mStream;
		}

		// try to open from the future access cache if possible
		if (!stream)
		{
			const char* filekey = extraData->getProperty(GHUtilsIdentifiers::FILETOKEN);
			if (filekey)
			{
				createFilePath(filekey, filenameBuffer, BUF_SZ);
				Platform::String^ filekeyStr = ref new Platform::String(filenameBuffer);
				bool inFAC = Windows::Storage::AccessCache::StorageApplicationPermissions::FutureAccessList->ContainsItem(filekeyStr);
				if (inFAC)
				{
					// we can't do this stuff on the ui thread.  null dispatcher means not ui thread.
					if (dispatcher != nullptr) {
						GHDebugMessage::outputString("Can't open future access list file on the UI thread.");
					}
					else
					{
						struct ACHolder {
							Windows::Storage::StorageFile^ loadedFile;
							Windows::Storage::Streams::IRandomAccessStream^ loadedStream;
						};
						ACHolder acHolder;
						ACHolder* acHPtr = &acHolder;
						try {
							auto openTask = concurrency::create_task(
								Windows::Storage::AccessCache::StorageApplicationPermissions::FutureAccessList->GetFileAsync(filekeyStr)).then(
								[this, acHPtr](Windows::Storage::StorageFile^ pickedfile)
							{
								acHPtr->loadedFile = pickedfile;
								auto byteStreamTask = concurrency::create_task(
									pickedfile->OpenAsync(Windows::Storage::FileAccessMode::Read)).then([this, acHPtr](Windows::Storage::Streams::IRandomAccessStream^ raStream)
								{
									acHPtr->loadedStream = raStream;
								});
								byteStreamTask.wait();
							});
							openTask.wait();
							stream = acHPtr->loadedStream;
							storageFile = acHPtr->loadedFile;
						}
						catch (Platform::Exception^ exception)
						{
							GHDebugMessage::outputString("Exception getting storage file from access cache.");
						}
					}
				}
			}
		}

		if (stream)
		{
			if (!storageFile)
			{
				if (platResult)
				{
					GHMetroStorageFile* ghStorageFile = platResult->mGHStorageFile;
					if (ghStorageFile)
					{
						storageFile = ghStorageFile->mStorageFile;
					}
				}
			}
			if (storageFile)
			{
				collectSoundID(storageFile, retInfo);
			}

			Microsoft::WRL::ComPtr<IMFByteStream> byteStream = 0;
			HRESULT hr = MFCreateMFByteStreamOnStreamEx((IUnknown*)stream, &byteStream);
			if (SUCCEEDED(hr))
			{
				hr = MFCreateSourceReaderFromByteStream(byteStream.Get(), nullptr, &ret);
				if (SUCCEEDED(hr))
				{
					return true;
				}
			}
		}
	}

	createFilePath(filename, filenameBuffer, BUF_SZ);

	HRESULT result = MFCreateSourceReaderFromURL(filenameBuffer, nullptr, &ret);
	if (FAILED(result))
	{
		GHDebugMessage::outputString("Failed to open sound file %s using function MFCreateSourceReaderFromURL with result 0x%X", filename, result);
		return false;
	}

	// we can't do this stuff on the ui thread.  null dispatcher means not ui thread.
	if (dispatcher == nullptr)
	{
		// file not loaded through picker, so try to get the metadata a different way.
		wchar_t fnameW[2048];
		mFileOpener.createFilePath(filename, false, fnameW, 2048);
		Platform::String^ fnamePStr = ref new Platform::String(fnameW);

		struct SFHolder {
			Windows::Storage::StorageFile^ loadedFile;
		};
		SFHolder sfHolder;
		SFHolder* sfHPtr = &sfHolder;
		GHSoundIDInfo* retInfoPtr = &retInfo;
		try {
			auto task = concurrency::create_task(Windows::Storage::StorageFile::GetFileFromPathAsync(fnamePStr)).then(
				[this, sfHPtr, retInfoPtr](Windows::Storage::StorageFile^ storageFile)
			{
				sfHPtr->loadedFile = storageFile;
				collectSoundID(sfHPtr->loadedFile, (GHSoundIDInfo&)*retInfoPtr);
			});
			task.wait();
		}
		catch (Platform::Exception^ exception)
		{
			GHDebugMessage::outputString("Exception getting storage file for sound metadata.");
		}
	}

	return true;
}
