// Copyright Golden Hammer Software
#include "GHConsoleCommandStringHandler.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHPlatform/GHTimeVal.h"
#include "GHInputState.h"
#include "GHKeyDef.h"

static const char* kPromptStr = "> ";
const size_t GHConsoleCommandStringHandler::kPromptStrLen = strlen(kPromptStr);

GHConsoleCommandStringHandler::GHConsoleCommandStringHandler(const GHInputState& inputState,
	GHPropertyContainer& appProperties,
	const GHIdentifier& consoleTextProp,
	const GHIdentifier& consoleResultProp,
	const GHTimeVal& timeVal,
	CommandParser* commandParser,
	size_t consoleStringBufferSize,
	size_t consoleResultBufferSize)
	: mInputState(inputState)
	, mAppProperties(appProperties)
	, mConsoleTextProp(consoleTextProp)
	, mConsoleResultProp(consoleResultProp)
	, mTimeVal(timeVal)
	, mCommandParser(commandParser)
	, mBufSz(consoleStringBufferSize)
	, mResBufSz(consoleResultBufferSize)
	, mCurrentChar(0)
	, mCursorVisible(false)
	, mLastCursorChangeTime(0)
	, mLastKey(0)
	, mLastKeyDownTime(-1)
	, mLastRepetitionTime(-1)
	, mCursorEnabled(true)
{
	mBufSz += kPromptStrLen + 1 + 1; //extra +1 for cursor character
	mConsoleStringBuf = new char[mBufSz];
	mConsoleResultBuf = new char[mResBufSz];
	mAppProperties.setProperty(mConsoleResultProp, mConsoleResultBuf);
	mConsoleResultBuf[0] = 0;
}

GHConsoleCommandStringHandler::~GHConsoleCommandStringHandler(void)
{
	mAppProperties.setProperty(mConsoleResultProp, 0);
	delete[] mConsoleResultBuf;
	delete[] mConsoleStringBuf;
	delete mCommandParser;
}

void GHConsoleCommandStringHandler::update(void)
{
	const GHInputStructs::InputEventList& inputEvents = mInputState.getInputEvents();
	size_t numEvents = inputEvents.size();
	for (size_t i = 0; i < numEvents; ++i)
	{
		const GHInputStructs::InputEvent& event = inputEvents[i];
		if (event.mType == GHInputStructs::IET_KEYCHANGE
			&& event.mVal[1] != 0)
		{
			int key = (int)event.mVal[0];
			if (isAccepted(key))
			{
				handleKey(key);
				mLastKey = key;
				mLastKeyDownTime = mTimeVal.getTime();
				mLastRepetitionTime = -1;
			}
		}
	}
	handleRepetition();
	appendCursor();
}

void GHConsoleCommandStringHandler::onActivate(void)
{
	mAppProperties.setProperty(mConsoleTextProp, mConsoleStringBuf);
	strncpy(mConsoleStringBuf, kPromptStr, kPromptStrLen + 1);
	mCurrentChar = kPromptStrLen;
	mConsoleStringBuf[mCurrentChar] = 0;

	mCursorVisible = true;
	mLastCursorChangeTime = mTimeVal.getTime();
}

void GHConsoleCommandStringHandler::onDeactivate(void)
{
	mConsoleStringBuf[mCurrentChar] = 0;

	if (mCurrentChar > kPromptStrLen) {
		if (mCommandParser) {
			mCommandParser->parseCommand(&mConsoleStringBuf[kPromptStrLen], mConsoleResultBuf, mResBufSz);
		}
	}

	mAppProperties.setProperty(mConsoleResultProp, mConsoleResultBuf);
	mAppProperties.setProperty(mConsoleTextProp, 0);
	mLastKey = 0;
	mLastKeyDownTime = -1;
}

bool GHConsoleCommandStringHandler::isAccepted(int key) const
{
	if ((key >= 'a' && key <= 'z')
		|| (key >= 'A' && key <= 'Z')
		|| (key >= '0' && key <= '9'))
	{
		return true;
	}

	int allowedKeys[] = {
		GHKeyDef::KEY_BACKSPACE,
		' ',
		'=',
		'_',
		'-',
		'.',
		',',
	};

	for (int i = 0; i < sizeof(allowedKeys) / sizeof(allowedKeys[0]); ++i)
	{
		if (key == allowedKeys[i]) {
			return true;
		}
	}

	return false;
}

void GHConsoleCommandStringHandler::eraseLastCharacter(void)
{
	if (mCurrentChar <= 2) {
		return;
	}

	--mCurrentChar;
}

void GHConsoleCommandStringHandler::handleKey(int key)
{
	if (key == GHKeyDef::KEY_BACKSPACE) {
		eraseLastCharacter();
		return;
	}

	if (mCurrentChar >= mBufSz) {
		return;
	}

	mConsoleStringBuf[mCurrentChar] = key;
	++mCurrentChar;
}

void GHConsoleCommandStringHandler::handleRepetition(void)
{
	if (mLastKeyDownTime == -1) {
		return;
	}

	if (!mInputState.getKeyState(0, mLastKey))
	{
		mLastKeyDownTime = -1;
		return;
	}

	const float kInitialLag = .5f;
	const float kTimeBetweenReps = .1f;

	float currTime = mTimeVal.getTime();

	if (currTime - mLastKeyDownTime > kInitialLag)
	{
		if (mLastRepetitionTime == -1
			|| currTime - mLastRepetitionTime > kTimeBetweenReps)
		{
			mLastRepetitionTime = mTimeVal.getTime();
			handleKey(mLastKey);
		}
	}
}

void GHConsoleCommandStringHandler::setCursorEnabled(bool enable)
{
	if (mCursorEnabled && !enable)
	{
		if (mCursorVisible)
		{
			mConsoleStringBuf[mCurrentChar] = 0;
		}
		mCursorVisible = false;
		mLastCursorChangeTime = 0;
	}

	mCursorEnabled = enable;
}

void GHConsoleCommandStringHandler::appendCursor(void)
{
	if (!mCursorEnabled) {
		return;
	}

	if (mTimeVal.getTime() - mLastCursorChangeTime > .5f)
	{
		mLastCursorChangeTime = mTimeVal.getTime();
		mCursorVisible = !mCursorVisible;
	}

	if (mCursorVisible) {
		snprintf(&mConsoleStringBuf[mCurrentChar], mBufSz - mCurrentChar, "|");
	}
	else {
		mConsoleStringBuf[mCurrentChar] = 0;
	}
}
