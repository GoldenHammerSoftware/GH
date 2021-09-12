#pragma once
#include "OVR_Avatar.h"
#include "GHMath/GHPoint.h"
#include <vector>

class GHXMLSerializer;
class GHTransform;

namespace GHOculusUtil
{
	void convertOvrAvatarVecToGH(const ovrAvatarVector3f& inVec, GHPoint3& outVec);

	void convertOvrAvatarQuatToGH(const ovrAvatarQuatf& inQuat, GHPoint4& outQuat);
	void convertGHQuatToOvrAvatar(const GHPoint4& inQuat, ovrAvatarQuatf& outQuat);

	void convertOvrAvatarTransformToGH(const ovrAvatarTransform& inTransform, GHTransform& outTransform);
	void convertOvrAvatarTransformToGH(const ovrAvatarTransform& inTransform, GHPoint16& outTransform);

	void convertGHTransformToOvrAvatar(const GHTransform& inTransform, ovrAvatarTransform& outTransform);

	enum AvatarComponentType
	{
		LEFTHAND,
		RIGHTHAND,
	};

	const ovrAvatarRenderPart_SkinnedMeshRender* getSkinnedMeshRender(ovrAvatar* avatar, AvatarComponentType componentType);
	const ovrAvatarSkinnedMeshPose* getSkinnedMeshPose(ovrAvatar* avatar, AvatarComponentType componentType);

	void saveGesture(ovrAvatar* avatar, AvatarComponentType componentType, const char* filename, const GHXMLSerializer& xmlSerializer);
	void loadGesture(ovrAvatar* avatar, AvatarComponentType componentType, const char* filename, const GHXMLSerializer& xmlSerializer, std::vector<ovrAvatarTransform>& gesture);
}