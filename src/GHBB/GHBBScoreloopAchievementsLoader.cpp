// Copyright 2010 Golden Hammer Software
#include "GHBBScoreloopAchievementsLoader.h"
#include "GHBBFileOpener.h"
#include "GHFile.h"
#include "GHAchievementsData.h"
#include "GHDebugMessage.h"

void GHBBScoreloopAchievementsLoader::loadAchievementsFromFile(GHAchievementsData& outData)
{
	GHBBFileOpener fileOpener;
	GHFile* file = fileOpener.openFile("scoreloop/SLAwards.bundle/en.lproj/Localizable.strings", GHFile::FT_TEXT, GHFile::FM_READONLY);
	if (!file) {
		GHDebugMessage::outputString("failed to load awards from file.");
		return;
	}

	file->readIntoBuffer();
	char* fileBuffer = 0;
	size_t fileSize = 0;
	file->getFileBuffer(fileBuffer, fileSize);

	char* prevLine = 0;
	char* line = 0;
	do
	{
		line = readLine(fileBuffer);
		if (line && prevLine) {
			parseLines(prevLine, line, outData);
			prevLine = 0;
		}
		else prevLine = line;
	}while(line);

	delete file;
}

char* GHBBScoreloopAchievementsLoader::readLine(char*& buffer)
{
	char* ret = buffer;

	while(*buffer != 0 && *buffer != '\n') {
		++buffer;
	}

	if (buffer - ret == 0) {
		return 0;
	}

	*buffer = 0;
	++buffer;

	return ret;
}

void GHBBScoreloopAchievementsLoader::parseLines(char* nameLine, char* descLine, GHAchievementsData& outData)
{
	char* id = popIDFromLine(nameLine);
	char* name = parseLine(nameLine);
	char* desc = parseLine(descLine);

	if (!name || !desc || !id)
	{
		GHDebugMessage::outputString("Failed to parse lines for an achievement.");
		return;
	}

	GHAchievementsData::Achievement achievement;
	achievement.mID.setConstChars(id, GHString::CHT_COPY);
	achievement.mName.setConstChars(name, GHString::CHT_COPY);
	achievement.mDescription.setConstChars(desc, GHString::CHT_COPY);
	achievement.mIsAchieved = false; //we'll get this from scoreloop later
	outData.addAchievement(achievement);
}

char* GHBBScoreloopAchievementsLoader::popIDFromLine(char*& line)
{
	//find the first '.'
	while (*line && *line != '.') {
		++line;
	}

	//If we've reached the end of the string, we haven't found the ID,
	// increment an extra time to get just past the '.' and check for null-temination again
	if (!*line || !*++line) {
		return 0;
	}

	//This is the beginning of the ID
	char* ret = line;

	//now find the next '.'
	// This will be the end of the ID
	while(*line && *line != '.') {
		++line;
	}

	//Null-terminate the ID and point line at the next character
	*line++ = 0;

	return ret;
}

char* GHBBScoreloopAchievementsLoader::parseLine(char* line)
{
	//Find the end of the line
	char* ptr = line;
	while (*ptr) {
		++ptr;
	}

	//Find the last quotation in the line, from the end.
	while (*ptr != '\"' && ptr > line) {
		--ptr;
	}

	//This is an error, shouldn't happen with good data, but let's be safe.
	if (ptr == line) {
		return 0;
	}

	//Null-terminate the last quotation.
	*ptr = 0;

	//Find the previous quotation. This will mark the beginning of our string.
	while (*ptr != '\"' && ptr > line) {
			--ptr;
	}

	//This is an error, shouldn't happen with good data, but let's be safe.
	if (ptr == line) {
		return 0;
	}

	//Our string starts immediately after the second-to-last quotation in the line
	return ptr+1;
}
