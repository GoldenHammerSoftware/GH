// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include "GHString/GHIdentifier.h"
#include "GHString/GHString.h"

class GHInputState;
class GHPropertyContainer;
class GHTimeVal;

//Handles keyboard input for a console input string. Partially handles the string parsing.

//Derive a class from CommandParser to handle string commands.

class GHConsoleCommandStringHandler : public GHController
{
public:
	class CommandParser
	{
	public:
		virtual ~CommandParser(void) { }
		virtual void parseCommand(const char* params, char* resultBuffer, size_t resultBufLen) = 0;
	};

public:
	GHConsoleCommandStringHandler(const GHInputState& inputState,
								  GHPropertyContainer& appProperties,
								  const GHIdentifier& consoleTextProp, 
								  const GHIdentifier& consoleResultProp,
								  const GHTimeVal& timeVal,
								  CommandParser* commandParser, //GHConsoleCommandStringHandler takes ownership of this
								  size_t consoleStringBufferSize = 256,
								  size_t consoleResultBufferSize = 256); 
	~GHConsoleCommandStringHandler(void);

	virtual void update(void);
	virtual void onActivate(void);
	virtual void onDeactivate(void);

	static const size_t kPromptStrLen;
	void setCursorEnabled(bool enable);

private:
	bool isAccepted(int key) const;
	void eraseLastCharacter(void);
	void handleKey(int key);
	void handleRepetition(void);
	void appendCursor(void);

private:
	const GHInputState& mInputState;
	GHPropertyContainer& mAppProperties;
	GHIdentifier mConsoleTextProp;
	GHIdentifier mConsoleResultProp;
	const GHTimeVal& mTimeVal;
	CommandParser* mCommandParser;
	size_t mBufSz;
	size_t mResBufSz;
	char* mConsoleStringBuf;
	char* mConsoleResultBuf;

	size_t mCurrentChar;
	bool mCursorEnabled;
	bool mCursorVisible;
	float mLastCursorChangeTime;
	int mLastKey;
	float mLastKeyDownTime;
	float mLastRepetitionTime;
};
