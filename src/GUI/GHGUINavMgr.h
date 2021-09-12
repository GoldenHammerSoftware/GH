// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include <vector>
#include "Base/GHKeyDef.h"
#include "GHMath/GHPoint.h"
#include "GHMath/GHRect.h"
#include "GHGUIWidget.h"
#include "Base/GHInputState.h"
#include "Base/GHActionClaim.h"

class GHGUIButton;
class GHGUIWidget;
class GHInputClaim;

//Handles navigating between GUI elements using a gamepad
class GHGUINavMgr : public GHController
{
public:
	GHGUINavMgr(const GHInputState& inputState, GHInputClaim& inputClaim);
	~GHGUINavMgr(void);

	virtual void onActivate(void);
	virtual void onDeactivate(void);
	virtual void update(void);

	void registerElement(GHGUIButton& element);
	void unregisterElement(GHGUIButton& element);

	void setCursorWidget(GHGUIWidgetResource* cursor);

	bool select(const GHIdentifier& id);
	bool select(const GHGUIWidget& widget);
	bool isActive(void) const;
	const GHGUIWidget* getCurrentSelection(void) const;
	void setActive(void);
	void setInactive(void);

	void excludeGamepadType(GHInputStructs::Gamepad::Type type);

	GHActionClaim* getActionClaim(void) { return &mDefaultActionClaim; }

private:
	bool select(int index);
	void deselectCurrentButton(void);
	void selectHighestPriority(void);
	void activateCurrentButtonSelection(void);
	int findIndexOfNextButton(const GHPoint2i& nav);
	int findIndexOfNextButton(int dimension, int direction);
	void calculateCenter(const GHRect<float, 2>& rect, GHPoint2& outCenter);
	float calculateOverlap(const GHRect<float, 2>& currentPos, const GHRect<float, 2>& otherPos, int dimension);
	float calculateDistance(const GHRect<float, 2>& currentPos, const GHRect<float, 2>& otherPos, int dimension);
	void setCursor(GHGUIWidget& target);
	void unsetCursor(GHGUIWidget& target);
	void printButtonPosDescs(void) const;
	bool handleNavigationEvent(GHGUIButton& element, const GHPoint2i& navDir);

private:
	class NavInputMgr
	{
	public:
		NavInputMgr(const GHInputState& inputState);

		void getNavInput(GHPoint2i& nav);
		bool wasTriggerAttempted(void) const;
		bool wasNavigationSuperceded(void) const;
		void onNavActivation(void);

		void excludeGamepadType(GHInputStructs::Gamepad::Type type);

	private:
		void checkJoystick(int gamepadIndex, int joystickIndex, int dimension, float value, bool& negDir, bool& posDir);
		void handleJoystickChange(float value, bool& negDir, bool& posDir);
		bool isExcluded(const GHInputStructs::Gamepad& gamepad) const;
	private:
		const GHInputState& mInputState;
		struct JoystickId
		{
			int mGamepadIndex;
			int mJoystickIndex;
			int mDimension; 
			JoystickId(int gamepadIndex, int joystickIndex, int dimension)
				: mGamepadIndex(gamepadIndex), mJoystickIndex(joystickIndex), mDimension(dimension)
			{ }

			bool operator <(const JoystickId& other) const
			{
				if (mGamepadIndex == other.mGamepadIndex) {
					if (mJoystickIndex == other.mJoystickIndex) {
						return mDimension < other.mDimension;
					}
					return mJoystickIndex < other.mJoystickIndex;
				}
				return mGamepadIndex < other.mGamepadIndex;
			}
		};
		typedef std::map<JoystickId, float> JoyMap;
		JoyMap mJoystickPositions;
		typedef std::vector<GHInputStructs::Gamepad::Type> GamepadTypeList;
		GamepadTypeList mExcludedGamepadTypes;
		GHPoint2 mMousePosOnActivate;
	};
	NavInputMgr mInputMgr;

private:
	//For now, all guis share a single action claim. Potential todo: map of action claims by GHIdentifier (still with a default action claim) 
	GHActionClaim mDefaultActionClaim;
	GHInputClaim& mInputClaim;
	const GHInputState& mInputState;
	GHGUIWidgetResource* mCursor;
	typedef std::vector<GHGUIButton*> ElementList;
	ElementList mElements;
	int mCurrentActiveIndex;
	bool mActive;
};
