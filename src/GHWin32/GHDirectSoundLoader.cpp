// Copyright Golden Hammer Software
#include "GHDirectSoundLoader.h"
#include "GHDirectSoundHandle.h"
#include "GHDirectSoundDevice.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHPlatform/GHDebugMessage.h"

GHDirectSoundLoader::GHDirectSoundLoader(const GHFileOpener& fileOpener, GHDirectSoundDevice& device)
: mFileOpener(fileOpener)
, mDevice(device)
{
}

GHResource* GHDirectSoundLoader::loadFile(const char* filename, GHPropertyContainer* extraData)
{
	GHFile* file = mFileOpener.openFile(filename, GHFile::FT_BINARY, GHFile::FM_READONLY);
	if (!file)
	{
		GHDebugMessage::outputString("Could not find sound file %s", filename);
		return 0;
	}
	if (!file->readIntoBuffer())
	{
		GHDebugMessage::outputString("Failed to read sound file into buffer %s", filename);
		return 0;
	}

	char* buffer = 0;
	size_t bufferSize;
	if (!file->getFileBuffer(buffer, bufferSize))
	{
		GHDebugMessage::outputString("Failed to getFileBuffer %s", filename);
		return 0;
	}
	WaveHeader* waveHeader = 0;
	waveHeader = (WaveHeader*)buffer;
	buffer += sizeof(WaveHeader);

	if (!verifyHeader(*waveHeader)) 
	{
		GHDebugMessage::outputString("Unexpected wav format on %s", filename);
		return 0;
	}

	IDirectSoundBuffer8* sb8 = createEmptyBuffer(*waveHeader);
	if (!sb8)
	{
		GHDebugMessage::outputString("Failed to create empty sound buffer for %s", filename);
		return 0;
	}

	// Lock the sound buffer to write wave data into it.
	void* lockBuffer = 0;
	size_t lockBufferSize;
	HRESULT lockResult = sb8->Lock(0, waveHeader->dataSize, (void**)&lockBuffer, (DWORD*)&lockBufferSize, NULL, 0, 0);
	if (FAILED(lockResult))
	{
		GHDebugMessage::outputString("Failed to lock wave data for %s", filename);
		return 0;
	}

	// Copy the wave data into the buffer.
	memcpy(lockBuffer, buffer, waveHeader->dataSize);

	// Unlock the secondary buffer after the data has been written to it.
	HRESULT unlockResult = sb8->Unlock((void*)lockBuffer, (DWORD)lockBufferSize, NULL, 0);
	if (FAILED(unlockResult))
	{
		GHDebugMessage::outputString("Failed to unlock wave data for %s", filename);
		return 0;
	}

	delete file;
	GHDirectSoundHandle* ret = new GHDirectSoundHandle(sb8);
	return ret;
}

bool GHDirectSoundLoader::verifyHeader(const WaveHeader& header)
{
	// Check that the chunk ID is the RIFF format.
	if ((header.chunkId[0] != 'R') || (header.chunkId[1] != 'I') ||
		(header.chunkId[2] != 'F') || (header.chunkId[3] != 'F'))
	{
		GHDebugMessage::outputString("Expected RIFF");
		return false;
	}

	// Check that the file format is the WAVE format.
	if ((header.format[0] != 'W') || (header.format[1] != 'A') ||
		(header.format[2] != 'V') || (header.format[3] != 'E'))
	{
		GHDebugMessage::outputString("Expected WAVE");
		return false;
	}

	// Check that the sub chunk ID is the fmt format.
	if ((header.subChunkId[0] != 'f') || (header.subChunkId[1] != 'm') ||
		(header.subChunkId[2] != 't') || (header.subChunkId[3] != ' '))
	{
		GHDebugMessage::outputString("Expected fmt ");
		return false;
	}

	// Check that the audio format is WAVE_FORMAT_PCM.
	if (header.audioFormat != WAVE_FORMAT_PCM)
	{
		GHDebugMessage::outputString("Expected WAVE_FORMAT_PCM");
		return false;
	}

	// Check for the data chunk header.
	if ((header.dataChunkId[0] != 'd') || (header.dataChunkId[1] != 'a') ||
		(header.dataChunkId[2] != 't') || (header.dataChunkId[3] != 'a'))
	{
		GHDebugMessage::outputString("Expected data");
		return false;
	}
	/*
	// Check that the wave file was recorded in stereo format.
	if (header.numChannels != 2)
	{
		GHDebugMessage::outputString("Expected 2 channels");
		return false;
	}

	// Check that the wave file was recorded at a sample rate of 44.1 KHz.
	if (header.sampleRate != 44100)
	{
		GHDebugMessage::outputString("Expected 44100 sample rate");
		return false;
	}

	// Ensure that the wave file was recorded in 16 bit format.
	if (header.bitsPerSample != 16)
	{
		GHDebugMessage::outputString("Expected 16 bits per sample");
		return false;
	}
	*/
	return true;
}

IDirectSoundBuffer8* GHDirectSoundLoader::createEmptyBuffer(const WaveHeader& waveHeader)
{
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	IDirectSoundBuffer8* ret = 0;
	IDirectSoundBuffer* tempBuffer;

	// Set the wave format of secondary buffer that this wave file will be loaded onto.
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = waveHeader.sampleRate;
	waveFormat.wBitsPerSample = waveHeader.bitsPerSample;
	waveFormat.nChannels = waveHeader.numChannels;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Create a temporary sound buffer with the specific buffer settings.
	HRESULT result = mDevice.getDevice()->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if (FAILED(result))
	{
		GHDebugMessage::outputString("CreateSoundBuffer failed");
		return 0;
	}

	// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&ret);
	if (FAILED(result))
	{
		GHDebugMessage::outputString("QueryInterface IID_IDirectSoundBuffer8 failed");
		return 0;
	}

	// Release the temporary buffer.
	tempBuffer->Release();
	tempBuffer = 0;

	return ret;
}

