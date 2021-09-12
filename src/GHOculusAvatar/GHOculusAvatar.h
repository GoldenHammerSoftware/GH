#pragma once

#include "GHUtils/GHController.h"
#include "OVR_Avatar.h"
#include <map>
#include <vector>
#include "GHOculusAvatarDebugDrawHands.h"
#include "GHString/GHString.h"
#include "GHMath/GHPoint.h"
#include "GHOculusAvatarCustomGestureSetter.h"
#include "GHMaterialParamHandle.h"
#include "GHMaterialCallback.h"
#include "GHMath/GHTransform.h"
#include "GHRenderDeviceReinitListener.h"
#include "GHOculusAvatarComponentOverride.h"

class GHRenderGroup;
class GHModel;
class GHStringIdFactory;
class GHVBFactory;
class GHMaterial;
class GHTransformNode;
class GHXMLObjFactory;
class GHGeometry;
class GHRenderServices;
class GHTexture;
class GHMaterialParamHandle;
class GHOculusAvatarCustomGestureFinder;
class GHInputState;
class GHXMLSerializer;
class GHEventMgr;
class GHResourceFactory;
class GHResourceLoader;
class GHMaterialCallbackMgr;
class GHOculusAvatarPlatformPoser;

//todo: handle networked avaters (ovrAvatar_UpdatePosefromPacket)
class GHOculusAvatar : public GHController
{
public:
	GHOculusAvatar(uint64_t userId,
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
				   const char* gestureMapFilename);
	~GHOculusAvatar(void);

	virtual void onActivate(void);
	virtual void onDeactivate(void);
	virtual void update(void);

	void onDeviceReinit(void) { mDeviceReinitHappened = true; }

	//for loading
	GHOculusAvatarComponentOverride& getComponentOverride(void) { return mComponentOverrider; }

private:
	void updateAvatar(void);
	void runMessageQueue(void);
	void handleAvatarSpecification(const ovrAvatarMessage_AvatarSpecification* message);
	void handleAssetLoaded(const ovrAvatarMessage_AssetLoaded* message);
	void* loadMesh(const ovrAvatarMeshAssetData* data, const ovrAvatarAssetID& assetID);
	void* loadTexture(const ovrAvatarTextureAssetData* data, const ovrAvatarAssetID& assetID);
	bool isAvatarLoaded(void) const;

	void updateAvatarSkin(void);
	void updateSkinnedMeshPart(const ovrAvatarRenderPart_SkinnedMeshRender* mesh, const GHTransform& worldTrans, bool useClothingTexture);
	void updateSkinnedMeshPartPBS(const ovrAvatarRenderPart_SkinnedMeshRenderPBS* mesh, const GHTransform& worldTrans);
	void updateProjectorPart(const ovrAvatarRenderPart_ProjectorRender* projectorPart, const GHTransform& worldTrans, bool useClothingTexture);
	void updateMeshSkin(const ovrAvatarTransform& localTransform, const ovrAvatarSkinnedMeshPose& skinnedPose, const GHTransform& worldTransform, ovrAvatarAssetID assetID);
	bool checkVisibility(uint32_t visibilityMask, ovrAvatarAssetID assetID);

	void createMaterials(void);
	void createMaterial(const ovrAvatarMaterialState& materialState, ovrAvatarAssetID meshAssetID, bool useClothingTexture);
	void createPBSMaterial(const ovrAvatarRenderPart_SkinnedMeshRenderPBS* mesh);
	GHMaterial* createMaterialForGeometry(ovrAvatarAssetID meshAssetID, const char* filename);
	void addMeshPoseToMaterial(GHMaterial* material, ovrAvatarAssetID meshAssetID);
	
	//todo: call this on a per-frame basis? maybe that's handled by the material system
	void setMaterialState(const ovrAvatarMaterialState& materialState, ovrAvatarAssetID meshAssetID, bool useClothingTexture);

private:
	uint64_t mUserId;
	ovrAvatar* mAvatar{ nullptr };
	GHOculusAvatarCustomGestureSetter mGestureSetter;
	GHOculusAvatarComponentOverride mComponentOverrider;
	GHString mMaterialFilename;
	GHString mPBSMaterialFilename;
	GHOculusAvatarPlatformPoser* mPlatformPoser{ nullptr };
	GHVBFactory& mVBFactory;
	const GHXMLObjFactory& mXMLObjFactory;
	GHResourceFactory& mResourceFactory;
	const GHStringIdFactory& mIdFactory;
	const GHTransform& mAvatarOrigin;
	GHModel* mAvatarModel { nullptr };
	ovrAvatarVisibilityFlags mVisibilityFlags { ovrAvatarVisibilityFlag_FirstPerson };
	GHOculusAvatarDebugDrawHands* mDebugDrawHands { nullptr };
	GHOculusAvatarCustomGestureFinder* mCustomGestureFinder { nullptr };
	std::map<ovrAvatarAssetID, void*> mAssetMap;
	GHRenderDeviceReinitListener<GHOculusAvatar> mDeviceReinitListener;

	typedef std::vector<GHTransformNode*> ModelTransformList;

	typedef std::vector<GHPoint16> MeshSkeleton;
	typedef std::vector<GHTransform> TransformList;

	class AvatarMaterialStateCallbackSkinnedMesh;
	class AvatarMaterialStateCallbackSkinnedMeshPBS;

	//separate skeleton for each mesh. we own these and are responsible for deleting them
	struct ShaderSkeleton
	{
		MeshSkeleton* mSkeleton { 0 };
		TransformList* mBoneInversetransforms { 0 };
		GHMaterialParamHandle* mParamHandle { 0 };
		ModelTransformList* mModelTransforms { 0 }; //we are not responsible for deleting these. these are handy pointers into the model for communicating our bone transforms to game code. they are owned by the model (we still need to delete the vector)
		GHGeometry* mGeometry { 0 }; //owned by the mesh if mIsVisible is true
		union
		{
			AvatarMaterialStateCallbackSkinnedMesh* mMaterialStateCallback{ 0 };
			AvatarMaterialStateCallbackSkinnedMeshPBS* mMaterialStateCallbackPBS;
		};
		bool mIsVisible { false };
	};
	typedef std::map<ovrAvatarAssetID, ShaderSkeleton> SkeletonMap;
	SkeletonMap mSkeletonMap; 
	SkeletonMap mProjectorSkeletonMap;
	
	//for creating the materials after the textures are loaded 
	GHResourceLoader* mTextureLoader { 0 };
	std::map<ovrAvatarAssetID, GHTexture*> mTextureMap;
	int mNumAssetsLoading { 0 };

	// Debug texture for displaying to the gui.
	// We will pipe this to anything that asks for "AvatarDebugTexture"
	GHTexture* mDebugTexture{ nullptr };

	bool mDeviceReinitHappened{ false };

private:
	class SkeletonMaterialCallback : public GHMaterialCallback
	{
	public:
		SkeletonMaterialCallback(ShaderSkeleton& shaderSkeleton);

		virtual void apply(const void* data);

	private:
		ShaderSkeleton& mShaderSkeleton;
	};

	class AvatarMaterialStateCallback : public GHMaterialCallback
	{
	public:
		AvatarMaterialStateCallback(const std::map<ovrAvatarAssetID, GHTexture*>& textureMap);
		~AvatarMaterialStateCallback(void);

		virtual void apply(const void* data);

	protected:
		bool addParamToMaterial(GHMaterial* material, const void* value, GHMaterialParamHandle::HandleType handleType, const char* paramHandle);
		bool addTextureToMaterial(ovrAvatarAssetID textureAssetID, GHMaterial* material, const char* paramHandle);

	private:
		const std::map<ovrAvatarAssetID, GHTexture*>& mTextureMap;

		struct ParamHandle
		{
			GHMaterialParamHandle* mHandle{ 0 };
			GHMaterialParamHandle::HandleType mType;
			const void* mData{ 0 };
			ParamHandle(GHMaterialParamHandle* handle, GHMaterialParamHandle::HandleType type, const void* data)
				: mHandle(handle), mType(type), mData(data)
			{}
		};
		struct TextureHandle
		{
			GHMaterialParamHandle* mHandle{ 0 };
			GHTexture* mTexture;
			TextureHandle(GHMaterialParamHandle* handle, GHTexture* texture)
				: mHandle(handle), mTexture(texture)
			{}
		};

		std::vector<ParamHandle> mParams;
		std::vector<TextureHandle> mTextureParams;
	};

	class AvatarMaterialStateCallbackSkinnedMesh : public AvatarMaterialStateCallback
	{
	public:
		AvatarMaterialStateCallbackSkinnedMesh(const std::map<ovrAvatarAssetID, GHTexture*>& textureMap, GHMaterial* material, ovrAvatar* avatar,
											   const ovrAvatarMaterialState& materialState, bool useClothingTexture);

		void updateMaterialState(const ovrAvatarMaterialState& materialState, bool useClothingTexture);

		void createMaterialHandles(ovrAvatar* avatar, GHMaterial* material);

	protected:
		typedef float floatBool;

		ovrAvatarMaterialState mOvrMaterialState;
		floatBool mUseAlphaFlag{ 0 };
		floatBool mUseNormalMap{ 0 };
		floatBool mUseRoughnessMap{ 0 };
		floatBool mUseProjector{ 0 };
		GHTransform mProjectorInv;
		ovrAvatarVector4f mAvatarCombinedMeshAlphaOffset{ 0,0,0,0 };
		ovrAvatarAssetID mAvatarCombinedMeshAlpha{ 0 };

		struct LayerUniforms {
			int mLayerSamplerModes[OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT];
			int mLayerBlendModes[OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT];
			int mLayerMaskTypes[OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT];
			ovrAvatarVector4f mLayerColors[OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT];
			ovrAvatarAssetID mLayerSurfaceIDs[OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT];
			ovrAvatarVector4f mLayerSurfaceScaleOffsets[OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT];
			ovrAvatarVector4f mLayerSampleParameters[OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT];
			ovrAvatarVector4f mLayerMaskParameters[OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT];
			ovrAvatarVector4f mLayerMaskAxes[OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT];
		} mLayerUniforms;
		int mLayerCount{ 0 };

		bool mUseClothingTexture{ false };
	};

	class AvatarMaterialStateCallbackSkinnedMeshPBS : public AvatarMaterialStateCallback
	{
	public:
		AvatarMaterialStateCallbackSkinnedMeshPBS(const std::map<ovrAvatarAssetID, GHTexture*>& textureMap,
												  GHMaterial* material,
												  const ovrAvatarRenderPart_SkinnedMeshRenderPBS* mesh);

		void updateMaterialState(const ovrAvatarRenderPart_SkinnedMeshRenderPBS* mesh);

		void createMaterialHandles(GHMaterial* material);

	private:
		ovrAvatarAssetID mAlbedoTextureAssetID;
		ovrAvatarAssetID mSurfaceTextureAssetID;
	};
};
