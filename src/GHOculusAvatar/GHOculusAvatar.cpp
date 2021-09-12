#include "GHOculusAvatar.h"
#include "GHOculusAvatarPlatformPoser.h"
#include "GHModel.h"
#include "GHGeometryRenderable.h"
#include "GHGeometry.h"
#include "GHVertexBuffer.h"
#include "GHString/GHStringIdFactory.h"
#include "GHVBDescription.h"
#include "GHVBFactory.h"
#include "GHOculusAvatarUtil.h"
#include "GHSkinAnimControllerCPU.h"
#include "GHModelSkinAnimCloner.h"
#include "GHMaterial.h"
#include "GHXMLObjFactory.h"
#include "GHRenderServices.h"
#include "GHMDesc.h"
#include "GHMaterialParamHandle.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHProfiler.h"
#include "GHOculusAvatarCustomGestureFinder.h"
#include "GHGeoCreationUtil.h"
#include "GHUtils/GHEventMgr.h"
#include "GHUtils/GHResourceFactory.h"
#include "GHUtils/GHResourceLoader.h"
#include "GHTextureFormat.h"
#include "GHRenderProperties.h"
#include "GHTexture.h"
#include "GHMaterialCallbackFactoryTexture.h"
#include "GHMaterialCallbackMgr.h"
#include "GHUtils/GHMessage.h"
#include "GHGameIdentifiers.h"

//#define GH_ENABLE_OCULUS_AVATAR_DEBUG_TEXTURE

GHOculusAvatar::GHOculusAvatar(uint64_t userId,
							   const GHTransform& hmdOrigin,
							   GHOculusAvatarPlatformPoser* platformPoser,
							   GHVBFactory& vbFactory, 
  							   GHMaterialCallbackMgr& materialCallbackMgr,
							   const GHStringIdFactory& hashTable, 
							   const GHInputState& inputState,
							   const GHPropertyContainer& appProps,
							   const GHXMLSerializer& xmlSerializer,
							   const GHXMLObjFactory& xmlObjFactory,
							   GHResourceFactory& resourceFactory,
							   GHEventMgr& systemEventMgr,
							   GHEventMgr& gameEventMgr,
							   GHModel* model, 
							   const char* materialFilename, 
							   const char* pbsMaterialFilename,
							   const char* gestureMapFilename)
	: mGestureSetter(mAvatar, gameEventMgr, xmlSerializer, hashTable, gestureMapFilename)
	, mComponentOverrider(appProps)
	, mPlatformPoser(platformPoser)
	, mAvatarOrigin(hmdOrigin)
	//, mDebugDrawHands(new GHOculusAvatarDebugDrawHands(hmdOrigin))
	//, mCustomGestureFinder(new GHOculusAvatarCustomGestureFinder(oculusSystem, inputState, xmlSerializer, mAvatarModel, mAvatar))
	, mVBFactory(vbFactory)
	, mIdFactory(hashTable)
	, mXMLObjFactory(xmlObjFactory)
	, mResourceFactory(resourceFactory)
	, mDeviceReinitListener(systemEventMgr, *this)
{
	// grab the png loader.  this is an arbitrary extension we expect to be supported by the loader that will work for us.
	mTextureLoader = resourceFactory.getLoader(".ovrtex");
	mMaterialFilename.setConstChars(materialFilename, GHString::CHT_COPY);
	mPBSMaterialFilename.setConstChars(pbsMaterialFilename, GHString::CHT_COPY);

	GHRefCounted::changePointer((GHRefCounted*&)mAvatarModel, model);

#ifdef GH_ENABLE_OCULUS_AVATAR_DEBUG_TEXTURE
	// create a debug texture material callback for displaying arbitrary textures to the gui.
	GHMaterialCallbackFactory* avatarDebugTextureCallback = new GHMaterialCallbackFactoryTexture("AvatarDebugTexture", GHMaterialCallbackType::CT_PERFRAME, &mDebugTexture);
	materialCallbackMgr.addFactory(avatarDebugTextureCallback);
#endif
}

GHOculusAvatar::~GHOculusAvatar(void)
{
	// todo: release the textures in mTextureMap.

	auto iter = mSkeletonMap.begin(), iterEnd = mSkeletonMap.end();
	for (; iter != iterEnd; ++iter)
	{
		MeshSkeleton* meshSkeleton = iter->second.mSkeleton;
		delete meshSkeleton;
		delete iter->second.mBoneInversetransforms;
		delete iter->second.mParamHandle;
		delete iter->second.mModelTransforms;

		//if we are visible, the geometry is owned by the model. if not visible, we are responsible for deleting it
		if (!iter->second.mIsVisible)
		{
			delete iter->second.mGeometry;
		}

	}

	GHRefCounted::changePointer((GHRefCounted*&)mAvatarModel, 0);

	if (mAvatar)
	{
		ovrAvatar_Destroy(mAvatar);
	}
	
	delete mDebugDrawHands;
	delete mCustomGestureFinder;
	delete mPlatformPoser;
}

void GHOculusAvatar::onActivate(void)
{
	//steps not taken yet in comments
	//create microphone handle (implement later, probably in another file)

	//async function, handle result in message queue
	ovrAvatar_RequestAvatarSpecification(mUserId);
}

void GHOculusAvatar::onDeactivate(void)
{

}

void GHOculusAvatar::update(void)
{
	runMessageQueue();
	mPlatformPoser->poseAvatar(mAvatar);

	if (mDebugDrawHands)
	{
		mDebugDrawHands->drawAvatar(mAvatar);
	}

	if (mCustomGestureFinder)
	{
		mCustomGestureFinder->update();
	}

	if (mDeviceReinitHappened)
	{
		mGestureSetter.handleMessage(GHMessage(GHGameIdentifiers::M_RESETCURRENTHANDGESTURE));
		mDeviceReinitHappened = false;
	}

	if (isAvatarLoaded())
	{
		updateAvatarSkin();
	}
}

void GHOculusAvatar::runMessageQueue(void)
{
	while (ovrAvatarMessage* message = ovrAvatarMessage_Pop())
	{
		switch (ovrAvatarMessage_GetType(message))
		{
			case ovrAvatarMessageType_AvatarSpecification:
				handleAvatarSpecification(ovrAvatarMessage_GetAvatarSpecification(message));
				break;
			case ovrAvatarMessageType_AssetLoaded:
				handleAssetLoaded(ovrAvatarMessage_GetAssetLoaded(message));
				break;
			case ovrAvatarMessageType_Count: 
				assert(false);
				break;
		}
		ovrAvatarMessage_Free(message);
	}
}

//#define GH_SHOW_ENTIRE_AVATAR

void GHOculusAvatar::handleAvatarSpecification(const ovrAvatarMessage_AvatarSpecification* message)
{
#ifdef GH_SHOW_ENTIRE_AVATAR
	mAvatar = ovrAvatar_Create(message->avatarSpec, ovrAvatarCapability_All);
	ovrAvatar_SetActiveCapabilities(mAvatar, ovrAvatarCapability_All);
	mVisibilityFlags = ovrAvatarVisibilityFlag_ThirdPerson;
	ovrAvatar_SetLeftControllerVisibility(mAvatar, true);
	ovrAvatar_SetRightControllerVisibility(mAvatar, true);
#else
	//todo: make capabilities configurable
	mAvatar = ovrAvatar_Create(message->avatarSpec, ovrAvatarCapability_All);
	ovrAvatar_SetActiveCapabilities(mAvatar, ovrAvatarCapability_Hands);
#endif

	uint32_t refCount = ovrAvatar_GetReferencedAssetCount(mAvatar);
	for (uint32_t i = 0; i < refCount; ++i)
	{
		ovrAvatarAssetID id = ovrAvatar_GetReferencedAsset(mAvatar, i);
		ovrAvatarAsset_BeginLoading(id);
		++mNumAssetsLoading;
	}
}

//#define DEBUG_PRINT_TEXTURE_MAPPING
#ifdef DEBUG_PRINT_TEXTURE_MAPPING
void printTexture(const std::map<ovrAvatarAssetID, GHTexture*>& textureMap, int component, int renderPart, ovrAvatarAssetID textureID, const char* str)
{
	if (textureID == 0)
	{
		GHDebugMessage::outputString("component %d, render part %d does not have a %s texture (this is ok)", component, renderPart, str);
		return;
	}

	auto texIter = textureMap.find(textureID);
	if (texIter == textureMap.end())
	{
		GHDebugMessage::outputString("component %d, render part %d's %s uses a texture we failed to load (this is a bug)", component, renderPart, str);
		return;
	}

	//or use some other identifying information to recognize the texture
	GHDebugMessage::outputString("component %d, render part %d's %s texture is the texture loaded at address %x (asset id %llu)", component, renderPart, str, texIter->second, textureID);
}

void printTextures(const ovrAvatarMaterialState& materialState, const std::map<ovrAvatarAssetID, GHTexture*>& textureMap, int i, int j)
{
	printTexture(textureMap, i, j, materialState.alphaMaskTextureID, "alphaMaskTexture");
	printTexture(textureMap, i, j, materialState.normalMapTextureID, "normalMapTexture");
	printTexture(textureMap, i, j, materialState.parallaxMapTextureID, "parallaxMapTexture");
	printTexture(textureMap, i, j, materialState.roughnessMapTextureID, "roughnessMapTexture");

	const char* layerString[] =
	{
		"layer0",
		"layer1",
		"layer2",
		"layer3",
		"layer4",
		"layer5",
		"layer6",
		"layer7",
	};

	for (uint32_t l = 0; l < materialState.layerCount; ++l)
	{
		const ovrAvatarMaterialLayerState& layerState = materialState.layers[l];
		printTexture(textureMap, i, j, layerState.sampleTexture, layerString[l]);
	}
}

void printTextures(ovrAvatar* avatar, const std::map<ovrAvatarAssetID, GHTexture*>& textureMap)
{
	uint32_t componentCount = ovrAvatarComponent_Count(avatar);
	for (uint32_t i = 0; i < componentCount; ++i)
	{
		const ovrAvatarComponent* component = ovrAvatarComponent_Get(avatar, i);
		GHTransform worldTrans;
		GHOculusUtil::convertOvrAvatarTransformToGH(component->transform, worldTrans);
		for (uint32_t j = 0; j < component->renderPartCount; ++j)
		{
			const ovrAvatarRenderPart* renderPart = component->renderParts[j];
			switch (ovrAvatarRenderPart_GetType(renderPart))
			{
			case ovrAvatarRenderPartType_SkinnedMeshRender:
			{
				const ovrAvatarRenderPart_SkinnedMeshRender* mesh = ovrAvatarRenderPart_GetSkinnedMeshRender(renderPart);
				printTextures(mesh->materialState, textureMap, i, j);
				break;
			}
			case ovrAvatarRenderPartType_SkinnedMeshRenderPBS:
			{
				const ovrAvatarRenderPart_SkinnedMeshRenderPBS* mesh = ovrAvatarRenderPart_GetSkinnedMeshRenderPBS(renderPart);
				printTexture(textureMap, i, j, mesh->albedoTextureAssetID, "albedoTexture");
				printTexture(textureMap, i, j, mesh->surfaceTextureAssetID, "surfaceTexture");
				break;
			}
			case ovrAvatarRenderPartType_ProjectorRender:
			{
				const ovrAvatarRenderPart_ProjectorRender* projector = ovrAvatarRenderPart_GetProjectorRender(renderPart);
				printTextures(projector->materialState, textureMap, i, j);
				break;
			}
			case ovrAvatarRenderPartType_SkinnedMeshRenderPBS_V2:
			{
				//todo
				break;
			}
			case ovrAvatarRenderPartType_Count:
			{
				assert(false);
				break;
			}
			}
		}
	}
}
#endif


void GHOculusAvatar::handleAssetLoaded(const ovrAvatarMessage_AssetLoaded* message)
{
	ovrAvatarAssetType assetType = ovrAvatarAsset_GetType(message->asset);
	void* data = nullptr;

	// Call the appropriate loader function
	switch (assetType)
	{
		case ovrAvatarAssetType_Mesh:
			data = loadMesh(ovrAvatarAsset_GetMeshData(message->asset), message->assetID);
			break;
		case ovrAvatarAssetType_Texture:
			data = loadTexture(ovrAvatarAsset_GetTextureData(message->asset), message->assetID);
			break;
		case ovrAvatarAssetType_Pose:
		case ovrAvatarAssetType_Material:
		case ovrAvatarAssetType_PBSMaterial:
		case ovrAvatarAssetType_CombinedMesh:
			//todo
			break;
		case ovrAvatarAssetType_FailedLoad:
		    assert(false);
		    break;
		case ovrAvatarAssetType_Count:
			assert(false);
			break;
	}

	//todo: we probably want to save this data as explicit types (GHGeometryRenderable*, GHTexture*)
	mAssetMap[message->assetID] = data;
	--mNumAssetsLoading;

	if (isAvatarLoaded())
	{
#ifdef DEBUG_PRINT_TEXTURE_MAPPING
		printTextures(mAvatar, mTextureMap);
#endif
        createMaterials();
	}
}

void GHOculusAvatar::updateAvatarSkin(void)
{
	mAvatarModel->getSkeleton()->getLocalTransform() = mAvatarOrigin;

//debugging purposes, place the avatar so we can see it from the outside
#ifdef GH_SHOW_ENTIRE_AVATAR
	GHTransform rotate;
	mAvatarModel->getSkeleton()->getLocalTransform().becomeYRotation(3.14159f);
	GHPoint3 offsetTranslate;
	mAvatarOrigin.getTranslate(offsetTranslate);
	offsetTranslate += GHPoint3(0, 0, 2);
	mAvatarModel->getSkeleton()->getLocalTransform().setTranslate(offsetTranslate);
#endif

	//Hack: Todo: get rid of the GHOvrGo "floor offset" and instead bake it into the HMD origin
	// --anything that needs it is already offsetting by HMD origin anyway and places like this could
	// still be platform-independent
#ifdef GH_OVR_GO
	GHPoint3 floorOffset;
	mAvatarModel->getSkeleton()->getLocalTransform().getTranslate(floorOffset);
	floorOffset[1] += 1.7f;
	mAvatarModel->getSkeleton()->getLocalTransform().setTranslate(floorOffset);
#endif

	mComponentOverrider.cacheComponents(mAvatar, mAvatarModel->getSkeleton()->getLocalTransform());

	const ovrAvatarComponent* bodyComponent = nullptr;
	if (const ovrAvatarBodyComponent* body = ovrAvatarPose_GetBodyComponent(mAvatar))
	{
		bodyComponent = body->renderComponent;
	}

	uint32_t componentCount = ovrAvatarComponent_Count(mAvatar);
	for (uint32_t i = 0; i < componentCount; ++i)
	{
		const ovrAvatarComponent* component = ovrAvatarComponent_Get(mAvatar, i);
		GHTransform componentTrans;
		mComponentOverrider.getComponentTransform(component, componentTrans);
		
		const bool isCombinedMesh = bodyComponent == component;

		for (uint32_t j = 0; j < component->renderPartCount; ++j)
		{
			const ovrAvatarRenderPart* renderPart = component->renderParts[j];
			switch (ovrAvatarRenderPart_GetType(renderPart))
			{
				case ovrAvatarRenderPartType_SkinnedMeshRender:
					updateSkinnedMeshPart(ovrAvatarRenderPart_GetSkinnedMeshRender(renderPart), componentTrans, isCombinedMesh);
					break;
				case ovrAvatarRenderPartType_SkinnedMeshRenderPBS:
					updateSkinnedMeshPartPBS(ovrAvatarRenderPart_GetSkinnedMeshRenderPBS(renderPart), componentTrans);
					break;
				case ovrAvatarRenderPartType_ProjectorRender:
					updateProjectorPart(ovrAvatarRenderPart_GetProjectorRender(renderPart), componentTrans, isCombinedMesh);
					break;
				case ovrAvatarRenderPartType_SkinnedMeshRenderPBS_V2:
					//todo
					break;
				case ovrAvatarRenderPartType_Count:
					assert(false);
					break;
			}
		}
	}
}

void GHOculusAvatar::updateSkinnedMeshPart(const ovrAvatarRenderPart_SkinnedMeshRender* mesh, const GHTransform& worldTrans, bool useClothingTexture)
{
	if (!checkVisibility(mesh->visibilityMask, mesh->meshAssetID)) {
		return;
	}

	updateMeshSkin(mesh->localTransform, mesh->skinnedPose, worldTrans, mesh->meshAssetID);
	setMaterialState(mesh->materialState, mesh->meshAssetID, useClothingTexture);
}

void GHOculusAvatar::updateSkinnedMeshPartPBS(const ovrAvatarRenderPart_SkinnedMeshRenderPBS* mesh, const GHTransform& worldTrans)
{
	if (!checkVisibility(mesh->visibilityMask, mesh->meshAssetID)) {
		return;
	}

	updateMeshSkin(mesh->localTransform, mesh->skinnedPose, worldTrans, mesh->meshAssetID);

	auto iter = mSkeletonMap.find(mesh->meshAssetID);
	if (iter != mSkeletonMap.end())
	{
		AvatarMaterialStateCallbackSkinnedMeshPBS* materialCallback = iter->second.mMaterialStateCallbackPBS;
		if (materialCallback)
		{
			materialCallback->updateMaterialState(mesh);
		}
	}
}

bool GHOculusAvatar::checkVisibility(uint32_t visibilityMask, ovrAvatarAssetID assetID)
{
	bool isVisible = (visibilityMask & mVisibilityFlags) != 0;

	ShaderSkeleton& shaderSkel = mSkeletonMap[assetID];

	if (isVisible && !shaderSkel.mIsVisible)
	{
		mAvatarModel->getRenderable()->addGeometry(shaderSkel.mGeometry);
		shaderSkel.mIsVisible = true;
	}
	else if (shaderSkel.mIsVisible && !isVisible)
	{
		mAvatarModel->getRenderable()->removeGeometry(shaderSkel.mGeometry);
		shaderSkel.mIsVisible = false;
	}

	return isVisible;
}

void GHOculusAvatar::updateMeshSkin(const ovrAvatarTransform& localTransform, 
									const ovrAvatarSkinnedMeshPose& skinnedPose, 
									const GHTransform& worldTransform, ovrAvatarAssetID assetID)
{
	GHPROFILESCOPE("GHOculusAvatar::updateMeshSkin", GHString::CHT_REFERENCE)

	GHTransformNode* meshNode = mAvatarModel->getSkeleton()->findChild(GHIdentifier::IDType(assetID));
	assert(meshNode);

	//move the top level mesh node in the skeleton to the ovr mesh's transform in world space
	{
		GHTransform& ghMeshTransform = meshNode->getLocalTransform();
		GHOculusUtil::convertOvrAvatarTransformToGH(localTransform, ghMeshTransform);
		ghMeshTransform.mult(worldTransform, ghMeshTransform);
	}

	assert(mSkeletonMap.find(assetID) != mSkeletonMap.end());

	ShaderSkeleton& shaderSkel = mSkeletonMap[assetID];
	MeshSkeleton* skeleton = shaderSkel.mSkeleton;
	ModelTransformList* modelTransforms = shaderSkel.mModelTransforms;
	
	//get all skin matrix transforms in model space
	assert(skeleton && skeleton->size() >= skinnedPose.jointCount);
	for (uint32_t i = 0; i < skinnedPose.jointCount; ++i)
	{
		GHPoint16& ourTrans = (*skeleton)[i];
		GHOculusUtil::convertOvrAvatarTransformToGH(skinnedPose.jointTransform[i], ourTrans);

		(*modelTransforms)[i]->getLocalTransform() = ourTrans;

		int parentIndex = skinnedPose.jointParents[i];
		if (parentIndex >= 0)
		{
			const GHPoint16& parentTrans = (*skeleton)[parentIndex];
			GHTransform::mult(ourTrans, parentTrans, ourTrans);
		}
	}

	//apply inverse bind matrices
	TransformList* inverseSkel = shaderSkel.mBoneInversetransforms;
	for (uint32_t i = 0; i < skinnedPose.jointCount; ++i)
	{
		GHPoint16& ourTrans = (*skeleton)[i];
		const GHTransform& ibm = (*inverseSkel)[i];
		GHTransform::mult(ibm.getMatrix(), ourTrans, ourTrans);
	}
}

void GHOculusAvatar::updateProjectorPart(const ovrAvatarRenderPart_ProjectorRender* projectorPart, const GHTransform& worldTrans, bool useClothingTexture)
{
	const ovrAvatarComponent* component = ovrAvatarComponent_Get(mAvatar, projectorPart->componentIndex);
	const ovrAvatarRenderPart* renderPart = component->renderParts[projectorPart->renderPartIndex];
	const ovrAvatarRenderPart_SkinnedMeshRender* mesh = ovrAvatarRenderPart_GetSkinnedMeshRender(renderPart);

	if ((mesh->visibilityMask & mVisibilityFlags) == 0) { 
		return; 
	}
	updateMeshSkin(mesh->localTransform, mesh->skinnedPose, worldTrans, mesh->meshAssetID);
	//This line breaks the avatar rendering (invisible head). Todo: figure out how to actually draw the projector part
	//setMaterialState(projectorPart->materialState, mesh->meshAssetID, useClothingTexture);
	setMaterialState(mesh->materialState, mesh->meshAssetID, useClothingTexture);
}

void* GHOculusAvatar::loadMesh(const ovrAvatarMeshAssetData* data, const ovrAvatarAssetID& assetID)
{
	//return nullptr;
	GHIdentifier ourID((GHIdentifier::IDType)assetID);

	uint32_t vertexCount = data->vertexCount;
	const ovrAvatarMeshVertex* vertexBuffer = data->vertexBuffer;
	uint32_t indexCount = data->indexCount;
	const uint16_t* indexBuffer = data->indexBuffer;

	//ovrAvatarMeshVertex format: 68 bytes
	//[pos] [normal] [tangent]  [uv]   [blend indices]  [blend weights]
	// 3xf	  3xf		4xf		 2xf	   4xb				4xf
	// [12]   [12]      [16]     [8]       [4]				[16]

	//create geometry
	GHGeometry* geo = new GHGeometry;
	geo->setId(ourID);
	geo->setCloneType(GHGeometry::CT_CLONEVB); //GHAMModelLoader does this if a mesh is skinned

	//create vb description
	GHVBDescription vbDesc(data->vertexCount, data->indexCount, GHVBBlitter::BT_INDEX);
	GHVBDescription::StreamDescription streamDescription;
	streamDescription.mComps.push_back(GHVertexComponent::AP_POS);
	streamDescription.mComps.push_back(GHVertexComponent::AP_NORMAL);
	streamDescription.mComps.push_back(GHVertexComponent(GHVertexComponentShaderID::SI_TANGENT, GHVertexComponentType::CT_FLOAT, 4));
	streamDescription.mComps.push_back(GHVertexComponent::AP_UV1);
	streamDescription.mComps.push_back(GHVertexComponent(GHVertexComponentShaderID::SI_BONEIDX, GHVertexComponentType::CT_UBYTE, 4));
	streamDescription.mComps.push_back(GHVertexComponent(GHVertexComponentShaderID::SI_BONEWEIGHT, GHVertexComponentType::CT_FLOAT, 4));
	vbDesc.addStreamDescription(streamDescription);

	//create vertex buffer
	GHVertexBuffer* vb = mVBFactory.createVB(vbDesc);
	assert(vb);
	assert(vb->getStreams().size() == 1);
	GHVertexStream* stream = vb->getStreams()[0]->get();
	size_t vertexSize = stream->getFormat().getVertexSize() * sizeof(float);
	assert(vertexSize == sizeof(ovrAvatarMeshVertex));
	size_t vbSize = stream->getNumVerts() * vertexSize;

	float* vertData = stream->lockWriteBuffer();
	assert(vertData);
	//other things might break if our stride is not a multiple of sizeof(float)
	assert(sizeof(ovrAvatarMeshVertex) % sizeof(float) == 0);

	memcpy(vertData, data->vertexBuffer, vbSize);

	//convert positions to GH space
	//todo: convert normals and tangents too
	{
		ovrAvatarMeshVertex* modifiedVertexData = (ovrAvatarMeshVertex*)vertData;
		for (uint32_t v = 0; v < vertexCount; ++v, ++modifiedVertexData)
		{
			//positions
			GHOculusUtil::convertOvrAvatarVecToGH((const ovrAvatarVector3f&)modifiedVertexData->x, (GHPoint3&)modifiedVertexData->x);

			//normals
			GHOculusUtil::convertOvrAvatarVecToGH((const ovrAvatarVector3f&)modifiedVertexData->nx, (GHPoint3&)modifiedVertexData->nx);

			//tangents (note: not sure yet if we need to invert the w component. if stuff is funky with tangents, try that)
			GHOculusUtil::convertOvrAvatarVecToGH((const ovrAvatarVector3f&)modifiedVertexData->tx, (GHPoint3&)modifiedVertexData->tx);
		}
	}

	stream->unlockWriteBuffer();

	geo->setVB(new GHVertexBufferPtr(vb));

	//create index buffer
	GHVBBlitterIndex* ibBlitter = (GHVBBlitterIndex*)vb->getBlitter()->get();
	unsigned short* ibBuf = ibBlitter->lockWriteBuffer();
	assert(ibBlitter->getNumIndices() == data->indexCount);
	size_t ibSize = ibBlitter->getNumIndices()*sizeof(unsigned short);
	memcpy(ibBuf, data->indexBuffer, ibSize);
	ibBlitter->unlockWriteBuffer();

	//set up skeleton
	GHTransformNode* rootNode = mAvatarModel->getSkeleton();

	GHTransformNode* ourNode = new GHTransformNode();
	ourNode->setId(ourID);
	rootNode->addChild(ourNode);

	geo->setTransform(ourNode);

	const ovrAvatarSkinnedMeshPose& skinnedBindPose = data->skinnedBindPose;
	uint32_t jointCount = skinnedBindPose.jointCount;
	const ovrAvatarTransform* jointTransforms = skinnedBindPose.jointTransform;
	const int* jointParents = skinnedBindPose.jointParents;
	const char * const * jointNames = skinnedBindPose.jointNames;

	ShaderSkeleton& newSkeleton = mSkeletonMap[assetID];

	newSkeleton.mGeometry = geo;

	MeshSkeleton* meshSkeleton = new MeshSkeleton;
	newSkeleton.mSkeleton = meshSkeleton;

	TransformList* inverseSkeleton = new TransformList;
	newSkeleton.mBoneInversetransforms = inverseSkeleton;

	ModelTransformList* modelTransforms = new ModelTransformList;
	newSkeleton.mModelTransforms = modelTransforms;
	
	//must be max size. we pass a fixed size to the shader
	meshSkeleton->resize(OVR_AVATAR_MAXIMUM_JOINT_COUNT);
	inverseSkeleton->resize(jointCount);
	modelTransforms->resize(jointCount);
	for (uint32_t i = 0; i < jointCount; ++i)
	{
		GHPoint16& ourTrans = (*meshSkeleton)[i];
		GHOculusUtil::convertOvrAvatarTransformToGH(jointTransforms[i], ourTrans);

		//put GHTransformNodes in the model's skeleton (bone space)
		GHTransformNode* boneNode = new GHTransformNode;
		boneNode->getLocalTransform() = ourTrans;
		boneNode->setId(mIdFactory.generateHash(jointNames[i]));
		(*modelTransforms)[i] = boneNode;
		

		int parentIndex = jointParents[i];
		if (parentIndex >= 0)
		{
			const GHPoint16& parentTrans = (*meshSkeleton)[parentIndex];
			GHTransform::mult(ourTrans, parentTrans, ourTrans);
			(*modelTransforms)[parentIndex]->addChild(boneNode);
		}
		else
		{
			ourNode->addChild(boneNode);
		}

		GHTransform& inverseTrans = (*inverseSkeleton)[i];
		inverseTrans = ourTrans;
		inverseTrans.invert();
	}

	GHSphereBounds* bounds = new GHSphereBounds();
	GHGeoCreationUtil::calcSphereBounds(*vb, *bounds);
	geo->setBounds(bounds);

	//return a pointer to the data? or maybe decide the AssetMap isn't necessary (copied from mirror example)
	return nullptr;
}

void* GHOculusAvatar::loadTexture(const ovrAvatarTextureAssetData* data, const ovrAvatarAssetID& assetID)
{
	GHTexture* tex = nullptr;

#define LOAD_REAL_TEXTURES
#ifdef LOAD_REAL_TEXTURES
	assert(mTextureLoader != nullptr && "GHOculusAvatar::loadTexture called and we couldn't find a ovrtex loader");

	// figure out the gh texture type.
	GHTextureFormat::Enum textureFormat = GHTextureFormat::TF_UNKNOWN;
	bool noHeader = false;
	switch (data->format)
	{
	case ovrAvatarTextureFormat_RGB24:         ///< RGB 24bit uncompressed texture
		textureFormat = GHTextureFormat::TF_RGB8;
		break;
	case ovrAvatarTextureFormat_DXT1:          ///< DXT1 compressed texture
		textureFormat = GHTextureFormat::TF_DXT1;
		break;
	case ovrAvatarTextureFormat_DXT5:          ///< DXT5 compressed texture
		textureFormat = GHTextureFormat::TF_DXT5;
		break;
	case ovrAvatarTextureFormat_ASTC_RGB_6x6_DEPRECATED:
		textureFormat = GHTextureFormat::TF_ASTC_6x6;
		break;
	case ovrAvatarTextureFormat_ASTC_RGB_6x6_MIPMAPS:  ///< ASTC compressed texture. ASTC Header is included in texture data.
		textureFormat = GHTextureFormat::TF_ASTC_6x6;
		noHeader = true;
		break;
	case ovrAvatarTextureFormat_Count:
		assert(false);
		return nullptr;
		break;
	}
	assert(textureFormat != GHTextureFormat::TF_UNKNOWN); //unsupported

	GHPropertyContainer loadingProps;
	loadingProps.setProperty(GHRenderProperties::GP_TEXTUREFORMAT, textureFormat);
	loadingProps.setProperty(GHRenderProperties::GP_WIDTH, data->sizeX);
	loadingProps.setProperty(GHRenderProperties::GP_HEIGHT, data->sizeY);
	loadingProps.setProperty(GHRenderProperties::GP_NUMMIPS, data->mipCount);
	loadingProps.setProperty(GHRenderProperties::GP_NOHEADER, noHeader);
	// need to keep the texture data around for reload.
	loadingProps.setProperty(GHRenderProperties::GP_KEEPTEXTUREDATA, true);
	uint8_t* textureData = new uint8_t[(size_t)data->textureDataSize]; // Memory will ultimately be adopted by result GHTexture and released when it dies
	memcpy(textureData, data->textureData, (size_t)data->textureDataSize); 
	tex = (GHTexture*)mTextureLoader->loadMemory(textureData, (size_t)data->textureDataSize, &loadingProps);

	if (tex)
	{
		tex->acquire();
#ifdef GH_ENABLE_OCULUS_AVATAR_DEBUG_TEXTURE
		if (data->format == ovrAvatarTextureFormat_DXT5)
		{
			// debug texture output for gui display.
			// setting the pointer here will change the ** referenced by the "AvatarDebugTexture" callback.
			mDebugTexture = tex;
		}
#endif
		mTextureMap[assetID] = tex;
	}
#endif
	if (tex == nullptr)
	{
		//temporary dummy texture
		GHTexture* tex = (GHTexture*)mResourceFactory.getCacheResource("nsfrontwall.png");
		if (tex)
		{
			tex->acquire();
			mTextureMap[assetID] = tex;
		}
	}
	return 0;
}

GHMaterial* GHOculusAvatar::createMaterialForGeometry(ovrAvatarAssetID meshAssetID, const char* filename)
{
	auto skelIter = mSkeletonMap.find(meshAssetID);
	assert(skelIter != mSkeletonMap.end());
	if (skelIter == mSkeletonMap.end()) {
		return 0;
	}

	GHMaterial* newMaterial = (GHMaterial*)mXMLObjFactory.load(filename);
//	assert(newMaterial);
	if (!newMaterial) {
		return 0;
	}

	//todo: reenable this assert and figure out why it's getting triggered
	//assert(skelIter->second.mGeometry && !skelIter->second.mGeometry->getMaterial());

	skelIter->second.mGeometry->setMaterial(newMaterial);
	return newMaterial;
}


//typedef struct ovrAvatarMaterialLayerState_ {
//	ovrAvatarMaterialLayerBlendMode     blendMode;         ///< Blend mode of the material layer
//	ovrAvatarMaterialLayerSampleMode    sampleMode;        ///< Sample mode of the material layer
//	ovrAvatarMaterialMaskType			maskType;          ///< Mask type of the material layer
//	ovrAvatarVector4f					layerColor;        ///< Layer color
//	ovrAvatarVector4f                   sampleParameters;  ///< Parameters driving sample mode
//	ovrAvatarAssetID                    sampleTexture;     ///< Sample texture id (0 if unused)
//	ovrAvatarVector4f					sampleScaleOffset; ///< UV scale and offset parameters for the sample texture
//	ovrAvatarVector4f                   maskParameters;    ///< Parameters driving the layer mask
//	ovrAvatarVector4f                   maskAxis;          ///< Axis used by some mask types
//} ovrAvatarMaterialLayerState;


//typedef struct ovrAvatarMaterialState_ {
//	ovrAvatarVector4f			baseColor;               ///< Underlying base color for the material
//	ovrAvatarMaterialMaskType	baseMaskType;            ///< Mask type of the base color
//	ovrAvatarVector4f			baseMaskParameters;      ///< Parameters for the base mask type
//	ovrAvatarVector4f			baseMaskAxis;            ///< Axis used by some mask types
//	ovrAvatarAssetID            alphaMaskTextureID;      ///< Texture id for the base alpha mask (0 if unused)
//	ovrAvatarVector4f			alphaMaskScaleOffset;    ///< UV scale and offset parameters for the alpha mask
//	ovrAvatarAssetID			normalMapTextureID;      ///< Texture id for the normal map (0 if unused)
//	ovrAvatarVector4f			normalMapScaleOffset;    ///< UV scale and offset parameters for the normal map
//	ovrAvatarAssetID            parallaxMapTextureID;    ///< Texture id for the parallax map (0 if unused)
//	ovrAvatarVector4f			parallaxMapScaleOffset;  ///< UV scale and offset parameters for the parallax map
//	ovrAvatarAssetID			roughnessMapTextureID;   ///< Texture id for the roughness map (0 if unused)
//	ovrAvatarVector4f			roughnessMapScaleOffset; ///< UV scale and offset parameters for the roughness map
//	uint32_t                    layerCount;              ///< Number of layers
//	ovrAvatarMaterialLayerState layers[OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT]; ///< State for each material layer
//} ovrAvatarMaterialState;

void GHOculusAvatar::createMaterial(const ovrAvatarMaterialState& materialState, ovrAvatarAssetID meshAssetID, bool useClothingTexture)
{
	GHMaterial* material = createMaterialForGeometry(meshAssetID, mMaterialFilename.getChars());
	addMeshPoseToMaterial(material, meshAssetID);

	AvatarMaterialStateCallbackSkinnedMesh* materialCallback = new AvatarMaterialStateCallbackSkinnedMesh(mTextureMap, material, mAvatar, materialState, useClothingTexture);
	material->addCallback(GHMaterialCallbackType::CT_PERGEO, materialCallback);
	mSkeletonMap[meshAssetID].mMaterialStateCallback = materialCallback;
	
	//todo: look up GHTextures by asset ids from the materialState, apply to materials here
	//todo: apply other material state properties

	/*
	const char* textureParamHandles[] = 
	{
		"Texture0",
		"Texture1",
		"Texture2",
		"Texture3",
		"Texture4",
		"Texture5",
		"Texture6",
		"Texture7",
	};

	int textureIndex = 0; 
	for(uint32_t i = 0; i < materialState.layerCount; ++i)
	{
		const ovrAvatarMaterialLayerState& layerState = materialState.layers[i];
		if (addTextureToMaterial(layerState.sampleTexture, material, textureParamHandles[textureIndex]))
		{
			++textureIndex;
		}
	}
	

	addTextureToMaterial(materialState.alphaMaskTextureID, material, "alphaMaskTexture");
	addTextureToMaterial(materialState.normalMapTextureID, material, "normalMapTexture");
	addTextureToMaterial(materialState.parallaxMapTextureID, material, "parallaxMapTexture");
	addTextureToMaterial(materialState.roughnessMapTextureID, material, "roughnessMapTexture");
	//*/
}

void GHOculusAvatar::setMaterialState(const ovrAvatarMaterialState& materialState, ovrAvatarAssetID meshAssetID, bool useClothingTexture)
{
	//temp hack: not sure if we want to set the material state every frame or not
	// if we do, should optimize lookup of param handles
	bool doSetMaterialStateEveryFrame = true;
	if (!doSetMaterialStateEveryFrame)
	{
		return;
	}

	auto iter = mSkeletonMap.find(meshAssetID);
	if (iter->second.mMaterialStateCallback)
	{
		iter->second.mMaterialStateCallback->updateMaterialState(materialState, useClothingTexture);
	}
}

bool GHOculusAvatar::AvatarMaterialStateCallback::addParamToMaterial(GHMaterial* material, const void* value, GHMaterialParamHandle::HandleType handleType, const char* paramHandle)
{
	assert(material);
	if (!material) {
		return false;
	}

	GHMaterialParamHandle* param = material->getParamHandle(paramHandle);
	if (param)
	{
		//param->setValue(handleType, value);
		mParams.push_back(ParamHandle(param, handleType, value));
		return true;
	}
	else
	{
		GHDebugMessage::outputString("GHOculusAvatar::createMaterialParam could not find param handle for %s", paramHandle);
		return false;
	}
}

bool GHOculusAvatar::AvatarMaterialStateCallback::addTextureToMaterial(ovrAvatarAssetID textureAssetID, GHMaterial* material, const char* paramHandle)
{
	if (textureAssetID == 0) {
		return false;
	}

	assert(material);
	if (!material) {
		return false;
	}

	auto texIter = mTextureMap.find(textureAssetID);
	assert(texIter != mTextureMap.end());
	if (texIter == mTextureMap.end()) {
		return false;
	}

	GHMaterialParamHandle* param = material->getParamHandle(paramHandle);
	if (param)
	{
		//GHMaterialParamHandle::TextureHandle texHandle(texIter->second, GHMDesc::WM_WRAP);
		//param->setValue(GHMaterialParamHandle::HT_TEXTURE, &texHandle);
		mTextureParams.push_back(TextureHandle(param, texIter->second));
		return true;
	}
	else
	{
		GHDebugMessage::outputString("GHOculusAvatar::addTextureToMaterial could not find param handle for %s", paramHandle);
		return false;
	}
}

void GHOculusAvatar::addMeshPoseToMaterial(GHMaterial* material, ovrAvatarAssetID meshAssetID)
{
	if (!material) {
		return;
	}
	auto iter = mSkeletonMap.find(meshAssetID);
	if (iter == mSkeletonMap.end())
	{
		GHDebugMessage::outputString("GHOculusAvatar::addMeshPoseToMaterial could not find skeleton to pass to shader");
		return;
	}

	MeshSkeleton* skeleton = iter->second.mSkeleton;

	delete iter->second.mParamHandle;

	//const char* paramHandle = "MeshPose";
	const char* paramHandle = "meshPose";
	GHMaterialParamHandle* param = material->getParamHandle(paramHandle);
	if (param)
	{
		iter->second.mParamHandle = param;
		SkeletonMaterialCallback* materialCallback = new SkeletonMaterialCallback(iter->second);
		material->addCallback(GHMaterialCallbackType::CT_PERGEO, materialCallback);
	}
	else
	{
		GHDebugMessage::outputString("GHOculusAvatar::addMeshPoseToMaterial could not find param handle for %s", paramHandle);
		iter->second.mParamHandle = 0;
	}
}

void GHOculusAvatar::createPBSMaterial(const ovrAvatarRenderPart_SkinnedMeshRenderPBS* mesh)
{
	GHMaterial* material = createMaterialForGeometry(mesh->meshAssetID, mPBSMaterialFilename.getChars());
	addMeshPoseToMaterial(material, mesh->meshAssetID);

	AvatarMaterialStateCallbackSkinnedMeshPBS* materialCallback = new AvatarMaterialStateCallbackSkinnedMeshPBS(mTextureMap, material, mesh);
	material->addCallback(GHMaterialCallbackType::CT_PERGEO, materialCallback);
	mSkeletonMap[mesh->meshAssetID].mMaterialStateCallbackPBS = materialCallback;
} 

void GHOculusAvatar::createMaterials(void)
{
	// for debugging that assert
	if (false)
	{

	uint32_t componentCount = ovrAvatarComponent_Count(mAvatar);
	for (uint32_t i = 0; i < componentCount; ++i)
	{
		const ovrAvatarComponent* component = ovrAvatarComponent_Get(mAvatar, i);
		GHTransform worldTrans;
		GHOculusUtil::convertOvrAvatarTransformToGH(component->transform, worldTrans);
		for (uint32_t j = 0; j < component->renderPartCount; ++j)
		{
			const ovrAvatarRenderPart* renderPart = component->renderParts[j];
			switch (ovrAvatarRenderPart_GetType(renderPart))
			{
			case ovrAvatarRenderPartType_SkinnedMeshRender:
			{
				const ovrAvatarRenderPart_SkinnedMeshRender* mesh = ovrAvatarRenderPart_GetSkinnedMeshRender(renderPart);
				GHDebugMessage::outputString("component %d/%x, render part %d/%x, asset id %llx skinnedMeshRender", i, component, j, renderPart, mesh->meshAssetID);
				break;
			}
			case ovrAvatarRenderPartType_SkinnedMeshRenderPBS:
			{
				const ovrAvatarRenderPart_SkinnedMeshRenderPBS* mesh = ovrAvatarRenderPart_GetSkinnedMeshRenderPBS(renderPart);
				GHDebugMessage::outputString("component %d/%x, render part %d/%x, asset id %llx skinnedMeshRenderPBS", i, component, j, renderPart, mesh->meshAssetID);
				break;
			}
			case ovrAvatarRenderPartType_ProjectorRender:
			{
				const ovrAvatarRenderPart_ProjectorRender* projector = ovrAvatarRenderPart_GetProjectorRender(renderPart);
				GHDebugMessage::outputString("component %d/%x, render part %d/%x, asset id %llx projector", i, component, j, renderPart);
				break;
			}
			case ovrAvatarRenderPartType_SkinnedMeshRenderPBS_V2:
			{
				const ovrAvatarRenderPart_SkinnedMeshRenderPBS_V2* mesh = ovrAvatarRenderPart_GetSkinnedMeshRenderPBSV2(renderPart);
				GHDebugMessage::outputString("component %d/%x, render part %d/%x, asset id %llx skinnedMeshRenderPBS_V2", i, component, j, renderPart, mesh->meshAssetID);
				break;
			}
			case ovrAvatarRenderPartType_Count:
			{
				GHDebugMessage::outputString("component %d/%x, render part %d/%x was somehow ovrAvatarRenderPartType_Count", i, component, j, renderPart);
				break;
			}
			}
		}
	}
	}


	uint32_t componentCount = ovrAvatarComponent_Count(mAvatar);
	for (uint32_t i = 0; i < componentCount; ++i)
	{
		const ovrAvatarComponent* component = ovrAvatarComponent_Get(mAvatar, i);
		GHTransform worldTrans;
		GHOculusUtil::convertOvrAvatarTransformToGH(component->transform, worldTrans);

		const ovrAvatarComponent* bodyComponent = nullptr;
		if (const ovrAvatarBodyComponent* body = ovrAvatarPose_GetBodyComponent(mAvatar))
		{
			bodyComponent = body->renderComponent;
		}

		for (uint32_t j = 0; j < component->renderPartCount; ++j)
		{
			const ovrAvatarRenderPart* renderPart = component->renderParts[j];
			const bool isCombinedMesh = bodyComponent == component;

 			switch (ovrAvatarRenderPart_GetType(renderPart))
			{
				case ovrAvatarRenderPartType_SkinnedMeshRender:
				{
					const ovrAvatarRenderPart_SkinnedMeshRender* mesh = ovrAvatarRenderPart_GetSkinnedMeshRender(renderPart);
					createMaterial(mesh->materialState, mesh->meshAssetID, isCombinedMesh);
					break;
				}
				case ovrAvatarRenderPartType_SkinnedMeshRenderPBS:
				{
					const ovrAvatarRenderPart_SkinnedMeshRenderPBS* mesh = ovrAvatarRenderPart_GetSkinnedMeshRenderPBS(renderPart);
					createPBSMaterial(mesh);
					break;
				}
				case ovrAvatarRenderPartType_ProjectorRender:
				{
					const ovrAvatarRenderPart_ProjectorRender* projector = ovrAvatarRenderPart_GetProjectorRender(renderPart);
					//Creating an extra material for the projector causes badness. Todo: figure out how to create the proper material for the projector part
					//const ovrAvatarComponent* projectorComp = ovrAvatarComponent_Get(mAvatar, projector->componentIndex);
					//const ovrAvatarRenderPart* projectorPart = projectorComp->renderParts[projector->renderPartIndex];
					//const ovrAvatarRenderPart_SkinnedMeshRender * projectorMesh = ovrAvatarRenderPart_GetSkinnedMeshRender(projectorPart);
					//createMaterial(projector->materialState, projectorMesh->meshAssetID, isCombinedMesh);
					break;
				}
				case ovrAvatarRenderPartType_SkinnedMeshRenderPBS_V2:
				{
					//todo
					break;
				}
				case ovrAvatarRenderPartType_Count:
				{
					assert(false);
					break;
				}
			}
		}
	}
}

bool GHOculusAvatar::isAvatarLoaded(void) const
{
	return mAvatar && mNumAssetsLoading == 0;
}

GHOculusAvatar::SkeletonMaterialCallback::SkeletonMaterialCallback(ShaderSkeleton& shaderSkeleton)
	: mShaderSkeleton(shaderSkeleton)
{
}

void GHOculusAvatar::SkeletonMaterialCallback::apply(const void* data)
{
	//pass to shader
	GHMaterialParamHandle* param = mShaderSkeleton.mParamHandle;
	MeshSkeleton* skeleton = mShaderSkeleton.mSkeleton;
	if (param && skeleton)
	{
		GHMaterialParamHandle::HandleType handleType = GHMaterialParamHandle::HT_MAT16x64;
		//mat16x64 is currently a type unto itself. possible todo: turn it into mat16 with count 64
		assert(skeleton->size() * sizeof(skeleton->front()) == GHMaterialParamHandle::calcHandleSizeBytes(handleType));
		param->setValue(handleType, &skeleton->front());
	}
}

GHOculusAvatar::AvatarMaterialStateCallback::AvatarMaterialStateCallback(const std::map<ovrAvatarAssetID, GHTexture*>& textureMap)
	: mTextureMap(textureMap)
{

}

GHOculusAvatar::AvatarMaterialStateCallback::~AvatarMaterialStateCallback(void)
{
	for (ParamHandle& ph : mParams)
	{
		delete ph.mHandle;
	}

	for (TextureHandle& th : mTextureParams)
	{
		delete th.mHandle;
	}
}

void GHOculusAvatar::AvatarMaterialStateCallback::apply(const void* data)
{
	for (ParamHandle& ph : mParams)
	{
		ph.mHandle->setValue(ph.mType, ph.mData);
	}

	for (TextureHandle& th : mTextureParams)
	{
		GHMaterialParamHandle::TextureHandle texHandle(th.mTexture, GHMDesc::WM_WRAP);
		th.mHandle->setValue(GHMaterialParamHandle::HT_TEXTURE, &texHandle);
	}
}

GHOculusAvatar::AvatarMaterialStateCallbackSkinnedMesh::AvatarMaterialStateCallbackSkinnedMesh(const std::map<ovrAvatarAssetID, GHTexture*>& textureMap, GHMaterial* material, ovrAvatar* avatar,
																							   const ovrAvatarMaterialState& materialState, bool useClothingTexture)
	: AvatarMaterialStateCallback(textureMap)
{
	if (useClothingTexture)
	{
		ovrAvatar_GetCombinedMeshAlphaData(avatar, &mAvatarCombinedMeshAlpha, &mAvatarCombinedMeshAlphaOffset);
	}

	updateMaterialState(materialState, useClothingTexture);
	createMaterialHandles(avatar, material);
	apply(nullptr);
}

void GHOculusAvatar::AvatarMaterialStateCallbackSkinnedMesh::updateMaterialState(const ovrAvatarMaterialState& materialState, bool useClothingTexture)
{
	mOvrMaterialState = materialState;
	mUseClothingTexture = useClothingTexture;

	if (materialState.alphaMaskTextureID != 0) {
		mUseAlphaFlag = 1;
	}
	if (useClothingTexture) {
		mUseAlphaFlag = 2;
	}
	mUseNormalMap = mOvrMaterialState.normalMapTextureID != 0;
	mUseRoughnessMap = mOvrMaterialState.roughnessMapTextureID != 0;

	//todo: projector (only for rendering "projector" types):
	mUseProjector = 0;
	mProjectorInv.becomeIdentity();
	if (mUseProjector != 0)
	{
		//calculate projector inv. Example:
		//glm::mat4 projection;
		//_glmFromOvrAvatarTransform(projector->localTransform, &projection);
		//glm::mat4 worldProjection = world * projection;
		//glm::mat4 projectionInv = glm::inverse(worldProjection);
	}

	mLayerCount = materialState.layerCount;
	memset(&mLayerUniforms, 0, sizeof(mLayerUniforms));
	for (uint32_t i = 0; i < materialState.layerCount; ++i)
	{
		const ovrAvatarMaterialLayerState& layerState = materialState.layers[i];
		mLayerUniforms.mLayerSamplerModes[i] = layerState.sampleMode;
		mLayerUniforms.mLayerBlendModes[i] = layerState.blendMode;
		mLayerUniforms.mLayerMaskTypes[i] = layerState.maskType;
		mLayerUniforms.mLayerColors[i] = layerState.layerColor;
		mLayerUniforms.mLayerSurfaceIDs[i] = layerState.sampleTexture;
		mLayerUniforms.mLayerSurfaceScaleOffsets[i] = layerState.sampleScaleOffset;
		mLayerUniforms.mLayerSampleParameters[i] = layerState.sampleParameters;
		mLayerUniforms.mLayerMaskParameters[i] = layerState.maskParameters;
		mLayerUniforms.mLayerMaskAxes[i] = layerState.maskAxis;
	}
}

void GHOculusAvatar::AvatarMaterialStateCallbackSkinnedMesh::createMaterialHandles(ovrAvatar* avatar, GHMaterial* material)
{
	GHMaterialParamHandle::HandleType HT_INT = GHMaterialParamHandle::HT_INT;
	// bools passed as floats. Make HT_BOOL local variable so it's easy to see which things are bools
	GHMaterialParamHandle::HandleType HT_BOOL = GHMaterialParamHandle::HT_FLOAT;
	
	addParamToMaterial(material, &mUseAlphaFlag, HT_BOOL, "useAlpha");
	addParamToMaterial(material, &mUseNormalMap, HT_BOOL, "useNormalMap");
	addParamToMaterial(material, &mUseRoughnessMap, HT_BOOL, "useRoughnessMap");

	//elapsedSeconds -> renamed to Time, updated through time callback

	//(only for rendering "projector" types):
	if (mUseProjector)
	{
		addParamToMaterial(material, mProjectorInv.getMatrix().getCoords(), GHMaterialParamHandle::HT_MAT16, "projectorInv");
	}
	addParamToMaterial(material, &mUseProjector, HT_BOOL, "useProjector");


	addParamToMaterial(material, &mOvrMaterialState.baseColor, GHMaterialParamHandle::HT_VEC4, "baseColor");
	addParamToMaterial(material, &mOvrMaterialState.baseMaskType, HT_INT, "baseMaskType");
	addParamToMaterial(material, &mOvrMaterialState.baseMaskParameters, GHMaterialParamHandle::HT_VEC4, "baseMaskParameters");
	addParamToMaterial(material, &mOvrMaterialState.baseMaskAxis, GHMaterialParamHandle::HT_VEC4, "baseMaskAxis");
	addTextureToMaterial(mOvrMaterialState.alphaMaskTextureID, material, "alphaMaskTexture");
	addParamToMaterial(material, &mOvrMaterialState.alphaMaskScaleOffset, GHMaterialParamHandle::HT_VEC4, "alphaMaskScaleOffset");

	//todo after updating api
	//_setTextureSampler(program, textureSlot++, "clothingAlpha", _avatarCombinedMeshAlpha); 
	//glUniform4fv(glGetUniformLocation(program, "clothingAlphaScaleOffset"), 1, &_avatarCombinedMeshAlphaOffset.x);

	if (mUseClothingTexture)
	{
		addTextureToMaterial(mAvatarCombinedMeshAlpha, material, "clothingAlphaMaskTexture");
		addParamToMaterial(material, &mAvatarCombinedMeshAlphaOffset, GHMaterialParamHandle::HT_VEC4, "clothingAlphaMaskScaleOffset");
	}


	addTextureToMaterial(mOvrMaterialState.normalMapTextureID, material, "normalMapTexture");
	addParamToMaterial(material, &mOvrMaterialState.normalMapScaleOffset, GHMaterialParamHandle::HT_VEC4, "normalMapScaleOffset");

	addTextureToMaterial(mOvrMaterialState.parallaxMapTextureID, material, "parallaxMapTexture");
	addParamToMaterial(material, &mOvrMaterialState.parallaxMapScaleOffset, GHMaterialParamHandle::HT_VEC4, "parallaxMapScaleOffset");

	addTextureToMaterial(mOvrMaterialState.roughnessMapTextureID, material, "roughnessMapTexture");
	addParamToMaterial(material, &mOvrMaterialState.roughnessMapScaleOffset, GHMaterialParamHandle::HT_VEC4, "roughnessMapScaleOffset");

	addParamToMaterial(material, &mLayerCount, HT_INT, "layerCount");
	addParamToMaterial(material, &mLayerUniforms.mLayerSamplerModes, HT_INT, "layerSamplerModes");
	addParamToMaterial(material, &mLayerUniforms.mLayerBlendModes, HT_INT, "layerBlendModes");
	addParamToMaterial(material, &mLayerUniforms.mLayerMaskTypes, HT_INT, "layerMaskTypes");
	addParamToMaterial(material, &mLayerUniforms.mLayerColors, GHMaterialParamHandle::HT_VEC4, "layerColors");
	addParamToMaterial(material, &mLayerUniforms.mLayerSurfaceScaleOffsets, GHMaterialParamHandle::HT_VEC4, "layerSurfaceScaleOffsets");
	addParamToMaterial(material, &mLayerUniforms.mLayerSampleParameters, GHMaterialParamHandle::HT_VEC4, "layerSampleParameters");
	addParamToMaterial(material, &mLayerUniforms.mLayerMaskParameters, GHMaterialParamHandle::HT_VEC4, "layerMaskParameters");
	addParamToMaterial(material, &mLayerUniforms.mLayerMaskAxes, GHMaterialParamHandle::HT_VEC4, "layerMaskAxes");

	const char* textureParamHandles[] =
	{
		"layerSurfaceTextures",
		"layerSurfaceTextures2",
		"layerSurfaceTextures3",
		"layerSurfaceTextures4",
		"layerSurfaceTextures5",
		"layerSurfaceTextures6",
		"layerSurfaceTextures7",
		"layerSurfaceTextures8",
	};
	for (int i = 0; i < mLayerCount; ++i)
	{
		addTextureToMaterial(mLayerUniforms.mLayerSurfaceIDs[i], material, textureParamHandles[i]);
	}
}

GHOculusAvatar::AvatarMaterialStateCallbackSkinnedMeshPBS::AvatarMaterialStateCallbackSkinnedMeshPBS(const std::map<ovrAvatarAssetID, GHTexture*>& textureMap,
																									 GHMaterial* material,
																									 const ovrAvatarRenderPart_SkinnedMeshRenderPBS* mesh)
	: AvatarMaterialStateCallback(textureMap)
{
	updateMaterialState(mesh);
	createMaterialHandles(material);
	apply(nullptr);
}

void GHOculusAvatar::AvatarMaterialStateCallbackSkinnedMeshPBS::updateMaterialState(const ovrAvatarRenderPart_SkinnedMeshRenderPBS* mesh)
{
	mAlbedoTextureAssetID = mesh->albedoTextureAssetID;
	mSurfaceTextureAssetID = mesh->surfaceTextureAssetID;
}

void GHOculusAvatar::AvatarMaterialStateCallbackSkinnedMeshPBS::createMaterialHandles(GHMaterial* material)
{
	addTextureToMaterial(mAlbedoTextureAssetID, material, "albedoTexture");
	addTextureToMaterial(mSurfaceTextureAssetID, material, "surfaceTexture");
}
