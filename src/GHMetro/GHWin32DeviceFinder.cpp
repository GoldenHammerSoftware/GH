#include "GHWin32DeviceFinder.h"
#include <initguid.h>
#include <mmdeviceapi.h>
#include <SetupAPI.h>
#include <Functiondiscoverykeys_devpkey.h>
#include "GHPlatform/GHDebugMessage.h"
#include <stdio.h>

bool GHWin32DeviceFinder::findAudioDevicePath(const wchar_t* guidStr, wchar_t* outDevicePath, size_t pathBufSize)
{
#ifdef WINDOWS_STORE
	// todo: https://docs.microsoft.com/en-us/windows-hardware/drivers/install/porting-from-setupapi-to-cfgmgr32
	return false;
#else
	//https://social.msdn.microsoft.com/Forums/windowsdesktop/en-US/4989237b-814c-4a7a-8a35-00714d36b327/xaudio2-how-to-get-device-id-for-mastering-voice?forum=windowspro-audiodevelopment

	auto devInfoSet = SetupDiGetClassDevs(&DEVINTERFACE_AUDIO_RENDER, 0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
	if (devInfoSet == INVALID_HANDLE_VALUE) 
	{
		GHDebugMessage::outputString("SetupDiGetClassDevs returned INVALID_HANDLE_VALUE");
		return false;
	}

	SP_DEVINFO_DATA devInfo;
	devInfo.cbSize = sizeof(SP_DEVINFO_DATA);

	SP_DEVICE_INTERFACE_DATA devInterface;
	devInterface.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	SP_DEVICE_INTERFACE_DETAIL_DATA* interfaceDetail = 0;

	bool found = false;
	for (int i = 0; true; ++i)
	{
		DWORD deviceIndex = i;
		if (!SetupDiEnumDeviceInterfaces(devInfoSet, 0, &DEVINTERFACE_AUDIO_RENDER, deviceIndex, &devInterface))
		{
			DWORD error = GetLastError();
			if (error != ERROR_NO_MORE_ITEMS)
			{
				GHDebugMessage::outputString("SetupDiEnumDeviceInterfaces failed with error 0x%x", error);
			}
			break;
		}

		DWORD size = 0;
		SetupDiGetDeviceInterfaceDetail(devInfoSet, &devInterface, 0, 0, &size, 0);

		interfaceDetail = ((SP_DEVICE_INTERFACE_DETAIL_DATA*)realloc(interfaceDetail, size));
		interfaceDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		devInfo.cbSize = sizeof(SP_DEVINFO_DATA);

		if (!SetupDiGetDeviceInterfaceDetail(devInfoSet, &devInterface, interfaceDetail, size, 0, &devInfo))
		{
			DWORD error = GetLastError();
			GHDebugMessage::outputString("Error 0x%x calling SetupDiGetDeviceInterfaceDetail", error);
			continue;
		}
		else
		{
			if (containsGuidStr(guidStr, interfaceDetail->DevicePath))
			{
				size_t numCharsNeeded = wcslen(interfaceDetail->DevicePath) + 1;
				if (pathBufSize < numCharsNeeded)
				{
					GHDebugMessage::outputString("Error copying string: we need room for %zu wchar_t in the buffer to write the path, only %zu provided", numCharsNeeded, pathBufSize);
				}
				else
				{
					swprintf(outDevicePath, pathBufSize, L"%s", interfaceDetail->DevicePath);
					found = true;
				}
				break;
			}

			//other things we could look up
			//const unsigned int bufsz = 2048;
			//wchar_t deviceId[bufsz];
			//deviceId[0] = 0;
			//DWORD requiredSize;
			//if (!SetupDiGetDeviceInstanceId(devInfoSet, &devInfo, deviceId, bufsz, &requiredSize))
			//{
			//	DWORD error = GetLastError();
			//	GHDebugMessage::outputString("Error 0x%x calling SetupDiGetDeviceInstanceId", error);
			//	continue;
			//}
			//
			//wchar_t friendlyName[bufsz];
			//friendlyName[0] = 0;
			//DWORD propertyDataType;
			//if (!SetupDiGetDeviceRegistryProperty(devInfoSet, &devInfo, SPDRP_FRIENDLYNAME, &propertyDataType, (LPBYTE)friendlyName, size, 0))
			//{
			//	DWORD error = GetLastError();
			//	GHDebugMessage::outputString("Error 0x%x calling SetupDiGetDeviceRegistryProperty", error);
			//	continue;
			//}

			//if (!lstrcmp(friendlyName, L"Headphones (Rift Audio)"))
			//{
			//	GHDebugMessage::outputString("found oculus device");
			//	break;
			//}
		}
	}

	free(interfaceDetail);
	return found;
#endif
}

bool GHWin32DeviceFinder::containsGuidStr(const wchar_t* guidStr, const wchar_t* devicePath)
{
	// example of what we are looking for :
	// {0.0.0.00000000}.{ea7607d4-15e4-499b-8c10-ae93375f40c3} inside of
	// \\?\swd#mmdevapi#{0.0.0.00000000}.{ea7607d4-15e4-499b-8c10-ae93375f40c3}#{e6327cad-dcec-4949-ae8a-991e976a79d2}
	return wcsstr(devicePath, guidStr);
}