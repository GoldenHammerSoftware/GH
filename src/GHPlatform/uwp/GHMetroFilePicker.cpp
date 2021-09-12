// Copyright 2010 Golden Hammer Software
#include "GHMetroFilePicker.h"
#include "GHPlatform/GHDebugMessage.h"
#include <string.h>
#include <wrl.h>
#include <ppltasks.h>  
#include "GHUtils/GHPropertyContainer.h"
#include "GHMetroRandomAccessStream.h"
#include "GHMetroStorageFile.h"
#include "GHPlatform/GHPlatformIdentifiers.h"
#include "GHMetroIdentifiers.h"

using namespace concurrency;

GHMetroFilePicker::PlatformResult::PlatformResult(GHMetroRandomAccessStream* ras, GHMetroStorageFile* sf)
	: mGHStream(ras)
	, mGHStorageFile(sf)
{
	if (mGHStream) mGHStream->acquire();
	if (mGHStorageFile) mGHStorageFile->acquire();
}

GHMetroFilePicker::PlatformResult::~PlatformResult(void)
{
	if (mGHStream) mGHStream->release();
	if (mGHStorageFile) mGHStorageFile->release();
}

void GHMetroFilePicker::pickFile(PickedCallback& callback, const std::vector<const char*> exts) const
{
	Windows::Storage::Pickers::FileOpenPicker^ openPicker = createWindowsPicker(exts);

	create_task(openPicker->PickSingleFileAsync()).then([this, &callback](Windows::Storage::StorageFile^ file)
	{
		if (!file)
		{
			// return empty results.
			PickedCallback::Result result(0);
			callback.handleFilePicked(result);
			return;
		}

		create_task(file->OpenAsync(Windows::Storage::FileAccessMode::Read)).then([this, &callback, file]
			(Windows::Storage::Streams::IRandomAccessStream^ stream)
		{
			this->triggerCallback(callback, file, stream);
		});

	});
}


void GHMetroFilePicker::pickMultipleFiles(PickedCallback& callback, const std::vector<const char*> exts) const
{
	Windows::Storage::Pickers::FileOpenPicker^ openPicker = createWindowsPicker(exts);

	create_task(openPicker->PickMultipleFilesAsync()).then([this, &callback](Windows::Foundation::Collections::IVectorView<Windows::Storage::StorageFile^>^ files)
	{
		if (files->Size > 0)
		{
			for (size_t i = 0; i < files->Size; ++i)
			{
				Windows::Storage::StorageFile^ file = files->GetAt(i);
				create_task(file->OpenAsync(Windows::Storage::FileAccessMode::Read)).then([this, &callback, file]
					(Windows::Storage::Streams::IRandomAccessStream^ stream)
				{
					this->triggerCallback(callback, file, stream);
				});
			}
		}
		else
		{
			// return empty results.
			PickedCallback::Result result(0);
			callback.handleFilePicked(result);
			return;
		}
	});
}

Windows::Storage::Pickers::FileOpenPicker^ GHMetroFilePicker::createWindowsPicker(const std::vector<const char*> exts) const
{
	Windows::Storage::Pickers::FileOpenPicker^ openPicker = ref new Windows::Storage::Pickers::FileOpenPicker();
	openPicker->ViewMode = Windows::Storage::Pickers::PickerViewMode::List;
	openPicker->SuggestedStartLocation = Windows::Storage::Pickers::PickerLocationId::MusicLibrary;

	for (size_t i = 0; i < exts.size(); ++i) {
		size_t len = strlen(exts[i]) + 1;
		wchar_t* wstr = new wchar_t[len];
		::memset(wstr, 0, sizeof(wchar_t)*len);
		MultiByteToWideChar(CP_ACP, 0, exts[i], len - 1, wstr, len);
		Platform::String^ extStr = ref new Platform::String(wstr);
		openPicker->FileTypeFilter->Append(extStr);
	}

	// todo: figure out no window set on picker exception.

	return openPicker;
}

void GHMetroFilePicker::triggerCallback(PickedCallback& callback, Windows::Storage::StorageFile^ file,
	Windows::Storage::Streams::IRandomAccessStream^ stream) const
{
	GHMetroRandomAccessStream* ghStream = new GHMetroRandomAccessStream;
	ghStream->mStream = stream;

	GHMetroStorageFile* ghSF = new GHMetroStorageFile;
	ghSF->mStorageFile = file;

	GHMetroFilePicker::PlatformResult* platResult = new GHMetroFilePicker::PlatformResult(ghStream, ghSF);
	GHFilePicker::PickedCallback::Result resultStruct(platResult);

	char* pathbuf = new char[file->Path->Length() + 1];
	memset(pathbuf, 0, file->Path->Length() + 1);
	wcstombs(pathbuf, file->Path->Data(), file->Path->Length());
	resultStruct.mFilePath.setChars(pathbuf, GHString::CHT_CLAIM);

	char* namebuf = new char[file->Name->Length() + 1];
	memset(namebuf, 0, file->Name->Length() + 1);
	wcstombs(namebuf, file->Name->Data(), file->Name->Length());
	resultStruct.mFileName.setChars(pathbuf, GHString::CHT_COPY);

	// store out the result for later opening without a picker.
	auto futureAccessToken = Windows::Storage::AccessCache::StorageApplicationPermissions::FutureAccessList->Add(file);
	char* tokenBuff = new char[futureAccessToken->Length() + 1];
	memset(tokenBuff, 0, futureAccessToken->Length() + 1);
	wcstombs(tokenBuff, futureAccessToken->Data(), futureAccessToken->Length());
	resultStruct.mFileToken.setChars(tokenBuff, GHString::CHT_CLAIM);

	callback.handleFilePicked(resultStruct);
}
