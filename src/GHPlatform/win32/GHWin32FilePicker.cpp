// Copyright Golden Hammer Software
#include "GHWin32FilePicker.h"
#include "GHWin32Include.h"
#include "GHString/GHString.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHUtils/GHUtilsIdentifiers.h"

GHWin32FilePicker::GHWin32FilePicker(HWND hwnd)
: mHwnd(hwnd)
{
}

void GHWin32FilePicker::pickFile(PickedCallback& callback, const std::vector<const char*> exts) const
{
	CComPtr<IFileOpenDialog> pDlg;
	wchar_t* wExts = 0;
	bool dlgOk = createWin32Picker(exts, pDlg, wExts);

	PickedCallback::Result result(0);
	if (!dlgOk) {
		delete[] wExts;
		callback.handleFilePicked(result);
		return;
	}

	pDlg->SetTitle(L"Pick a file");
	
	HRESULT hr = pDlg->Show(mHwnd);
	delete[] wExts;

	if (SUCCEEDED(hr))
	{
		CComPtr<IShellItem> pItem;
		hr = pDlg->GetResult(&pItem);

		if (SUCCEEDED(hr))
		{
			sendPickResults(callback, pItem);
		}
		else
		{
			callback.handleFilePicked(result);
		}
	}
	else
	{
		callback.handleFilePicked(result);
	}
}

void GHWin32FilePicker::pickMultipleFiles(PickedCallback& callback, const std::vector<const char*> exts) const
{
	CComPtr<IFileOpenDialog> pDlg;
	wchar_t* wExts = 0;
	bool dlgOk = createWin32Picker(exts, pDlg, wExts);

	if (!dlgOk) {
		delete[] wExts;
		PickedCallback::Result result(0);
		callback.handleFilePicked(result);
		return;
	}

	pDlg->SetTitle(L"Pick files");
	DWORD dwFlags = 0;
	pDlg->GetOptions(&dwFlags);
	pDlg->SetOptions(dwFlags | FOS_ALLOWMULTISELECT | FOS_PATHMUSTEXIST | FOS_FORCEFILESYSTEM);

	HRESULT hr = pDlg->Show(mHwnd);
	delete[] wExts;

	bool sentAtLeastOne = false;
	if (SUCCEEDED(hr))
	{
		IShellItemArray *pickResults;
		hr = pDlg->GetResults(&pickResults);
		if (SUCCEEDED(hr))
		{
			DWORD numResults;
			pickResults->GetCount(&numResults);
			for (DWORD i = 0; i < numResults; ++i)
			{
				IShellItem* item = 0;
				hr = pickResults->GetItemAt(i, &item);
				if (SUCCEEDED(hr))
				{
					sendPickResults(callback, item);
					sentAtLeastOne = true;
				}
			}
		}
	}

	if (!sentAtLeastOne)
	{
		PickedCallback::Result result(0);
		callback.handleFilePicked(result);
	}
}

bool GHWin32FilePicker::createWin32Picker(const std::vector<const char*>& exts, CComPtr<IFileOpenDialog>& pDlg,
	wchar_t*& wExts) const
{
	HRESULT hr;

	size_t extLens = 1;
	for (size_t i = 0; i < exts.size(); ++i) {
		extLens += strlen(exts[i]) + 2; // *[ext];
	}
	wExts = new wchar_t[extLens];
	memset(wExts, 0, extLens*sizeof(wchar_t));
	for (size_t i = 0; i < exts.size(); ++i)
	{
		int len = (int)strlen(exts[i]) + 1;
		wchar_t* wstr = new wchar_t[len];
		::memset(wstr, 0, sizeof(wchar_t)*len);
		MultiByteToWideChar(CP_ACP, 0, exts[i], len - 1, wstr, len);

		wsprintf(wExts, L"%s*%s;", wExts, wstr);
		delete[] wstr;
	}

	COMDLG_FILTERSPEC aFileTypes[] = {
		{ L"files", wExts }
	};

	// Create the file-open dialog COM object.
	hr = pDlg.CoCreateInstance(__uuidof(FileOpenDialog));
	if (FAILED(hr)) return false;

	pDlg->SetFileTypes(_countof(aFileTypes), aFileTypes);
	return true;
}

void GHWin32FilePicker::sendPickResults(PickedCallback& callback, IShellItem* pItem) const
{
	PickedCallback::Result result(0);

	LPOLESTR pwsz = NULL;
	HRESULT hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pwsz);

	if (SUCCEEDED(hr))
	{
		char* charbuf = new char[lstrlen(pwsz) + 1];
		memset(charbuf, 0, lstrlen(pwsz) + 1);
		wcstombs(charbuf, pwsz, lstrlen(pwsz));
		result.mFilePath.setChars(charbuf, GHString::CHT_CLAIM);
		CoTaskMemFree(pwsz);
	}
	hr = pItem->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &pwsz);
	if (SUCCEEDED(hr))
	{
		char* charbuf = new char[lstrlen(pwsz) + 1];
		memset(charbuf, 0, lstrlen(pwsz) + 1);
		wcstombs(charbuf, pwsz, lstrlen(pwsz));
		result.mFileName.setChars(charbuf, GHString::CHT_CLAIM);
		CoTaskMemFree(pwsz);
	}
	callback.handleFilePicked(result);
}
