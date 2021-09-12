#pragma once

class GHWin32DeviceFinder
{
public:
	static bool findAudioDevicePath(const wchar_t* guidStr, wchar_t* outDevicePath, size_t pathBufSize);

private:
	static bool containsGuidStr(const wchar_t* guidStr, const wchar_t* devicePath);
};