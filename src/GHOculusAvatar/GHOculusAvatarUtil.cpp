#include "GHOculusAvatarUtil.h"
#include "GHXMLSerializer.h"
#include "GHXMLNode.h"
#include "GHMath/GHTransform.h"
#include "GHMath/GHQuaternion.h"

namespace GHOculusUtil
{
	void convertOvrAvatarVecToGH(const ovrAvatarVector3f& inVec, GHPoint3& outVec)
	{
		outVec[0] = -inVec.x;
		outVec[1] = inVec.y;
		outVec[2] = -inVec.z;
	}

	void convertOvrAvatarQuatToGH(const ovrAvatarQuatf& inQuat, GHPoint4& outQuat)
	{
		outQuat[0] = inQuat.x;
		outQuat[1] = -inQuat.y;
		outQuat[2] = inQuat.z;
		outQuat[3] = inQuat.w;
	}

	void convertGHQuatToOvrAvatar(const GHPoint4& inQuat, ovrAvatarQuatf& outQuat)
	{
		outQuat.x = inQuat[0];
		outQuat.y = -inQuat[1];
		outQuat.z = inQuat[2];
		outQuat.w = inQuat[3];
	}

	float eraseNan(float f)
	{
		if (isnan(f)) return 0;
		return f;
	}

	void convertOvrAvatarTransformToGH(const ovrAvatarTransform& inTransform, GHTransform& outTransform)
	{
		convertOvrAvatarTransformToGH(inTransform, outTransform.getMatrix());
		outTransform.incrementVersion();
	}

	void convertOvrAvatarTransformToGH(const ovrAvatarTransform& inTransform, GHPoint16& outTransform)
	{
		GHPoint4 ghQuat;
		convertOvrAvatarQuatToGH(inTransform.orientation, ghQuat);

		ghQuat.normalize();

		GHQuaternion::convertQuaternionToTransform(ghQuat, outTransform);

		GHTransform scaleMatrix;
		scaleMatrix.becomeScaleMatrix(inTransform.scale.x, inTransform.scale.y, inTransform.scale.z);
		//todo: add scale

		//assert(inTransform.scale.x == inTransform.scale.y == inTransform.scale.z == 1.f);
		outTransform[12] = -eraseNan(inTransform.position.x);
		outTransform[13] = eraseNan(inTransform.position.y);
		outTransform[14] = -eraseNan(inTransform.position.z);
	}

	void convertGHTransformToOvrAvatar(const GHTransform& inTransform, ovrAvatarTransform& outTransform)
	{
		GHPoint4 ghQuat;
		GHQuaternion::convertTransformToQuaternion(inTransform, ghQuat);

		convertGHQuatToOvrAvatar(ghQuat, outTransform.orientation);

		//todo: add scale
		outTransform.scale.x = outTransform.scale.y = outTransform.scale.z = 1;

		GHPoint3 pos;
		inTransform.getTranslate(pos);
		outTransform.position.x = -pos[0];
		outTransform.position.y = pos[1];
		outTransform.position.z = -pos[2];
	}

	const ovrAvatarRenderPart_SkinnedMeshRender* getSkinnedMeshRender(ovrAvatar* avatar, AvatarComponentType componentType)
	{
		const ovrAvatarHandComponent* handComp = 0;
		switch (componentType)
		{
		case LEFTHAND:
			handComp = ovrAvatarPose_GetLeftHandComponent(avatar);
			break;
		case RIGHTHAND:
			handComp = ovrAvatarPose_GetRightHandComponent(avatar);
			break;
		}

		const ovrAvatarComponent* comp = handComp->renderComponent;
		const ovrAvatarRenderPart* renderPart = comp->renderParts[0];
		return ovrAvatarRenderPart_GetSkinnedMeshRender(renderPart);
	}

	const ovrAvatarSkinnedMeshPose* getSkinnedMeshPose(ovrAvatar* avatar, AvatarComponentType componentType)
	{
		const ovrAvatarRenderPart_SkinnedMeshRender* meshRender = getSkinnedMeshRender(avatar, componentType);
		return &meshRender->skinnedPose;
	}

	void saveGesture(ovrAvatar* avatar, AvatarComponentType componentType, const char* filename, const GHXMLSerializer& xmlSerializer)
	{
		const ovrAvatarSkinnedMeshPose* meshPose = getSkinnedMeshPose(avatar, componentType);

		GHXMLNode saveNode;
		saveNode.setName("gesture", GHString::CHT_REFERENCE);
		const static int BUFSZ = 256;
		char buffer[BUFSZ];
		for (unsigned int i = 0; i < meshPose->jointCount; ++i)
		{
			const ovrAvatarTransform& transform = meshPose->jointTransform[i];

			GHXMLNode* jointNode = new GHXMLNode;
			jointNode->setName(meshPose->jointNames[i], GHString::CHT_REFERENCE);

			snprintf(buffer, BUFSZ, "%f %f %f", transform.position.x, transform.position.y, transform.position.z);
			jointNode->setAttribute("pos", GHString::CHT_REFERENCE, buffer, GHString::CHT_COPY);

			snprintf(buffer, BUFSZ, "%f %f %f %f", transform.orientation.x, transform.orientation.y, transform.orientation.z, transform.orientation.w);
			jointNode->setAttribute("quat", GHString::CHT_REFERENCE, buffer, GHString::CHT_COPY);

			snprintf(buffer, BUFSZ, "%f %f %f", transform.scale.x, transform.scale.y, transform.scale.z);
			jointNode->setAttribute("scale", GHString::CHT_REFERENCE, buffer, GHString::CHT_COPY);

			saveNode.addChild(jointNode);
		}

		xmlSerializer.writeXMLFile(filename, saveNode);
	}

	void loadGesture(ovrAvatar* avatar, AvatarComponentType componentType, const char* filename, const GHXMLSerializer& xmlSerializer, std::vector<ovrAvatarTransform>& gesture)
	{
		GHXMLNode* topNode = xmlSerializer.loadXMLFile(filename);

		if (!topNode) { return; }

		const ovrAvatarSkinnedMeshPose* meshPose = getSkinnedMeshPose(avatar, componentType);
		gesture.resize(meshPose->jointCount);
		for (unsigned int i = 0; i < meshPose->jointCount; ++i)
		{
			GHXMLNode* jointNode = topNode->getChild(meshPose->jointNames[i], false);
			if (!jointNode) { continue; }

			ovrAvatarTransform& transform = gesture[i];

			jointNode->readAttrFloatArr("pos", (float*)&transform.position, 3);
			jointNode->readAttrFloatArr("quat", (float*)&transform.orientation, 4);
			jointNode->readAttrFloatArr("scale", (float*)&transform.scale, 3);
		}

		delete topNode;
	}
}
