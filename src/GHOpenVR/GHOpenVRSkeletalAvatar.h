#pragma once

#include "GHUtils/GHController.h"
#include <openvr.h>
#include "GHString/GHString.h"
#include "GHInputState.h"

class GHOpenVRAvatarDebugDraw;
class GHTransform;
class GHModel;
class GHTransformNode;
class GHStringIdFactory;

class GHOpenVRSkeletalAvatar : public GHController
{
public:
	GHOpenVRSkeletalAvatar(GHModel& leftHandModel, GHModel& rightHandModel, 
						   const GHInputState& inputState, int gamepadIndex,
						   const GHStringIdFactory& idFactory,
						   const GHTransform& hmdOrigin, const char* gestureMapFilename);
	virtual ~GHOpenVRSkeletalAvatar(void);

	virtual void onActivate(void);
	virtual void onDeactivate(void);
	virtual void update(void);

	struct SkeletalData
	{
		SkeletalData(const GHInputStructs::Poseable& poseable, const GHStringIdFactory& idFactory)
			: mPoseable(poseable) 
			, mIdFactory(idFactory)
		{ }
		const GHInputStructs::Poseable& mPoseable;
		const GHStringIdFactory& mIdFactory;

		GHString mActionName;
		GHModel* mGHModel{ 0 };
		vr::VRActionHandle_t mAction{ 0 };
		vr::InputSkeletalActionData_t mSkeletalData;
		vr::InputPoseActionData_t mPoseData;
		uint32_t mNumBones{ 0 };
		vr::BoneIndex_t* mBoneParentIndices{ 0 };
		vr::VRBoneTransform_t* mReferenceTransforms{ 0 };

		//Dynamic data
		vr::VRBoneTransform_t* mBoneTransforms{ 0 };
		GHTransform* mWorldSpaceBoneTransforms{ 0 }; //Scratch memory for computing world transforms if necessary
		GHTransformNode** mTargetModelTransforms{ 0 };
		GHOpenVRAvatarDebugDraw* mDebugDrawer{ 0 };
		bool mInitializedSuccessfully{ false };
		bool mIsInModelSpace{ false }; //true if mBoneTransforms are in model space, false if in bone space

		~SkeletalData(void);
		bool initialize(const char* actionName, GHModel* ghModel);
		bool tryReinitialize(void);
		void update(void);
	};

private:
	SkeletalData mLeftHand;
	SkeletalData mRightHand;

	GHOpenVRAvatarDebugDraw* mDebugDrawer{ 0 };

	const GHTransform& mHMDOrigin;
};
