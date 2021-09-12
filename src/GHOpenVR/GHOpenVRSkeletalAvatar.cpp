#include "GHOpenVRSkeletalAvatar.h"
#include <openvr.h>
#include "GHPlatform/GHDebugMessage.h"
#include "GHOpenVRAvatarDebugDraw.h"
#include "GHModel.h"
#include "GHString/GHStringIdFactory.h"
#include "GHOpenVRUtil.h"

#define GHOVR_DEBUGDRAWHANDS

GHOpenVRSkeletalAvatar::GHOpenVRSkeletalAvatar(GHModel& leftHandModel, GHModel& rightHandModel, 
											   const GHInputState& inputState, int gamepadIndex,
											   const GHStringIdFactory& idFactory,
											   const GHTransform& hmdOrigin, const char* gestureMapFilename)
	: mLeftHand(inputState.getGamepad(gamepadIndex).mPoseables[0], idFactory)
	, mRightHand(inputState.getGamepad(gamepadIndex).mPoseables[1], idFactory)
	, mHMDOrigin(hmdOrigin)
{


	mLeftHand.initialize("/actions/main/in/lefthand_anim", &leftHandModel);
	mRightHand.initialize("/actions/main/in/righthand_anim", &rightHandModel);


#ifdef GHOVR_DEBUGDRAWHANDS
	mDebugDrawer = new GHOpenVRAvatarDebugDraw;
	mLeftHand.mDebugDrawer = mDebugDrawer;
	mRightHand.mDebugDrawer = mDebugDrawer;
#endif
}

GHOpenVRSkeletalAvatar::~GHOpenVRSkeletalAvatar(void)
{
	delete mDebugDrawer;
}

void GHOpenVRSkeletalAvatar::onActivate(void)
{
#ifdef GHOVR_DEBUGDRAWHANDS
	if (mDebugDrawer)
	{
		mDebugDrawer->onActivate();
	}
#endif
}

void GHOpenVRSkeletalAvatar::onDeactivate(void)
{
#ifdef GHOVR_DEBUGDRAWHANDS
	if (mDebugDrawer)
	{
		mDebugDrawer->onDeactivate();
	}
#endif
}

//This could go somewhere more generally accessible
bool checkSuccess(vr::EVRInputError error, const char* functionName)
{
	switch (error)
	{
		case vr::VRInputError_None: return true;
		case vr::VRInputError_NameNotFound: GHDebugMessage::outputString("error calling %s: VRInputError_NameNotFound", functionName); break;
		case vr::VRInputError_WrongType: GHDebugMessage::outputString("error calling %s: VRInputError_WrongType", functionName); break;
		case vr::VRInputError_InvalidHandle: GHDebugMessage::outputString("error calling %s: VRInputError_InvalidHandle ", functionName); break;
		case vr::VRInputError_InvalidParam: GHDebugMessage::outputString("error calling %s: VRInputError_InvalidParam ", functionName); break;
		case vr::VRInputError_NoSteam: GHDebugMessage::outputString("error calling %s: VRInputError_NoSteam ", functionName); break;
		case vr::VRInputError_MaxCapacityReached: GHDebugMessage::outputString("error calling %s: VRInputError_MaxCapacityReached ", functionName); break;
		case vr::VRInputError_IPCError: GHDebugMessage::outputString("error calling %s: VRInputError_IPCError ", functionName); break;
		case vr::VRInputError_NoActiveActionSet: GHDebugMessage::outputString("error calling %s: VRInputError_NoActiveActionSet ", functionName); break;
		case vr::VRInputError_InvalidDevice: GHDebugMessage::outputString("error calling %s: VRInputError_InvalidDevice", functionName); break;
		case vr::VRInputError_InvalidSkeleton: GHDebugMessage::outputString("error calling %s: VRInputError_InvalidSkeleton ", functionName); break;
		case vr::VRInputError_InvalidBoneCount: GHDebugMessage::outputString("error calling %s: VRInputError_InvalidBoneCount ", functionName); break;
		case vr::VRInputError_InvalidCompressedData: GHDebugMessage::outputString("error calling %s: VRInputError_InvalidCompressedData ", functionName); break;
		case vr::VRInputError_NoData: GHDebugMessage::outputString("error calling %s: VRInputError_NoData", functionName); break;
		case vr::VRInputError_BufferTooSmall: GHDebugMessage::outputString("error calling %s: VRInputError_BufferTooSmall", functionName); break;
		case vr::VRInputError_MismatchedActionManifest: GHDebugMessage::outputString("error calling %s: VRInputError_MismatchedActionManifest", functionName); break;
		case vr::VRInputError_MissingSkeletonData: GHDebugMessage::outputString("error calling %s: VRInputError_MissingSkeletonData ", functionName); break;
		case vr::VRInputError_InvalidBoneIndex: GHDebugMessage::outputString("error calling %s: VRInputError_InvalidBoneIndex ", functionName); break;
		default: GHDebugMessage::outputString("unknown error calling %s", functionName); break;
	}
	return false;
}

GHOpenVRSkeletalAvatar::SkeletalData::~SkeletalData(void)
{
	delete[] mBoneParentIndices;
	delete[] mReferenceTransforms;
	delete[] mBoneTransforms;
	delete[] mTargetModelTransforms;
	delete[] mWorldSpaceBoneTransforms;
}

bool GHOpenVRSkeletalAvatar::SkeletalData::initialize(const char* actionName, GHModel* ghModel)
{
	mGHModel = ghModel;
	mActionName.setConstChars(actionName, GHString::CHT_COPY);
	return tryReinitialize();
}

bool GHOpenVRSkeletalAvatar::SkeletalData::tryReinitialize(void)
{
	vr::EVRInputError error;

	if (!mAction) 
	{
		error = vr::VRInput()->GetActionHandle(mActionName.getChars(), &mAction);
		if (!checkSuccess(error, "GetActionHandle")) {
			return false;
		}
	}
	
	error = vr::VRInput()->GetBoneCount(mAction, &mNumBones);
	if (!checkSuccess(error, "GetBoneCount")) {
		return false;
	}

	if (!mBoneParentIndices) {
		mBoneParentIndices = new vr::BoneIndex_t[mNumBones];
	}
	if (!mBoneParentIndices) {
		GHDebugMessage::outputString("Error creating SkeletalData: out of memory");
		return false;
	}
	error = vr::VRInput()->GetBoneHierarchy(mAction, mBoneParentIndices, mNumBones);
	if (!checkSuccess(error, "GetBoneHierarchy")) {
		return false;
	}

	if (mGHModel && mGHModel->getSkeleton())
	{
		if (!mTargetModelTransforms) {
			mTargetModelTransforms = new GHTransformNode * [mNumBones];
			memset(mTargetModelTransforms, 0, sizeof(GHTransformNode*) * mNumBones);
		}
		if (!mTargetModelTransforms) {
			GHDebugMessage::outputString("Error creating SkeletalData: out of memory");
			return false;
		}

		const uint32_t bufSize = 512;
		char nameBuf[bufSize];
		for (uint32_t i = 0; i < mNumBones; ++i)
		{
			error = vr::VRInput()->GetBoneName(mAction, i, nameBuf, bufSize);
			if (!checkSuccess(error, "GetBoneName")) {
				return false;
			}

			GHIdentifier boneID = mIdFactory.generateHash(nameBuf);
			GHTransformNode* boneNode = mGHModel->getSkeleton()->findChild(boneID);
			mTargetModelTransforms[i] = boneNode;
			if (!boneNode) {
				GHDebugMessage::outputString("Error: could not find bone node named %s in GH hand model", nameBuf);
			}
		}
	}
	
	//just guessing what these should be
	vr::EVRSkeletalTransformSpace transformSpace = vr::VRSkeletalTransformSpace_Model;
	vr::EVRSkeletalReferencePose referencePose = vr::VRSkeletalReferencePose_BindPose;

	if (!mReferenceTransforms) {
		mReferenceTransforms = new vr::VRBoneTransform_t[mNumBones];
	}
	if (!mReferenceTransforms) {
		GHDebugMessage::outputString("Error creating SkeletalData: out of memory");
		return false;
	}
	vr::VRInput()->GetSkeletalReferenceTransforms(mAction, transformSpace, referencePose, mReferenceTransforms, mNumBones);
	if (!checkSuccess(error, "GetSkeletalReferenceTransforms")) {
		return false;
	}

	mBoneTransforms = new vr::VRBoneTransform_t[mNumBones];
	if (!mBoneTransforms) {
		GHDebugMessage::outputString("Error creating SkeletalData: out of memory");
		return false;
	}

	mWorldSpaceBoneTransforms = new GHTransform[mNumBones];

	mInitializedSuccessfully = true;
	return true;
}

void GHOpenVRSkeletalAvatar::SkeletalData::update(void)
{
	vr::EVRSkeletalTransformSpace transformSpace = vr::VRSkeletalTransformSpace_Model;
	//vr::EVRSkeletalTransformSpace transformSpace = vr::VRSkeletalTransformSpace_Parent;
	vr::EVRSkeletalMotionRange motionRange = vr::VRSkeletalMotionRange_WithoutController;
	//	vr::EVRSkeletalMotionRange motionRange = vr::VRSkeletalMotionRange_WithController;

	vr::EVRInputError error = vr::VRInput()->GetSkeletalActionData(mAction, &mSkeletalData, sizeof(mSkeletalData));
	if (!checkSuccess(error, "GetSkeletalActionData")) {
		return;
	}

	if (!mSkeletalData.bActive) {
		return;
	}

	if (!mInitializedSuccessfully) {
		if (!tryReinitialize()) {
			return;
		}

#ifdef GHOVR_DEBUGDRAWHANDS
		if (mDebugDrawer) {
			mDebugDrawer->addSkeleton(this);
		}
#endif
	}

	//other options are seated or standing
	vr::ETrackingUniverseOrigin trackingOrigin = vr::TrackingUniverseRawAndUncalibrated;
	//From the docs: skeletal poses are not restricted to a device
	vr::VRInputValueHandle_t restrictToDevice = vr::k_ulInvalidInputValueHandle;
	error = vr::VRInput()->GetPoseActionDataForNextFrame(mAction, trackingOrigin, &mPoseData, sizeof(mPoseData), restrictToDevice);
	checkSuccess(error, "GetPoseActionDataForNextFrame");

	error = vr::VRInput()->GetSkeletalBoneData(mAction, transformSpace, motionRange, mBoneTransforms, mNumBones);
	checkSuccess(error, "GetSkeletalBoneData");

	mIsInModelSpace = transformSpace == vr::VRSkeletalTransformSpace_Model;

	if (mIsInModelSpace)
	{
		for (unsigned int i = 0; i < mNumBones; ++i)
		{
			GHTransformNode* targetNode = mTargetModelTransforms[i];
			if (!targetNode) {
				continue;
			}

			GHTransform transform;
			GHOpenVRUtil::convertToGHTransform(mBoneTransforms[i], transform);

			transform.becomeIdentity();
			GHPoint3 pos;
			GHOpenVRUtil::convertToGHPoint3(mBoneTransforms[i].position, pos);
			transform.setTranslate(pos);

			targetNode->setWorldTransform(transform);
		}
	}
	else
	{
		for (unsigned int i = 0; i < mNumBones; ++i)
		{
			GHTransformNode* targetNode = mTargetModelTransforms[i];
			if (!targetNode) {
				continue;
			}

			GHTransform transform;
			GHOpenVRUtil::convertToGHTransform(mBoneTransforms[i], targetNode->getLocalTransform());
		}
	}

	mGHModel->getSkeleton()->setWorldTransform(mPoseable.mPosition);
}

void GHOpenVRSkeletalAvatar::update(void)
{
	mLeftHand.update();
	mRightHand.update();

#ifdef GHOVR_DEBUGDRAWHANDS
	if (mDebugDrawer)
	{
		mDebugDrawer->update();
	}
#endif
}
