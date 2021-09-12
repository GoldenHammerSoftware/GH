// Copyright 2010 Golden Hammer Software
#pragma once

class GHAchievementsData;

//Parses the English localization file in SLAwards.bundle
// and uses that data to fill the Achivements data list.
class GHBBScoreloopAchievementsLoader
{
public:
	void loadAchievementsFromFile(GHAchievementsData& outData);

private:
	//Read a \n-delimited line, null-terminate it
	char* readLine(char*& buffer);

	//convert the lines that contain the name and description data into
	// real strings, push an achievement data to the AchievementsData structure
	void parseLines(char* nameLine, char* descLine, GHAchievementsData& outData);

	//Read the achievement ID from the line, null-terminate and return the beginning
	// of the achievement ID. Set the line ptr to just past where the achievement ID was.
	//
	//Call this before calling parseLine on the same line
	char* popIDFromLine(char*& line);

	//Read the final " in the line, set the null termination there,
	// and return a pointer to just after the previous ". This is how the
	// lines look in the scoreloop achievement data for both names and descriptions.
	char* parseLine(char* line);
};

