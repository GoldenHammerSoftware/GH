#include "GHOculusAvatarDebugDrawHands.h"
#include "GHDebugDraw.h"
#include "GHOculusAvatarUtil.h"
#include "GHPlatform/GHDebugMessage.h"

GHOculusAvatarDebugDrawHands::GHOculusAvatarDebugDrawHands(const GHTransform& hmdOrigin)
	: mHMDOrigin(hmdOrigin)
{

}

GHOculusAvatarDebugDrawHands::~GHOculusAvatarDebugDrawHands(void)
{
	for (size_t i = 0; i < mLines.size(); ++i)
	{
		GHDebugDraw::getSingleton().removeShape(mLines[i]);
	}
}

unsigned int GHOculusAvatarDebugDrawHands::getNextLineId(void)
{
	unsigned int nextLineID = mNextLine;
	if (nextLineID >= mLines.size())
	{
		unsigned int ret = GHDebugDraw::getSingleton().addLine(GHPoint3(), GHPoint3(), GHPoint3());
		mLines.push_back(ret);
	}

	++mNextLine;
	return mLines[nextLineID];
}

const static int kNumCrosses = 8;
DebugCross debugCrosses[kNumCrosses];

void GHOculusAvatarDebugDrawHands::drawAvatar(ovrAvatar* avatar)
{
	if (!avatar) {
		return;
	}

	mNextLine = 0;

	
	uint32_t componentCount = ovrAvatarComponent_Count(avatar);
	//for (uint32_t i = 4; i < componentCount; i = componentCount)//only render the left hand
	for (uint32_t i = 0; i < componentCount; ++i)
	{
		const ovrAvatarComponent* component = ovrAvatarComponent_Get(avatar, i);

		GHTransform worldTrans;
		GHOculusUtil::convertOvrAvatarTransformToGH(component->transform, worldTrans);
		moveFromOvrToWorldSpace(worldTrans);
		
		//if (i < kNumCrosses)
		//{
		//	GHTransform tempWorld = worldTrans;
		//	moveFromOvrToWorldSpace(tempWorld);
		//	debugCrosses[i].setTransform(tempWorld);
		//}
		//continue;
		 
		for (uint32_t j = 0; j < component->renderPartCount; ++j)
		{
			const ovrAvatarRenderPart* renderPart = component->renderParts[j];
			ovrAvatarRenderPartType type = ovrAvatarRenderPart_GetType(renderPart);
			switch (type)
			{
				//mesh rendering would want to differentiate between these two types.
				// for debug drawing, we just need the transforms and joint poses
				case ovrAvatarRenderPartType_SkinnedMeshRender:
				{
					const ovrAvatarRenderPart_SkinnedMeshRender* mesh = ovrAvatarRenderPart_GetSkinnedMeshRender(renderPart);
					drawAvatarPart(mesh->localTransform, mesh->skinnedPose, worldTrans);
					break;
				}
				case ovrAvatarRenderPartType_SkinnedMeshRenderPBS:
				{
					const ovrAvatarRenderPart_SkinnedMeshRenderPBS* mesh = ovrAvatarRenderPart_GetSkinnedMeshRenderPBS(renderPart);
					drawAvatarPart(mesh->localTransform, mesh->skinnedPose, worldTrans);
					break;
				}
				case ovrAvatarRenderPartType_ProjectorRender:
					//do nothing, no joints to render
					break;
				case ovrAvatarRenderPartType_SkinnedMeshRenderPBS_V2:
				{
					const ovrAvatarRenderPart_SkinnedMeshRenderPBS_V2* mesh = ovrAvatarRenderPart_GetSkinnedMeshRenderPBSV2(renderPart);
					drawAvatarPart(mesh->localTransform, mesh->skinnedPose, worldTrans);
					break;
				}
				case ovrAvatarRenderPartType_Count:
					assert(false);
					break;
			}
		}
	}
}

void GHOculusAvatarDebugDrawHands::drawAvatarPart(const ovrAvatarTransform& localTransform, const ovrAvatarSkinnedMeshPose& skinnedPose, const GHTransform& worldTransform)
{
	GHTransform ghLocalTrans;
	GHOculusUtil::convertOvrAvatarTransformToGH(localTransform, ghLocalTrans);

	ghLocalTrans.mult(worldTransform, ghLocalTrans);

	GHTransform* transformedPoses = (GHTransform*)alloca(sizeof(GHTransform)*skinnedPose.jointCount);

	//transform by parent transforms
	for (uint32_t i = 0; i < skinnedPose.jointCount; ++i)
	{
		GHTransform jointTrans;
		GHOculusUtil::convertOvrAvatarTransformToGH(skinnedPose.jointTransform[i], jointTrans);

		int parentIndex = skinnedPose.jointParents[i];
		if (parentIndex < 0)
		{
			transformedPoses[i] = jointTrans;
		}
		else
		{
			const GHTransform& parentTrans = transformedPoses[parentIndex];
			jointTrans.mult(parentTrans, transformedPoses[i]);
		}
	}

	for (uint32_t i = 0; i < skinnedPose.jointCount; ++i)
	{
		GHTransform& pose = transformedPoses[i];
		pose.mult(ghLocalTrans, pose);
	}

	for (uint32_t i = 0; i < skinnedPose.jointCount; ++i)
	{
		int parentIndex = skinnedPose.jointParents[i];
		if (parentIndex < 0)
		{
			continue;
		}
		
		const GHTransform& parentTrans = transformedPoses[parentIndex];
		const GHTransform& myTrans = transformedPoses[i];

		//if (i == 1)
		//{
		//	debugCrosses[i].setTransform(myTrans);
		//}

		GHPoint3 parentPos, myPos;
		parentTrans.getTranslate(parentPos);

		myTrans.getTranslate(myPos);

		unsigned int lineID = getNextLineId();

 		GHDebugDraw::getSingleton().setLine(lineID, parentPos, myPos, GHPoint3(1, 0, 0));
	}
}

void GHOculusAvatarDebugDrawHands::moveFromOvrToWorldSpace(GHTransform& transform)
{
	transform.mult(mHMDOrigin, transform);
}
