#include "GHOculusAvatarCustomGestureFinder.h"
#include "GHInputState.h"
#include "GHXMLSerializer.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHKeyDef.h"
#include "GHMath/GHFloat.h"

#include "GHDebugDraw.h"
#include "GHModel.h"

enum LeftHandBoneIndex
{
	_l_hand_world,
	b_l_hand,
	b_l_grip,
	b_l_hand_ignore,
	b_l_index1,
	b_l_index2,
	b_l_index3,
	b_l_index_ignore,
	b_l_middle1,
	b_l_middle2,
	b_l_middle3,
	b_l_middle_ignore,
	b_l_pinky0,
	b_l_pinky1,
	b_l_pinky2,
	b_l_pinky3,
	b_l_pinky_ignore,
	b_l_ring1,
	b_l_ring2,
	b_l_ring3,
	b_l_ring_ignore,
	b_l_thumb1,
	b_l_thumb2,
	b_l_thumb3,
	b_l_thumb_ignore,
	MAX_LEFT_BONES,
};

enum RightHandBoneIndex
{
	_r_hand_world,
	b_r_hand,		 
	b_r_grip,		 
	b_r_hand_ignore,
	b_r_index1,     
	b_r_index2,    
	b_r_index3,     
	b_r_index_ignore,
	b_r_middle1,
	b_r_middle2,
	b_r_middle3,
	b_r_middle_ignore,
	b_r_pinky0,
	b_r_pinky1,
	b_r_pinky2,
	b_r_pinky3,
	b_r_pinky_ignore,
	b_r_ring1,
	b_r_ring2,
	b_r_ring3,
	b_r_ring_ignore,
	b_r_thumb1,
	b_r_thumb2,
	b_r_thumb3,
	b_r_thumb_ignore,
	MAX_RIGHT_BONES
};


struct Finger
{
	LeftHandBoneIndex fingerStart;
	unsigned int numBonesInFinger;
};

const Finger fingers[] = 
{
	{ b_l_index1, 3 },
	{ b_l_middle1, 3 },
	{ b_l_ring1, 3 },
	{ b_l_pinky0, 4 },
	{ b_l_thumb1, 3 }
};

GHOculusAvatarCustomGestureFinder::GHOculusAvatarCustomGestureFinder(const GHInputState& inputState,
																	 const GHXMLSerializer& xmlSerializer,
																	 GHModel*& avatarModel,
																	 ovrAvatar*& avatar)
	: mInputState(inputState)
	, mXMLSerializer(xmlSerializer)
	, mAvatarModel(avatarModel)
	, mAvatar(avatar)
	, mInitialized(false)
	, mCurrentBoneIndex(0)
{
	mDebugCross.setSizes(.1f, .1f, .1f);
	//mComponentToModify = GHOculusUtil::LEFTHAND;
	mComponentToModify = GHOculusUtil::RIGHTHAND;
}


void GHOculusAvatarCustomGestureFinder::onActivate(void)
{

}

void GHOculusAvatarCustomGestureFinder::onDeactivate(void)
{

}

void GHOculusAvatarCustomGestureFinder::update(void)
{
	if (!initialize()) { return; }
	
	mChangeHappenedLastFrame = mChangeHappenedThisFrame;
	mChangeHappenedThisFrame = false;

	handleInput();

	if (mComponentToModify == GHOculusUtil::LEFTHAND)
	{
		ovrAvatar_SetLeftHandCustomGesture(mAvatar, (uint32_t)mJoints.size(), &mJoints.front());
	}
	else if(mComponentToModify == GHOculusUtil::RIGHTHAND)
	{
		ovrAvatar_SetRightHandCustomGesture(mAvatar, (uint32_t)mJoints.size(), &mJoints.front());
	}
	

	updateDebugCross();
}

bool GHOculusAvatarCustomGestureFinder::initialize(void)
{
	if (mInitialized) { return true; }
	if (!mAvatar) { return false; }

	initJointBuffersFromHand();
	//loadFromOppositeHand("../leftHandBallGrabGesture.xml");
	load();

	mInitialized = true;
	return true;
}

void GHOculusAvatarCustomGestureFinder::handleInput(void)
{
	selectBone('0', (mCurrentBoneIndex + 1) % MAX_LEFT_BONES);
	selectBone('1', b_l_index1);
	selectBone('2', b_l_middle1);
	selectBone('3', b_l_ring1);
	selectBone('4', b_l_pinky0);
	selectBone('5', b_l_thumb1);

	if (mInputState.checkKeyChange(0, '6', true)
		|| mInputState.checkGamepadKeyChange(1, GHKeyDef::KEY_GP_A, true))
	{
		cycleBoneOnCurrentFinger();
	}

	if (mInputState.checkGamepadKeyChange(1, GHKeyDef::KEY_GP_B, true))
	{
		cycleFinger();
	}

	if (mInputState.checkKeyChange(0, 's', true)
		|| mInputState.checkKeyChange(0, 'S', true))
	{
		save();
	}

	if (mInputState.checkKeyChange(0, 'r', true)
		|| mInputState.checkKeyChange(0, 'R', true))
	{
		memcpy(&mJoints.front(), &mOriginalJoints.front(), mJoints.size()*sizeof(ovrAvatarTransform));
		mUndoStack.clear();
	}

	if (mInputState.checkKeyChange(0, 'p', true)
		|| mInputState.checkKeyChange(0, 'P', true)
		|| (mInputState.checkGamepadKeyChange(1, GHKeyDef::KEY_GP_LEFTSTICKBUTTON, true)))
	{
		toggleModifyMode();
	}

	handleTouchControllerRotations();

	if (mInputState.checkKeyChange(0, GHKeyDef::KEY_BACKSPACE, true)
		|| mInputState.checkGamepadKeyChange(1, GHKeyDef::KEY_GP_X, true))
	{
		handleUndo();
	}

	if (mInputState.checkKeyChange(0, 'd', true)
		|| mInputState.checkKeyChange(0, 'D', true))
	{
		mDebugCrossActive = !mDebugCrossActive;
		if (!mDebugCrossActive)
		{
			mDebugCross.deactivate();
		}
	}
}

void GHOculusAvatarCustomGestureFinder::handleTouchControllerRotations(void)
{
	const GHInputStructs::Gamepad& gamepad = mInputState.getGamepad(1);
	modifyBone(0, gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_LEFTBUMPER));
	modifyBone(0, -gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_LEFTTRIGGER));
	
	modifyBone(1, gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_RIGHTBUMPER));
	modifyBone(1, -gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_RIGHTTRIGGER));
	
	modifyBone(2, gamepad.mJoySticks[1][1]);
}

void GHOculusAvatarCustomGestureFinder::handleKeyboardRotations(void)
{
#ifdef QWERTY
	unsigned int xRotPosKey = 'a';
	unsigned int xRotNegKey = 'z';
	unsigned int yRotPosKey = 's';
	unsigned int yRotNegKey = 'x';
	unsigned int zRotPosKey = 'd';
	unsigned int zRotNegKey = 'c';
#else
	unsigned int xRotPosKey = 'a';
	unsigned int xRotNegKey = ';';
	unsigned int yRotPosKey = 'o';
	unsigned int yRotNegKey = 'q';
	unsigned int zRotPosKey = 'e';
	unsigned int zRotNegKey = 'j';
#endif

	if (mInputState.getKeyState(0, xRotPosKey)) {
		modifyBone(0, 1);
	}
	if (mInputState.getKeyState(0, xRotNegKey)) {
		modifyBone(0, -1);
	}
	if (mInputState.getKeyState(0, yRotPosKey)) {
		modifyBone(1, 1);
	}
	if (mInputState.getKeyState(0, yRotNegKey)) {
		modifyBone(1, -1);
	}
	if (mInputState.getKeyState(0, zRotPosKey)) {
		modifyBone(2, 1);
	}
	if (mInputState.getKeyState(0, zRotNegKey)) {
		modifyBone(2, -1);
	}
}

void GHOculusAvatarCustomGestureFinder::selectBone(unsigned int key, unsigned int fingerStartBoneIndex)
{
	if (mInputState.checkKeyChange(0, key, true))
	{
		mCurrentBoneIndex = fingerStartBoneIndex;
		printCurrentBoneIndex();
	}
}

void GHOculusAvatarCustomGestureFinder::cycleBoneOnCurrentFinger(void)
{
	for (unsigned int f = 0; f < sizeof(fingers)/sizeof(Finger); ++f)
	{
		const Finger& finger = fingers[f];

		if (mCurrentBoneIndex >= (unsigned int)finger.fingerStart
			&& mCurrentBoneIndex < finger.fingerStart + finger.numBonesInFinger)
		{
			unsigned int diff = mCurrentBoneIndex - finger.fingerStart;
			diff = (diff + 1) % finger.numBonesInFinger;
			mCurrentBoneIndex = finger.fingerStart + diff;
			printCurrentBoneIndex();
			return;
		}
	}
}

void GHOculusAvatarCustomGestureFinder::cycleFinger(void)
{
	unsigned int fingerCount = sizeof(fingers)/sizeof(Finger);
	int currentFingerIndex = -1;
	int currentJointIndex = 0;
	for (unsigned int f = 0; f < fingerCount; ++f)
	{
		const Finger& finger = fingers[f];
		if (mCurrentBoneIndex >= ( unsigned int ) finger.fingerStart
			&& mCurrentBoneIndex < finger.fingerStart + finger.numBonesInFinger)
		{
			currentFingerIndex = f;
			currentJointIndex = mCurrentBoneIndex - finger.fingerStart;
			break;
		}
	}

	currentFingerIndex = (currentFingerIndex + 1) % fingerCount;
	const Finger& finger = fingers[currentFingerIndex];

	if (currentJointIndex >= (int)finger.numBonesInFinger)
	{
		currentJointIndex = finger.numBonesInFinger - 1;
	}

	mCurrentBoneIndex = finger.fingerStart + currentJointIndex;
	printCurrentBoneIndex();
}

void GHOculusAvatarCustomGestureFinder::toggleModifyMode(void)
{
	mModifyMode = static_cast<ModifyMode>((int(mModifyMode) + 1) % MODIFY_MAX);

	const char* modifyStr = "";
	switch (mModifyMode)
	{
		case MODIFY_ROTATION: modifyStr = "rotations"; break;
		case MODIFY_MID_ROTATION: modifyStr = "mid rotations"; break;
		case MODIFY_POSITION: modifyStr = "positions"; break;
		case MODIFY_MID_POSITION: modifyStr = "mid positions"; break;
	}

	GHDebugMessage::outputString("Now modifying %s", modifyStr);
}

void GHOculusAvatarCustomGestureFinder::printCurrentBoneIndex(void)
{
	const ovrAvatarSkinnedMeshPose* pose = GHOculusUtil::getSkinnedMeshPose(mAvatar, mComponentToModify);
	GHDebugMessage::outputString("Selected %s: ", pose->jointNames[ mCurrentBoneIndex ]);
}

void GHOculusAvatarCustomGestureFinder::modifyTransform(unsigned int dimension, float direction, GHTransform& inOutTransform)
{
	switch (mModifyMode)
	{
		case MODIFY_ROTATION:
		case MODIFY_MID_ROTATION:
		{
			GHTransform rot;
			if (mModifyMode == MODIFY_MID_ROTATION)
			{
				float angle = .008f;
				GHPoint3 dir;
				switch (dimension)
				{
					case 0: dir.setCoords(0,1,1); break;
					case 1: dir.setCoords(1,0,1); break;
					case 2: dir.setCoords(1,1,0); break;
				}

				dir.normalize();
				rot.becomeAxisAngleRotation(dir, direction*angle);
			}
			else
			{
				float angle = .01f;
				switch (dimension)
				{
					case 0: rot.becomeXRotation(direction*angle); break;
					case 1: rot.becomeYRotation(direction*angle); break;
					case 2: rot.becomeZRotation(direction*angle); break;
				}
			}
			
			GHPoint3 translate;
			inOutTransform.getTranslate(translate);
			inOutTransform.setTranslate(GHPoint3(0,0,0));
			inOutTransform.mult(rot, inOutTransform);
			inOutTransform.setTranslate(translate);
		}
		break;

		case MODIFY_POSITION:
		case MODIFY_MID_POSITION:
		{
			GHPoint3 translate;
			inOutTransform.getTranslate(translate);
			float amount = .001f;
			if (mModifyMode == MODIFY_MID_POSITION)
			{
				GHPoint3 dir;
				switch (dimension)
				{
					case 0: dir.setCoords(0,1,1); break;
					case 1: dir.setCoords(1,0,1); break;
					case 2: dir.setCoords(1,1,0); break;
				}

				dir.normalize();
				dir *= direction*amount;
				translate += dir;				
			}
			else
			{
				switch (dimension)
				{
					case 0: translate[0] += direction*amount; break;
					case 1: translate[1] += direction*amount; break;
					case 2: translate[2] += direction*amount; break;
				}
			}

			inOutTransform.setTranslate(translate);
		}
		break;
	}
}

void GHOculusAvatarCustomGestureFinder::modifyBone(unsigned int dimension, float direction)
{
	if (GHFloat::isZero(direction))
	{
		return;
	}

	if (!mChangeHappenedLastFrame && !mChangeHappenedThisFrame)
	{
		mUndoStack.push_back(Change(mJoints[mCurrentBoneIndex]));
	}

	GHTransform boneTrans;
	GHOculusUtil::convertOvrAvatarTransformToGH(mJoints[mCurrentBoneIndex], boneTrans);
	modifyTransform(dimension, direction, boneTrans);
	GHOculusUtil::convertGHTransformToOvrAvatar(boneTrans, mJoints[mCurrentBoneIndex]);
	
	mChangeHappenedThisFrame = true;
}

void GHOculusAvatarCustomGestureFinder::updateDebugCross(void)
{
	if (!mDebugCrossActive) { return; }

	GHTransform ghTransform;
	calcCurrentWorldSpaceTransform(ghTransform);
	mDebugCross.setTransform(ghTransform);

	GHPoint3 color;
	switch (mModifyMode)
	{
		case MODIFY_ROTATION: 
			mDebugCross.setColors(GHPoint3(1,0,0), GHPoint3(0,1,0), GHPoint3(0,0,1));
			break;
		case MODIFY_MID_ROTATION: 
			mDebugCross.setColors(GHPoint3(.7f,0,.7f), GHPoint3(.7f,.7f,0), GHPoint3(0,.7f,.7f));
			break;
		case MODIFY_POSITION: 
			mDebugCross.setColors(GHPoint3(1,1,0), GHPoint3(1,0,1), GHPoint3(0,1,0));
			break;
		case MODIFY_MID_POSITION: 
			mDebugCross.setColors(GHPoint3(1,.5f,.5), GHPoint3(.5f,.5f,1), GHPoint3(.5f,1,.5f));
			break;
	}
}
void GHOculusAvatarCustomGestureFinder::initJointBuffersFromHand(void)
{
	const ovrAvatarSkinnedMeshPose* meshPose = GHOculusUtil::getSkinnedMeshPose(mAvatar, mComponentToModify);
	mJoints.resize(meshPose->jointCount);
	memcpy(&mJoints.front(), meshPose->jointTransform, mJoints.size()*sizeof(ovrAvatarTransform));

	mOriginalJoints.resize(meshPose->jointCount);
	memcpy(&mOriginalJoints.front(), &mJoints.front(), mJoints.size()*sizeof(ovrAvatarTransform));

	mUndoStack.clear();
}

void GHOculusAvatarCustomGestureFinder::save(void)
{
	const char* saveFilename = "../oculusGestureTransforms.xml";
	GHDebugMessage::outputString("Saving Oculus Gesture Transforms to %s", saveFilename);

	GHOculusUtil::saveGesture(mAvatar, mComponentToModify, saveFilename, mXMLSerializer);
}

#include "GHMath/GHQuaternion.h"

void GHOculusAvatarCustomGestureFinder::load(void)
{
	const char* saveFilename = "../oculusGestureTransforms.xml";
	GHOculusUtil::loadGesture(mAvatar, mComponentToModify, saveFilename, mXMLSerializer, mJoints);
	mUndoStack.clear();
}

void GHOculusAvatarCustomGestureFinder::loadFromOppositeHand(const char* filename)
{
	GHOculusUtil::AvatarComponentType componentToLoad = mComponentToModify == GHOculusUtil::LEFTHAND ? GHOculusUtil::RIGHTHAND : GHOculusUtil::LEFTHAND;

	GHOculusUtil::loadGesture(mAvatar, componentToLoad, filename, mXMLSerializer, mJoints);

	for (int i = 0; i < mJoints.size(); ++i)
	{
		ovrAvatarTransform& transform = mJoints[i];

		//all bones have negative x components
		transform.position.x = -transform.position.x;

		//these bones specifically also have negative y and z components
		if (i == b_l_index1
			|| i == b_l_middle1
			|| i == b_l_pinky0
			|| i == b_l_pinky1
			|| i == b_l_ring1
			|| i == b_l_thumb1)
		{
			transform.position.y = -transform.position.y;
			transform.position.z = -transform.position.z;
		}
	}

	//these bones have very different rotations, and we aren't changing them anyway, so just copy the base pose
	auto skinnedMeshRender = GHOculusUtil::getSkinnedMeshPose(mAvatar, mComponentToModify);
	mJoints[b_l_hand] = skinnedMeshRender->jointTransform[b_l_hand];
	mJoints[b_l_grip] = skinnedMeshRender->jointTransform[b_l_grip];
	

	mUndoStack.clear();
}

void GHOculusAvatarCustomGestureFinder::calcCurrentWorldSpaceTransform(GHTransform& outTransform)
{
	if (!mAvatarModel) { return; }

	const ovrAvatarRenderPart_SkinnedMeshRender* meshRender = getSkinnedMeshRender(mAvatar, mComponentToModify);
	GHTransformNode* modelNode = mAvatarModel->getSkeleton()->findChild(GHIdentifier((GHIdentifier::IDType)meshRender->meshAssetID));

	if (!modelNode) { return; }

	GHTransform modelTrans = modelNode->getTransform();
	
	calcBoneModelSpaceTransform(mCurrentBoneIndex, outTransform);

	outTransform.mult(modelTrans, outTransform);
}

void GHOculusAvatarCustomGestureFinder::calcBoneModelSpaceTransform(unsigned int boneIndex, GHTransform& outTransform)
{
	const ovrAvatarSkinnedMeshPose* meshPose = getSkinnedMeshPose(mAvatar, mComponentToModify);

	if (boneIndex >= meshPose->jointCount)
	{
		return;
	}

	GHOculusUtil::convertOvrAvatarTransformToGH(meshPose->jointTransform[boneIndex], outTransform);

	int parentIndex = meshPose->jointParents[boneIndex];
	if (parentIndex >= 0)
	{
		GHTransform parentTransform;
		calcBoneModelSpaceTransform(parentIndex, parentTransform);
		outTransform.mult(parentTransform, outTransform);
	}
}

void GHOculusAvatarCustomGestureFinder::handleUndo(void)
{
	if (mUndoStack.size())
	{
		Change& change = mUndoStack.back();
		change.mTransformRef = change.mOriginalTransform;

		mUndoStack.pop_back();

		mChangeHappenedThisFrame = false;

		GHDebugMessage::outputString("Undoing Previous Change");
	}
}
