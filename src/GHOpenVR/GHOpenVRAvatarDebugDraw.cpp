#include "GHOpenVRAvatarDebugDraw.h"
#include "GHDebugDraw.h"
#include "GHOpenVRUtil.h"
#include "GHModel.h"
#include "GHPlatform/GHDebugMessage.h"

GHOpenVRAvatarDebugDraw::GHOpenVRAvatarDebugDraw(void)
{

}

GHOpenVRAvatarDebugDraw::~GHOpenVRAvatarDebugDraw(void)
{
	for (int i = 0; i < mDebugDrawIds.size(); ++i)
	{
		int* debugDrawIDs = mDebugDrawIds[i];
		int numLines = mSkeletons[i]->mNumBones;
		for (int line = 0; line < numLines; ++line)
		{
			GHDebugDraw::getSingleton().removeShape(debugDrawIDs[line]);
		}
		delete[] debugDrawIDs;

		int* modelDebugDrawIDs = mModelDebugDrawIds[i];
		for (int line = 0; line < numLines; ++line)
		{
			GHDebugDraw::getSingleton().removeShape(modelDebugDrawIDs[line]);
		}
		delete[] modelDebugDrawIDs;
	}
}

void GHOpenVRAvatarDebugDraw::addSkeleton(const SkeletalData* skeleton)
{
	if (!skeleton) {
		return;
	}

	mSkeletons.push_back(skeleton);
	int numLines = skeleton->mNumBones; 
	int* debugDrawIds = new int[numLines];
	for (int i = 0; i < numLines; ++i)
	{
		debugDrawIds[i] = GHDebugDraw::getSingleton().addLine(GHPoint3(0, 0, 0), GHPoint3(0, 0, 0), GHPoint3(1, 0, 0));
	}
	mDebugDrawIds.push_back(debugDrawIds);


	int* modelDebugDrawIds  = new int[numLines];
	for (int i = 0; i < numLines; ++i)
	{
		modelDebugDrawIds[i] = GHDebugDraw::getSingleton().addLine(GHPoint3(0, 0, 0), GHPoint3(0, 0, 0), GHPoint3(0, 0, 1));
	}
	mModelDebugDrawIds.push_back(modelDebugDrawIds);
}

void GHOpenVRAvatarDebugDraw::onActivate(void)
{

}

void GHOpenVRAvatarDebugDraw::onDeactivate(void)
{

}

void GHOpenVRAvatarDebugDraw::update(void)
{
	for (int i = 0; i < mSkeletons.size(); ++i)
	{
		//updateSkeleton(*mSkeletons[i], mDebugDrawIds[i]);

		int* debugDrawHandle = mModelDebugDrawIds[i];
		drawModelSkeleton(mSkeletons[i]->mGHModel->getSkeleton(), debugDrawHandle);
		//drawModelSkeleton(*mSkeletons[i], debugDrawHandle);
	}
}

void GHOpenVRAvatarDebugDraw::updateSkeleton(const SkeletalData& skeleton, const int* debugDrawIds) const
{
	for (uint32_t i = 0; i < skeleton.mNumBones; ++i)
	{
		vr::BoneIndex_t parentIndex = skeleton.mBoneParentIndices[i];
		if (parentIndex < 0) {
			//we have an extra trivial line here that we just won't bother to update.
			if (!skeleton.mIsInModelSpace) {
				GHTransform& ourTrans = skeleton.mWorldSpaceBoneTransforms[i];
				GHOpenVRUtil::convertToGHTransform(skeleton.mBoneTransforms[i], ourTrans);
			}
			continue;
		}

		if (parentIndex >= (vr::BoneIndex_t)skeleton.mNumBones) {
			//yikes! this shouldn't happen
			continue;
		}

		const vr::VRBoneTransform_t& bone = skeleton.mBoneTransforms[i];
		const vr::VRBoneTransform_t& parentBone = skeleton.mBoneTransforms[parentIndex];

		GHPoint3 ourPos, parentPos;

		if (skeleton.mIsInModelSpace)
		{
			//GHOpenVRUtil::convertToGHPoint3(bone.position, ourPos);
			//GHOpenVRUtil::convertToGHPoint3(parentBone.position, parentPos);
			//

			GHTransform ourTrans, parentTrans;
			GHOpenVRUtil::convertToGHTransform(bone, ourTrans);
			GHOpenVRUtil::convertToGHTransform(parentBone, parentTrans);
			ourTrans.getTranslate(ourPos);
			parentTrans.getTranslate(parentPos);
		}
		else // bone space
		{
			GHTransform& ourTrans = skeleton.mWorldSpaceBoneTransforms[i];
			GHOpenVRUtil::convertToGHTransform(bone, ourTrans);
			
			const GHTransform& parentTrans = skeleton.mWorldSpaceBoneTransforms[parentIndex];
			ourTrans.mult(parentTrans, ourTrans);

			ourTrans.getTranslate(ourPos);
			parentTrans.getTranslate(parentPos);
		}

		skeleton.mPoseable.mPosition.mult(ourPos, ourPos);
		skeleton.mPoseable.mPosition.mult(parentPos, parentPos);

		GHPoint3 color(1, 0, 0);
		GHDebugDraw::getSingleton().setLine(debugDrawIds[i], parentPos, ourPos, color);
	}
}

void GHOpenVRAvatarDebugDraw::drawModelSkeleton(GHTransformNode* transformNode, int*& debugDrawHandle) const
{
	if (!transformNode) {
		return;
	}

	GHPoint3 ourPos;
	transformNode->getTransform().getTranslate(ourPos);

	auto& children = transformNode->getChildren();
	for (GHTransformNode* child : children)
	{
		GHPoint3 childPos;
		child->getTransform().getTranslate(childPos);

		//GHDebugMessage::outputString("%s -> %s: (%f %f %f) -> (%f %f %f)", transformNode->getId().getString(), child->getId().getString(), ourPos[0], ourPos[1], ourPos[2], childPos[0], childPos[1], childPos[2]);

		GHPoint3 color(0, 0, 1);
		GHDebugDraw::getSingleton().setLine(*(debugDrawHandle++), ourPos, childPos, color);

		int debugDrawIndex = 0;
		drawModelSkeleton(child, debugDrawHandle);
	}
}

void GHOpenVRAvatarDebugDraw::drawModelSkeleton(const SkeletalData& skeleton, const int* debugDrawIds) const
{
	for (unsigned int i = 0; i < skeleton.mNumBones; ++i)
	{
		vr::BoneIndex_t parentIndex = skeleton.mBoneParentIndices[i];
		if (parentIndex < 0) {
			//we have an extra trivial line here that we just won't bother to update.
			continue;
		}

		GHTransformNode* ourNode = skeleton.mTargetModelTransforms[i];
		GHTransformNode* parentNode = skeleton.mTargetModelTransforms[parentIndex];

		GHPoint3 parentPos, ourPos;
		parentNode->getTransform().getTranslate(parentPos);
		ourNode->getTransform().getTranslate(ourPos);

		GHPoint3 color(0, 0, 1);
		GHDebugDraw::getSingleton().setLine(debugDrawIds[i], parentPos, ourPos, color);
	}
}
