// Copyright 2010 Golden Hammer Software
#include "GHMetroSocketTCP.h"
#include <string.h>
#include <stdlib.h>
#include <ppltasks.h>
#include "GHPlatform/GHDebugMessage.h"

GHMetroSocketTCP::GHMetroSocketTCP(const char* addr, const char* port)
: mStreamSocket(nullptr)
, mDataWriter(nullptr)
, mDataReader(nullptr)
, mConnected(false)
{
	mStreamSocket = ref new Windows::Networking::Sockets::StreamSocket;

	size_t addrLen = strlen(addr) + 1;
	wchar_t * wcstring = new wchar_t[addrLen];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wcstring, addrLen, addr, _TRUNCATE);
	Platform::String^ addrStr = ref new Platform::String(wcstring);
	delete[] wcstring;

	Windows::Networking::HostName^ hostName = nullptr;
	try
	{
		hostName = ref new Windows::Networking::HostName(addrStr);
	}
	catch (Platform::InvalidArgumentException^ e)
	{
		GHDebugMessage::outputString("Failed to create hostName in GHMetroSocketTCP");
		return;
	}

	size_t portLen = strlen(port) + 1;
	wcstring = new wchar_t[portLen];
	convertedChars = 0;
	mbstowcs_s(&convertedChars, wcstring, portLen, port, _TRUNCATE);
	Platform::String^ portStr = ref new Platform::String(wcstring);
	delete[] wcstring;

	try {
		auto connectTask = concurrency::create_task(mStreamSocket->ConnectAsync(hostName, portStr));
		connectTask.wait();
		connectTask.get();
	}
	catch (Platform::Exception^ e)
	{
		GHDebugMessage::outputString("GHMetroSocketTCP connect failed");
		return;
	}

	mDataWriter = ref new Windows::Storage::Streams::DataWriter(mStreamSocket->OutputStream);
	mDataReader = ref new Windows::Storage::Streams::DataReader(mStreamSocket->InputStream);
	mConnected = true;
}

GHMetroSocketTCP::~GHMetroSocketTCP(void)
{

}

bool GHMetroSocketTCP::send(const char* data, size_t dataLen)
{
	if (!mConnected) return false;

	try 
	{
		// todo: avoid the copying going on here.  stupid windows api.
		auto platformBuffer = ref new Platform::Array<BYTE>((byte*)data, (UINT)dataLen);
		mDataWriter->WriteBytes(platformBuffer);
		auto sendTask = concurrency::create_task(mDataWriter->StoreAsync());
		sendTask.wait();
		sendTask.get();
	}
	catch (Platform::Exception^ e)
	{
		GHDebugMessage::outputString("Failed to write to socket, assume disconnected");
		mConnected = false;
		return false;
	}
	return true;
}

bool GHMetroSocketTCP::read(char* buffer, size_t bufferLen, size_t& sizeRead)
{
	if (!mConnected) {
		sizeRead = 0;
		return false;
	}

	unsigned int unreadSize = 0;
	try
	{
		auto readTask = concurrency::create_task(mDataReader->LoadAsync(bufferLen));
		readTask.wait();
		readTask.get();
		unreadSize = mDataReader->UnconsumedBufferLength;
		if (unreadSize == 0)
		{
			// nothing to read but not disconnected?
			sizeRead = 0;
			return true;
		}
	}
	catch (Platform::Exception^ e)
	{
		GHDebugMessage::outputString("Failure reading from socket");
		mConnected = false;
		sizeRead = 0;
		return false;
	}
	// dataReader has stuff to read.
	if (unreadSize >= bufferLen)
	{
		sizeRead = bufferLen - 1;
	}
	else
	{
		sizeRead = unreadSize;
	}
	auto platformBuffer = ref new Platform::Array<BYTE>((UINT)sizeRead);
	mDataReader->ReadBytes(platformBuffer);
	memcpy(buffer, platformBuffer->Data, sizeRead);
	return true;
}
