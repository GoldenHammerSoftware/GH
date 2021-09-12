#pragma once

#include "GHUtils/GHController.h"
#include "OVR_Avatar.h"
#include <vector>
#include "GHDebugDraw.h"
#include "GHOculusAvatarUtil.h"

class GHInputState;
class GHXMLSerializer;
class GHModel; 

class GHOculusAvatarCustomGestureFinder : public GHController
{
public:
	GHOculusAvatarCustomGestureFinder(const GHInputState& inputState,
									  const GHXMLSerializer& xmlSerializer,
									  GHModel*& avatarModel,
									  ovrAvatar*& avatar);


	virtual void onActivate(void);
	virtual void onDeactivate(void);
	virtual void update(void);

private:
	bool initialize(void);
	void save(void);
	void load(void);
	void initJointBuffersFromHand(void);
	void handleInput(void);
	void selectBone(unsigned int key, unsigned int fingerStartBoneIndex);
	void handleKeyboardRotations(void);
	void handleTouchControllerRotations(void);
	void cycleBoneOnCurrentFinger(void);
	void cycleFinger(void);
	void toggleModifyMode(void);
	void printCurrentBoneIndex(void);
	void updateDebugCross(void);
	void modifyBone(unsigned int dimension, float direction);
	void modifyTransform(unsigned int dimension, float direction, GHTransform& inOutTransform);
	void calcCurrentWorldSpaceTransform(GHTransform& outTransform);
	void calcBoneModelSpaceTransform(unsigned int boneIndex, GHTransform& outTransform);
	void handleUndo(void);
	void loadFromOppositeHand(const char* filename);

private:
	GHOculusUtil::AvatarComponentType mComponentToModify;
	const GHInputState& mInputState;
	const GHXMLSerializer& mXMLSerializer;
	GHModel*& mAvatarModel;
	DebugCross mDebugCross;
	bool mDebugCrossActive { true };
	
	enum ModifyMode
	{
		MODIFY_ROTATION,
		MODIFY_MID_ROTATION,
		MODIFY_POSITION,
		MODIFY_MID_POSITION,
		MODIFY_MAX
	};

	ModifyMode mModifyMode { MODIFY_ROTATION }; 
	bool mChangeHappenedThisFrame { false }; //used for determining whether to save a state on the undo stack
	bool mChangeHappenedLastFrame { false };
	ovrAvatar*& mAvatar;

	std::vector<ovrAvatarTransform> mOriginalJoints;
	std::vector<ovrAvatarTransform> mJoints;

	struct Change
	{
		ovrAvatarTransform mOriginalTransform;
		ovrAvatarTransform& mTransformRef;
		
		Change(ovrAvatarTransform& transform)
			: mOriginalTransform(transform), mTransformRef(transform) {  }
	};
	std::vector<Change> mUndoStack;

	bool mInitialized;

	unsigned int mCurrentBoneIndex;
};
