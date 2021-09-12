// Copyright Golden Hammer Software
#include "GHGUINavMgr.h"
#include "GHInputState.h"
#include "GHKeyDef.h"
#include "GHMath/GHFloat.h"
#include "GHMath/GHMath.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGUIButton.h"
#include "GHFillType.h"
#include <float.h>
#include "GHGUINavResponse.h"
#include "GHInputClaim.h"
#include <algorithm>

GHGUINavMgr::GHGUINavMgr(const GHInputState& inputState, GHInputClaim& inputClaim)
: mInputClaim(inputClaim)
, mDefaultActionClaim(3)
, mInputState(inputState)
, mCurrentActiveIndex(-1)
, mInputMgr(inputState)
, mCursor(0)
, mActive(false)
{
	
}

GHGUINavMgr::~GHGUINavMgr(void)
{
	if (mCursor) mCursor->release();
}

void GHGUINavMgr::onActivate(void)
{
	mCurrentActiveIndex = -1;
	setInactive();
}

void GHGUINavMgr::onDeactivate(void)
{
	deselectCurrentButton();
}

void GHGUINavMgr::update(void)
{
	mDefaultActionClaim.clearClaim();
	printButtonPosDescs();
	if (!mElements.size()) {
		return;
	}

	GHPoint2i nav;
	mInputMgr.getNavInput(nav);

	if (!mActive)
	{
		if (nav[0] || nav[1] || mInputMgr.wasTriggerAttempted())
		{
			setActive();
		}
		return;
	}
	else
	{
		if (mInputMgr.wasNavigationSuperceded())
		{
			setInactive();
		}
		else
		{
			setActive();
		}
	}

	if (nav[0] || nav[1])
	{
		//Handle case where nothing is currently selected.
		// Select a default starting button.
		if (mCurrentActiveIndex < 0) 
		{
			selectHighestPriority();
		}
		else
		{
			if (!handleNavigationEvent(*mElements[mCurrentActiveIndex], nav))
			{
				int nextButton = findIndexOfNextButton(nav);
				if (nextButton >= 0)
				{
					select(nextButton);
				}
			}
		}
	}

	if (mInputMgr.wasTriggerAttempted())
	{
		if (mCurrentActiveIndex >= 0)
		{
			mElements[mCurrentActiveIndex]->getNavigator().trigger();
		}
	}
}

bool GHGUINavMgr::select(const GHIdentifier& id)
{
	unsigned int numButtons = (unsigned int)mElements.size();
	for (unsigned int i = 0; i < numButtons; ++i)
	{
		if (id == mElements[i]->getId())
		{
			if (i != (unsigned int)mCurrentActiveIndex)
			{
				return select(i);
			}
			return true;
		}
	}
	return false;
}

bool GHGUINavMgr::select(const GHGUIWidget& widget)
{
	unsigned int numButtons = (unsigned int)mElements.size();
	for (unsigned int i = 0; i < numButtons; ++i)
	{
		if (&widget == mElements[i])
		{
			if (i != (size_t)mCurrentActiveIndex)
			{
				return select(i);
			}
			return true;
		}
	}
	return false;
}

void GHGUINavMgr::selectHighestPriority(void)
{
	unsigned int numButtons = (unsigned int)mElements.size();
	if (!numButtons) { return; }

	unsigned int highestPriIndex = 0;
	float highestPri = mElements[0]->getNavigator().getPriority();

	for (unsigned int i = 1; i < numButtons; ++i)
	{
		float pri = mElements[i]->getNavigator().getPriority();
		if (pri > highestPri)
		{
			highestPri = pri;
			highestPriIndex = i;
		}
	}

	select(highestPriIndex);
}

bool GHGUINavMgr::select(int index)
{
	if (mCurrentActiveIndex == index) {
		return true;
	}

	deselectCurrentButton();
	mCurrentActiveIndex = index;

	if (mCurrentActiveIndex < 0)
	{
		setInactive();
		return false;
	}

	if (mActive)
	{
		activateCurrentButtonSelection();
	}

	return true;
}

void GHGUINavMgr::deselectCurrentButton(void)
{
	if (mCurrentActiveIndex < 0) {
		return;
	}

	unsetCursor(*mElements[mCurrentActiveIndex]);
	mElements[mCurrentActiveIndex]->getNavigator().setNavSelected(false);
	mCurrentActiveIndex = -1;
}

void GHGUINavMgr::activateCurrentButtonSelection(void)
{
	if (mCurrentActiveIndex >= 0)
	{
		mElements[mCurrentActiveIndex]->getNavigator().setNavSelected(true);
		setCursor(*mElements[mCurrentActiveIndex]);
		GHGUINavResponse* selectResponse = mElements[mCurrentActiveIndex]->getNavigator().mSelectResponse;
		if (selectResponse)
		{
			selectResponse->respondToNavigation(GHPoint2i());
		}
	}
}

bool GHGUINavMgr::handleNavigationEvent(GHGUIButton& element, const GHPoint2i& navDir)
{
	GHGUINavResponse* navResponse = 0;
	if (navDir[1] == -1) {
		navResponse = element.getNavigator().mUpResponse;
	}
	else if (navDir[1] == 1) {
		navResponse = element.getNavigator().mDownResponse;
	}
	else if (navDir[0] == -1) {
		navResponse = element.getNavigator().mLeftResponse;
	}
	else if (navDir[0] == 1) {
		navResponse = element.getNavigator().mRightResponse;
	}

	if (navResponse)
	{
		if (navResponse->respondToNavigation(navDir)) {
			return true;
		}
	}

	return false;
}

int GHGUINavMgr::findIndexOfNextButton(const GHPoint2i& nav)
{
	if (nav[0])
	{
		return findIndexOfNextButton(0, nav[0]);
	}
	else
	{
		return findIndexOfNextButton(1, nav[1]);
	}
}

int GHGUINavMgr::findIndexOfNextButton(int dimension, int direction)
{
	if (direction == 0) {
		return -1;
	}

	int perpDimension = (dimension + 1) % 2;

	const GHRect<float, 2>& currentPos = mElements[mCurrentActiveIndex]->getScreenPos();
	GHPoint2 currentCenter;
	calculateCenter(currentPos, currentCenter);

	struct Option
	{
		Option(float perpOverlap, float distToRect, float perpDist, int index)
		: mPerpOverlap(perpOverlap), mDistToRect(distToRect), mPerpDist(perpDist), mIndex(index) { }

		float mPerpOverlap;
		float mDistToRect;
		float mPerpDist;
		int mIndex;

		//returns true if we are the better option
		bool operator<(const Option& other)
		{
			if (GHFloat::isZero(mPerpOverlap) && !GHFloat::isZero(other.mPerpOverlap))
			{
				return false;
			}

			if (!GHFloat::isZero(mPerpOverlap) && GHFloat::isZero(other.mPerpOverlap))
			{
				return true;
			}

			if (GHFloat::isZero(mPerpOverlap) && GHFloat::isZero(other.mPerpOverlap))
			{
				if (!GHFloat::isEqual(mPerpDist, other.mPerpDist))
				{
					return mPerpDist < other.mPerpDist;
				}
			}


			if (!GHFloat::isEqual(mDistToRect, other.mDistToRect))
			{
				return mDistToRect < other.mDistToRect;
			}

			return mPerpOverlap > other.mPerpOverlap;
		}
	};

	Option bestOption(0, FLT_MAX, FLT_MAX, -1);

	size_t numButtons = mElements.size();
	for (int i = 0; i < (int)numButtons; ++i)
	{
		if (i == mCurrentActiveIndex) {
			continue;
		}

		const GHRect<float, 2>& otherPos = mElements[i]->getScreenPos();
		GHPoint2 otherCenter;
		calculateCenter(otherPos, otherCenter);
		
		float diff = otherCenter[dimension] - currentCenter[dimension];
		if (GHFloat::sign(diff) == (float)direction)
		{
			//calc overlap
			float perpOverlap = calculateOverlap(currentPos, otherPos, perpDimension);
			float distToRect = calculateDistance(currentPos, otherPos, dimension);
			float perpDist = calculateDistance(currentPos, otherPos, perpDimension);

			Option option(perpOverlap, distToRect, perpDist, i);
			//GHDebugMessage::outputString("%s perpOverlap: %f distToRect: %f", mElements[i]->getId().getString(), option.mPerpOverlap, option.mDistToRect);

			if (option < bestOption)
			{
				bestOption = option;
			}
		}
	}

	return bestOption.mIndex;
}

void GHGUINavMgr::calculateCenter(const GHRect<float, 2>& rect, GHPoint2& outCenter)
{
	outCenter = rect.mMax;
	outCenter -= rect.mMin;
	outCenter *= .5f;
	outCenter += rect.mMin;
}

float GHGUINavMgr::calculateOverlap(const GHRect<float, 2>& currentPos, const GHRect<float, 2>& otherPos, int dimension)
{
	float ret = 0;
	{
		if (otherPos.mMin[dimension] <= currentPos.mMin[dimension])
		{
			if (otherPos.mMax[dimension] >= currentPos.mMax[dimension])
			{
				ret = currentPos.mMax[dimension] - currentPos.mMin[dimension];
			}
			else
			{
				ret = otherPos.mMax[dimension] - currentPos.mMin[dimension];
			}
		}
		else if (otherPos.mMin[dimension] > currentPos.mMax[dimension])
		{
			ret = 0;
		}
		else
		{
			if (otherPos.mMax[dimension] <= currentPos.mMax[dimension])
			{
				ret = otherPos.mMax[dimension] - currentPos.mMin[dimension];
			}
			else
			{
				ret = currentPos.mMax[dimension] - otherPos.mMin[dimension];
			}
		}
	}

	if (ret < 0) {
		ret = 0;
	}

	return ret;
}

float GHGUINavMgr::calculateDistance(const GHRect<float, 2>& currentPos, const GHRect<float, 2>& otherPos, int dimension)
{
	/*
	int perpDimension = (dimension + 1) % 2;
	
	//there is no line parallel to the requested dimension which intersects both rects
	if (currentPos.mMin[perpDimension] > otherPos.mMax[perpDimension]
		|| currentPos.mMax[perpDimension] < otherPos.mMin[perpDimension])
	{
		return FLT_MAX;
	}
	*/

	//curr rect is above other rect
	if (currentPos.mMin[dimension] >= otherPos.mMax[dimension])
	{
		return currentPos.mMin[dimension] - otherPos.mMax[dimension];
	}

	//curr rect is below other rect
	if (currentPos.mMax[dimension] <= otherPos.mMin[dimension])
	{
		return otherPos.mMin[dimension] - currentPos.mMax[dimension];
	}

	//the rects are overlapping
	return 0;
}

void GHGUINavMgr::registerElement(GHGUIButton& element)
{
	mElements.push_back(&element);
}

void GHGUINavMgr::unregisterElement(GHGUIButton& element)
{
	size_t numButtons = mElements.size();
	for (int i = 0; i < (int)numButtons; ++i)
	{
		if (&element == mElements[i])
		{
			if (i == mCurrentActiveIndex)
			{
				deselectCurrentButton();
				setInactive();
			}
			else if (i < mCurrentActiveIndex)
			{
				--mCurrentActiveIndex;
			}

			mElements.erase(mElements.begin() + i);
			return;
		}
	}
}

void GHGUINavMgr::excludeGamepadType(GHInputStructs::Gamepad::Type type)
{
	mInputMgr.excludeGamepadType(type);
}

void GHGUINavMgr::NavInputMgr::excludeGamepadType(GHInputStructs::Gamepad::Type type)
{
	mExcludedGamepadTypes.push_back(type);
}

bool GHGUINavMgr::NavInputMgr::isExcluded(const GHInputStructs::Gamepad& gamepad) const
{
	return std::find(mExcludedGamepadTypes.begin(), mExcludedGamepadTypes.end(), gamepad.mType) != mExcludedGamepadTypes.end();
}

void GHGUINavMgr::setCursorWidget(GHGUIWidgetResource* cursor)
{
	if (mCursor) {
		if (mCurrentActiveIndex > 0) {
			mElements[mCurrentActiveIndex]->removeChild(cursor);
		}
		mCursor->release();
	}

	mCursor = cursor;
	if (mCursor)
	{
		mCursor->acquire();

		GHGUIPosDesc desc;
		desc.mAlign.setCoords(0.5, 0.5);
		desc.mOffset.setCoords(0, 0);
		desc.mXFill = GHFillType::FT_PCT;
		desc.mYFill = GHFillType::FT_PCT;
		desc.mSizeAlign[0] = GHAlign::A_CENTER;
		desc.mSizeAlign[1] = GHAlign::A_CENTER;
		desc.mSize.setCoords(1, 1);
		mCursor->get()->setPosDesc(desc);
		mCursor->get()->updatePosition();

		if (mCurrentActiveIndex > 0) {
			setCursor(*mElements[mCurrentActiveIndex]);
		}
	}
}

void GHGUINavMgr::setCursor(GHGUIWidget& target)
{
	if (!mCursor) { return; }
	target.addChild(mCursor);
}

void GHGUINavMgr::unsetCursor(GHGUIWidget& target)
{
	if (!mCursor) { return; }
	target.removeChild(mCursor);
}

void GHGUINavMgr::printButtonPosDescs(void) const
{
	//put a breakpoint here and set print to true in the watch window 
	// when you want to print the current set of buttons
	bool print = false;
	if (print)
	{
		for (int i = 0; i < (int)mElements.size(); ++i)
		{
			GHGUIButton& button = *mElements[i];
			GHGUIPosDesc posDesc = button.getPosDesc();

			const char* xFill = "FT_PIXELS", *yFill = "FT_PIXELS";
			if (posDesc.mXFill == GHFillType::FT_PCT) { xFill = "FT_PCT"; }
			if (posDesc.mYFill == GHFillType::FT_PCT) { yFill = "FT_PCT"; }

			GHDebugMessage::outputString("align=\"%f %f\" offset=\"%f %f\" size=\"%f %f\" sizeAlign=\"%f %f\" xFill=%s yFill=%s ",
				posDesc.mAlign[0], posDesc.mAlign[1],
				posDesc.mOffset[0], posDesc.mOffset[1],
				posDesc.mSize[0], posDesc.mSize[1],
				posDesc.mSizeAlign[0], posDesc.mSizeAlign[1],
				xFill, yFill);
		}
	}
}

void GHGUINavMgr::setActive(void)
{
	for (int i = 0; i < (int)mInputState.getNumPointers(); ++i) {
		mInputClaim.claimPointer(i, this);
	}
	if (!mActive) 
	{
		mActive = true;
		mInputMgr.onNavActivation();
		if (mCurrentActiveIndex < 0) {
			selectHighestPriority();
		}
		else {
			activateCurrentButtonSelection();
		}
	}
}

void GHGUINavMgr::setInactive(void)
{
	for (int i = 0; i < (int)mInputState.getNumPointers(); ++i) {
		mInputClaim.releasePointer(i, this);
	}
	if (mCurrentActiveIndex >= 0) {
		unsetCursor(*mElements[mCurrentActiveIndex]);
	}
	mActive = false;
}

bool GHGUINavMgr::isActive(void) const
{
	return mActive;
}

const GHGUIWidget* GHGUINavMgr::getCurrentSelection(void) const
{
	if (mCurrentActiveIndex < 0) {
		return 0;
	}
	return mElements[mCurrentActiveIndex];
}


GHGUINavMgr::NavInputMgr::NavInputMgr(const GHInputState& inputState)
: mInputState(inputState)
{

}

void GHGUINavMgr::NavInputMgr::getNavInput(GHPoint2i& nav)
{
	bool up = false, down = false, left = false, right = false;

	int numGamepads = (int)mInputState.getNumGamepads();
	for (int g = 0; g < numGamepads; ++g)
	{
		const GHInputStructs::Gamepad& gamepad = mInputState.getGamepad(g);
		if (!gamepad.mActive) {
			continue;
		}

		if (isExcluded(gamepad)) {
			continue;
		}

		up |= mInputState.checkGamepadKeyChange(g, GHKeyDef::KEY_GP_DPADUP, true);
		down |= mInputState.checkGamepadKeyChange(g, GHKeyDef::KEY_GP_DPADDOWN, true);
		left |= mInputState.checkGamepadKeyChange(g, GHKeyDef::KEY_GP_DPADLEFT, true);
		right |= mInputState.checkGamepadKeyChange(g, GHKeyDef::KEY_GP_DPADRIGHT, true);

		int numJoysticks = (int)gamepad.mJoySticks.size();
		for (int j = 0; j < numJoysticks; ++j)
		{
			checkJoystick(g, j, 0, gamepad.mJoySticks[j][0], left, right);
			checkJoystick(g, j, 1, gamepad.mJoySticks[j][1], down, up);
		}
	}

	nav[0] = (left ? -1 : 0) + (right ? 1 : 0);
	nav[1] = (down ? 1 : 0) + (up ? -1 : 0); //higher up in gui screen space is lower numbers

	//if (nav[0] != 0 || nav[1] != 0)
	//{
	//	GHDebugMessage::outputString("%s%s%s%s", up ? "up " : "", down ? "down " : "", left ? "left " : "", right ? "right " : "");
	//}
}

bool GHGUINavMgr::NavInputMgr::wasTriggerAttempted(void) const
{
	int numGamepads = (int)mInputState.getNumGamepads();
	for (int i = 0; i < numGamepads; ++i)
	{
		if (isExcluded(mInputState.getGamepad(i))) {
			continue;
		}

		if (mInputState.checkGamepadKeyChange(i, GHKeyDef::KEY_GP_A, true))
		{
			return true;
		}
	}
	return false;
}

void GHGUINavMgr::NavInputMgr::onNavActivation(void)
{
	mMousePosOnActivate = mInputState.getPointerPosition(0);
}

bool GHGUINavMgr::NavInputMgr::wasNavigationSuperceded(void) const
{
	GHPoint2 mouseDelta = mInputState.getPointerPosition(0);
	mouseDelta -= mMousePosOnActivate;
	return mouseDelta.lenSqr() > .001f;
}

void GHGUINavMgr::NavInputMgr::checkJoystick(int gamepadIndex, int joystickIndex, int dimension, float value, bool& negDir, bool& posDir)
{
	const float kDeadZoneThreshold = .5f;
	if (fabs(value) < kDeadZoneThreshold) {
		value = 0;
	}

	JoystickId id(gamepadIndex, joystickIndex, dimension);
	JoyMap::iterator iter = mJoystickPositions.find(id);
	if (iter == mJoystickPositions.end())
	{
		if (!GHFloat::isZero(value))
		{
			mJoystickPositions.insert(JoyMap::value_type(id, value));
			handleJoystickChange(value, negDir, posDir);
		}
	}
	else
	{
		float currentSign = GHFloat::sign(value);
		if (currentSign && currentSign != GHFloat::sign(iter->second))
		{
			handleJoystickChange(value, negDir, posDir);
		}
		iter->second = value;
	}
}

void GHGUINavMgr::NavInputMgr::handleJoystickChange(float value, bool& negDir, bool& posDir)
{
	negDir |= value < 0;
	posDir |= value > 0;
}
