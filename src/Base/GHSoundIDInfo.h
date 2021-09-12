// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHString.h"

// class to wrap id3 info for a sound
class GHSoundIDInfo
{
public:
	void setArtistName(const char* name) { mArtistName.setConstChars(name, GHString::CHT_COPY); }
	const char* getArtistName(void) const { return mArtistName.getChars(); }

	void setSongName(const char* name) { mSongName.setConstChars(name, GHString::CHT_COPY); }
	const char* getSongName(void) const { return mSongName.getChars(); }

	void setAlbumName(const char* name) { mAlbumName.setConstChars(name, GHString::CHT_COPY); }
	const char* getAlbumName(void) const { return mAlbumName.getChars(); }

	void setPictureName(const char* name) { mPictureName.setConstChars(name, GHString::CHT_COPY); }
	const char* getPictureName(void) const { return mPictureName.getChars(); }

private:
	GHString mArtistName;
	GHString mSongName;
	GHString mAlbumName;
	GHString mPictureName;
};
