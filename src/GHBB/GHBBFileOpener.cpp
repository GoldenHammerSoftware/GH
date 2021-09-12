// Copyright 2010 Golden Hammer Software
#include "GHBBFileOpener.h"
#include "GHFileC.h"
#include "GHDebugMessage.h"

GHFile* GHBBFileOpener::openPlatformFile(const char* filename, GHFile::FileType fileType, GHFile::FileMode fileMode) const
{	
	char dirBuff[1024];
	::snprintf(dirBuff, 1024, "data/%s", filename);
	const char* flags = "r";
	if (fileMode == GHFile::FM_WRITEONLY) {
		if (fileType == GHFile::FT_BINARY) {
			flags = "wb";
		}
		else {
			flags = "wt";
		}
	}
	else if (fileMode == GHFile::FM_READWRITE) {
		flags = "r+";
		if (fileType == GHFile::FT_BINARY) {
			flags = "r+b";
		}
	}

	FILE* file = fopen(dirBuff, flags);
	if (!file) {

		if (fileMode == GHFile::FM_READONLY) {
			::snprintf(dirBuff, 1024, "app/native/%s", filename);
			file = fopen(dirBuff, "r");
		}

		if (!file) {
			GHDebugMessage::outputString("Error %sing file %s as %s",
									 	 fileMode == GHFile::FM_READONLY ? "read" : "writ",
									     filename,
									     fileType == GHFile::FT_BINARY ? "binary" : "text");
			return 0;
		}
	}

	return new GHFileC(file, fileType, fileMode);
}
