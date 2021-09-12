// WindowsDataCopyGenerator.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <initguid.h> // needs to be included before directsound.
#include <vector>

#include "stdafx.h"

// load in a datacopy.sh from command line and spit out a datacopy.bat
int _tmain(int argc, _TCHAR* argv[])
{
	wchar_t debugBuf[10024];

	::OutputDebugString(L"Running windows data copy generator\n");
	if (argc < 3) {
		::OutputDebugString(L"Useage: windowsdatacopygenerator input.sh output.bat\n");
	}

	wsprintf(debugBuf, L"Opening input file %s\n", argv[1]);
	::OutputDebugString(debugBuf);

	FILE* file = 0;
	_wfopen_s(&file, argv[1], L"rt");
	if (!file) {
		::OutputDebugString(L"Failed to open input file");
		return 1;
	}

	// read into mem buffer
	::fseek(file, 0, SEEK_END);
	size_t bufferSize = ::ftell(file);
	::rewind(file);
	char* fileBuffer = new char[bufferSize + 1];
	::fread(fileBuffer, 1, bufferSize, file);
	fileBuffer[bufferSize] = '\0';
	fclose(file);

	// translate the sh into windows commands
	std::vector<char> outputBuf;
	bool foundcp = false;
	for (size_t i = 0; i < bufferSize; ++i)
	{
		if (!foundcp && fileBuffer[i] == 'c' && fileBuffer[i + 1] == 'p')
		{
			i++;
			outputBuf.push_back('c');
			outputBuf.push_back('o');
			outputBuf.push_back('p');
			outputBuf.push_back('y');
			foundcp = true;
			continue;
		}
		if (fileBuffer[i] == '/')
		{
			outputBuf.push_back('\\');
			continue;
		}
		if (fileBuffer[i] == '\n') {
			foundcp = false;
		}
		outputBuf.push_back(fileBuffer[i]);
	}

	// and spit out the .bat
	FILE* outfile = 0;
	_wfopen_s(&outfile, argv[2], L"wt");
	if (!outfile) {
		::OutputDebugString(L"Failed to open output file");
		return 1;
	}
	char str[2];
	str[1] = 0;
	for (size_t i = 0; i < outputBuf.size(); ++i)
	{
		str[0] = outputBuf[i];
		fwrite(str, 1, 1, outfile);
	}
	fclose(outfile);

	return 0;
}

