// Copyright Golden Hammer Software
#include "GHWin32SoundFinder.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHPlatform/win32/GHLChar.h"
#include "GHUtils/GHUtilsIdentifiers.h"
#include "GHUtils/GHPropertyContainer.h"

GHWin32SoundFinder::GHWin32SoundFinder(const GHFileOpener& fileOpener)
: mFileOpener(fileOpener)
, mFileFinder(fileOpener)
{
}

bool GHWin32SoundFinder::createSoundReader(const char* filename, const GHPropertyContainer* extraData, 
	Microsoft::WRL::ComPtr<IMFSourceReader>& ret, GHSoundIDInfo& soundID) const
{
	const char* path = filename;
	if (extraData) {
		const char* testPath = (const char*)extraData->getProperty(GHUtilsIdentifiers::FILEPATH);
		if (testPath) {
			path = testPath;
		}
	}
	GHDebugMessage::outputString("createSoundReader path %s", path);

	size_t filenameSize = 0;
	const int BUF_SZ = 512;
	wchar_t filenameBuffer[BUF_SZ];
	if (!mFileFinder.createFilePath(path, filenameBuffer, BUF_SZ, false))
	{
		GHDebugMessage::outputString("Couldn't find sound file %s", filename);
		return false;
	}

	HRESULT result = MFCreateSourceReaderFromURL(filenameBuffer, nullptr, &ret);
	if (FAILED(result))
	{
		GHDebugMessage::outputString("Failed to open sound file %s using function MFCreateSourceReaderFromURL with result 0x%X", filename, result);
		return false;
	}

	readSoundID(path, soundID);

	return true;
}

void GHWin32SoundFinder::readSoundID(const char* filename, GHSoundIDInfo& soundID) const
{
	GHFile* file = mFileOpener.openFile(filename, GHFile::FT_BINARY, GHFile::FM_READONLY);
	if (file) {
		file->readIntoBuffer();
		char* buffer;
		size_t bufferSize;
		file->getFileBuffer(buffer, bufferSize);

		readID3v1(buffer, bufferSize, soundID);
		readID3v2(buffer, bufferSize, soundID, filename);
		readM4AHeader(buffer, bufferSize, soundID);

		delete file;
	}
}

void GHWin32SoundFinder::readID3v1(const char* buffer, size_t bufferSize, GHSoundIDInfo& soundID) const
{
	const char* tag = &buffer[bufferSize - 128];
	if (tag[0] != 'T') return;
	if (tag[1] != 'A') return;
	if (tag[2] != 'G') return;

	struct V1Header
	{
		char tag[3];
		char song[30];
		char artist[30];
		char album[30];
		char year[4];
		char comment[30];
		char genre[30];
	};
	struct V1HeaderExtended
	{
		char tag[4];
		char song[60];
		char artist[60];
		char album[60];
		char speed[1];
		char genre[30];
		char startTime[6];
		char endTime[6];
	};

	if (*tag + 3 == '+')
	{
		V1HeaderExtended header;
		memcpy(&header, tag, sizeof(V1HeaderExtended));
		soundID.setArtistName(header.artist);
		soundID.setSongName(header.song);
		soundID.setAlbumName(header.album);
	}
	else
	{
		V1Header header;
		memcpy(&header, tag, sizeof(V1Header));
		soundID.setArtistName(header.artist);
		soundID.setSongName(header.song);
		soundID.setAlbumName(header.album);
	}
}

static void parseID3Text(const char* text, int size, int maxTextSize, char* dest)
{
	int charcode = text[0];
	if (charcode == 0) {
		// ISO-8859-1
		memcpy(dest, &text[1], min(maxTextSize, size) - 1);
	}
	else if (charcode == 3) {
		// UTF-8
		memcpy(dest, &text[1], min(maxTextSize, size) - 1);
	}
	else {
		// UTF-16 (1)
		wchar_t* wstr = new wchar_t[maxTextSize];
		::memset(wstr, 0, sizeof(wchar_t)*maxTextSize);
		MultiByteToWideChar(CP_ACP, 0, &text[3], size, wstr, maxTextSize);

		for (int i = 0; i < (size-3)/2 && i < maxTextSize; ++i)
		{
			dest[i] = (char)wstr[i * 2];
		}

		delete [] wstr;
	}
}

static void parseAttachedPicture(const char* data, int size, char*& dest, const char* filename, const GHWin32FileFinder& fileFinder)
{
	// create a filename where we will cache the attached picture.
	size_t oldLen = strlen(filename);
	size_t newLen = oldLen + 7 + 4; // cache\\ + name + .jpg or .png
	char* newName = new char[newLen];
	sprintf(newName, "cache\\%s.jpg", filename); // todo: handle other MIME types
	// replace \ or : with _
	for (size_t i = 7; i < newLen; ++i)
	{
		if (newName[i] == ':' || newName[i] == '\\' || newName[i] == '/') {
			newName[i] = '_';
		}
	}
	// if the file exists just return the filename in dest.
	if (fileFinder.fileExists(newName))
	{
		dest = newName;
		return;
	}

	size_t dataLoc = 0;
	int encodingType = data[dataLoc];
	dataLoc++;

	const char* pictureType = &data[dataLoc];
	size_t typeLen = strlen(pictureType);
	dataLoc += typeLen+1;

	int imgCat = data[dataLoc];
	dataLoc += 2;

	// seems from the docs there should be an image desc but I'm not seeing it.
	// the jpg starts with chars ÿØÿà

	// save out the image data to file.
	CreateDirectory(L"cache", NULL);
	FILE* file = ::fopen(newName, "wb");
	if (file) {
		::fwrite(&data[dataLoc], 1, size-dataLoc, file);
		fclose(file);
		dest = newName;
	}
	else {
		delete[] newName;
		dest = 0;
	}
}

void GHWin32SoundFinder::readID3v2(const char* buffer, size_t bufferSize, GHSoundIDInfo& soundID, const char* filename) const
{
	const char* tagHeader = buffer;
	if (tagHeader[0] != 'I') return;
	if (tagHeader[1] != 'D') return;
	if (tagHeader[2] != '3') return;

	int versionMajor = tagHeader[3];
	int versionMinor = tagHeader[4];
	if (versionMajor < 0 || versionMajor > 4)
	{
		return;
	}

	// a lot of this copied from http://www.ulduzsoft.com/2012/07/parsing-id3v2-tags-in-the-mp3-files/

	// skip the header.
	size_t tagSize = (tagHeader[9] & 0xFF) | ((tagHeader[8] & 0xFF) << 7) | ((tagHeader[7] & 0xFF) << 14) | ((tagHeader[6] & 0xFF) << 21) + 10;
	boolean usesSynch = (tagHeader[5] & 0x80) != 0 ? true : false;
	boolean hasExtendedHdr = (tagHeader[5] & 0x40) != 0 ? true : false;

	if (usesSynch) {
		int brkpt = 1;
	}

	const char* tag = tagHeader + 10;

	// Read the extended header length and skip it
	int extendedHeaderSize = 0;
	if (hasExtendedHdr)
	{
		extendedHeaderSize = *((int*)&tagHeader[10]);
	}
	tag += extendedHeaderSize;

	size_t totTagSize = 10 + extendedHeaderSize + tagSize;
	assert(totTagSize < bufferSize);
	if (totTagSize >= bufferSize) {
		GHDebugMessage::outputString("Can't read id3 tag, buffer too small");
		return;
	}

	const int ID3FrameSize = versionMajor < 3 ? 6 : 10;

	size_t length = tagSize;
	size_t pos = 0;
	const size_t maxNameSize = 256;
	char artist[maxNameSize];
	memset(artist, 0, maxNameSize);
	char album[maxNameSize];
	memset(album, 0, maxNameSize);
	char title[maxNameSize];
	memset(title, 0, maxNameSize);

	// Parse the tags
	// optimization note: the strcmps could be replaced with GHIdentifier comparisons if this bottlenecks.
	while (true)
	{
		int rembytes = (int)(length - pos);

		// Do we have the frame header?
		if (rembytes < ID3FrameSize)
			break;

		// Is there a frame?
		if (tag[pos] < 'A' || tag[pos] > 'Z')
			break;

		// Frame name is 3 chars in pre-ID3v3 and 4 chars after
		char framename[5] = { 0 };
		int framesize;

		if (versionMajor < 3)
		{
			memcpy(framename, &tag[pos], 3);
			framesize = ((tag[pos + 5] & 0xFF) << 8) | ((tag[pos + 4] & 0xFF) << 16) | ((tag[pos + 3] & 0xFF) << 24);
		}
		else
		{
			memcpy(framename, &tag[pos], 4);
			framesize = (tag[pos + 7] & 0xFF) | ((tag[pos + 6] & 0xFF) << 8) | ((tag[pos + 5] & 0xFF) << 16) | ((tag[pos + 4] & 0xFF) << 24);
		}

		if (pos + framesize > length)
			break;

		if (!strcmp(framename, "TPE1") || !strcmp(framename, "TPE2") || !strcmp(framename, "TPE3") || !strcmp(framename, "TPE"))
		{
			if (artist[0] == 0)
			{
				parseID3Text(&tag[pos + ID3FrameSize], framesize, maxNameSize, artist);
			}
		}
		else if (!strcmp(framename, "TIT2") || !strcmp(framename, "TIT"))
		{
			if (title[0] == 0)
			{
				parseID3Text(&tag[pos + ID3FrameSize], framesize, maxNameSize, title);
			}
		}
		else if (!strcmp(framename, "TALB"))
		{
			if (album[0] == 0)
			{
				parseID3Text(&tag[pos + ID3FrameSize], framesize, maxNameSize, album);
			}
		}
		else if (!strcmp(framename, "APIC"))
		{
			char* art = 0;
			parseAttachedPicture(&tag[pos + ID3FrameSize], framesize, art, filename, mFileFinder);
			if (art) {
				soundID.setPictureName(art);
				delete[] art;
			}
		}

		pos += framesize + ID3FrameSize;
		continue;
	}

	if (artist[0] != 0)
	{
		soundID.setArtistName(artist);
	}
	if (title[0] != 0)
	{
		soundID.setSongName(title);
	}
	if (album[0] != 0)
	{
		soundID.setAlbumName(album);
	}
	int brkpt = 1;
}

void GHWin32SoundFinder::readM4AHeader(const char* buffer, size_t bufferSize, GHSoundIDInfo& soundID) const
{
	//This format is not well documented. semi-decent overview can be found here: http://atomicparsley.sourceforge.net/mpeg-4files.html

	// first check file type.
	if (bufferSize < 8) return;
	if (buffer[4] != 'f') return;
	if (buffer[5] != 't') return;
	if (buffer[6] != 'y') return;
	if (buffer[7] != 'p') return;

	unsigned int atomSize;
	int i = 0;
	char atomName[5];
	atomName[4] = 0;

	const size_t maxNameSize = 256;
	char artist[maxNameSize];
	memset(artist, 0, maxNameSize);
	char album[maxNameSize];
	memset(album, 0, maxNameSize);
	char title[maxNameSize];
	memset(title, 0, maxNameSize);

	const char* dataStart;
	for (int i = 0; i < (int)bufferSize-8; ++i)
	{
		const char* test = &buffer[i];
		if (buffer[i] == '©') {
			if (tolower(buffer[i + 1]) == 'a') {
				if (tolower(buffer[i + 2]) == 'l') {
					if (tolower(buffer[i + 3]) == 'b')
					{
						if (!album[0])
						{
							atomSize = ((buffer[i - 4] & 0xff) << 24 | (buffer[i - 3] & 0xff) << 16 | (buffer[i - 2] & 0xff) << 8 | buffer[i - 1] & 0xff) << 0;

							//4 bytes atom length, 4 bytes aton name, 4 bytes crap, 4 bytes null, actual data.actual data is offset by 4 bytes for some reason
							// i think i did +16 and got garbage, figured out it was off by 4, and added 4 more
							dataStart = &buffer[i + 20]; 
							strncpy(album, dataStart, min(atomSize - 20, maxNameSize));
							i += atomSize - 1;

							if (album[0] && artist[0] && title[0]) { break; }
						}
					}
				}
				else if (tolower(buffer[i + 2]) == 'r') {
					if (tolower(buffer[i + 3]) == 't')
					{
						if (!artist[0])
						{
							atomSize = ((buffer[i - 4] & 0xff) << 24 | (buffer[i - 3] & 0xff) << 16 | (buffer[i - 2] & 0xff) << 8 | buffer[i - 1] & 0xff) << 0;
							dataStart = &buffer[i + 20];
							strncpy(artist, dataStart, min(atomSize - 20, maxNameSize));
							i += atomSize - 1;

							if (album[0] && artist[0] && title[0]) { break; }
						}
					}
				}
			}
			else if (tolower(buffer[i + 1]) == 'n') {
				if (tolower(buffer[i + 2]) == 'a') {
					if (tolower(buffer[i + 3]) == 'm')
					{
						if (!title[0])
						{
							atomSize = ((buffer[i - 4] & 0xff) << 24 | (buffer[i - 3] & 0xff) << 16 | (buffer[i - 2] & 0xff) << 8 | buffer[i - 1] & 0xff) << 0;
							dataStart = &buffer[i + 20];
							strncpy(title, dataStart, min(atomSize - 20, maxNameSize));
							i += atomSize - 1;

							if (album[0] && artist[0] && title[0]) { break; }
						}
					}
				}
			}
		}
	}

	if (artist[0] != 0)
	{
		soundID.setArtistName(artist);
	}
	if (title[0] != 0)
	{
		soundID.setSongName(title);
	}
	if (album[0] != 0)
	{
		soundID.setAlbumName(album);
	}
	int brkpt = 1;
}
