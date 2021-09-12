// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHString.h"
#include <vector>
#include "GHRenderPassMembership.h"
#include "GHString/GHIdentifier.h"
#include <map>

class GHTexture;
class GHMaterial;
class GHMaterialParamHandle;

// wraps info that would be used to render a GHM material
class GHMDesc
{
public:
	enum VertexMode {
		VM_VERTEXCOLOR=0,
		VM_VERTEXALPHA
	};
	enum CullMode {
		CM_NOCULL=0,
		CM_CCW, 
		CM_CW
	};
	enum WrapMode {
		WM_WRAP=0,
		WM_CLAMP,
	};
	enum BlendMode {
		BM_ALPHA=0,
		BM_INVALPHA,
		BM_ONE
	};
    enum BillboardType {
        BT_NONE=0,
        BT_FULL,
        BT_UP
    };

    class TextureInfo
    {
    public:
        TextureInfo(const char* handle, GHTexture* texture, WrapMode wrapMode);
        TextureInfo(const TextureInfo& other);
        ~TextureInfo(void);
    
        GHTexture* getTexture(void) const { return mTexture; }
        const char* getHandle(void) const { return mHandle; }
        WrapMode getWrapMode(void) const { return mWrapMode; }
        
    private:
        GHTexture* mTexture;
        GHString mHandle;
        WrapMode mWrapMode;
    };
    
    class FloatArg
    {
    public:
        FloatArg(const char* handle, unsigned int count, float* values);
        FloatArg(const FloatArg& other);
        ~FloatArg(void);
        
        const char* getHandle(void) const { return mHandle; }
        unsigned int getCount(void) const { return mCount; }
        float* getValues(void) const { return mValues; }
        
    private:
        GHString mHandle;
        unsigned int mCount;
        float* mValues;
    };
    
	// Param wrapper for one material and this GHMDesc.
	// Used to avoid looking up param handles every frame.
	class ParamHandles
	{
	public:
		// Making this a friend so ParamHandles can be opaque to the outside world.
		friend GHMDesc;

		~ParamHandles(void);

	private:
		// Params should be stored in the same order as GHMDesc::mTextures and GHMDesc::mFloatArgs.
		std::vector<GHMaterialParamHandle*> mTextureParamHandles;
		std::vector<GHMaterialParamHandle*> mFloatParamHandles;
	};

public:
    GHMDesc(void);
	// copy any values from another GHMDesc
	GHMDesc(const GHMDesc& other);
	~GHMDesc(void);
    
    // do any initialization-time modifications to the material.
    // example: pass membership, pass overrides.
    // also applyDefaultArgs.
	// Returns a param handles that is expected to be passed in to applyDefaultArgs.
	GHMDesc::ParamHandles* initMaterial(GHMaterial& mat) const;
    
    // apply any non-texture shader params to the mat
    void applyDefaultArgs(GHMaterial& mat, const GHMDesc::ParamHandles& handles) const;
	// apply the texture params to the mat
	void applyTextures(GHMaterial& mat, const GHMDesc::ParamHandles& handles, bool isOverrideMat=false) const;

	// if we have an alias for paramName return
	const char* checkParamAlias(const char* paramName);

public:
    GHString mVertexFile;
    GHString mPixelFile;
    BillboardType mBillboard;
    CullMode mCullMode;
    float mDrawOrder;
    bool mZRead;
    bool mZWrite;
    float mZOffset;
    bool mAlphaBlend;
    BlendMode mSrcBlend;
    BlendMode mDstBlend;
    bool mAlphaTest;
    bool mAlphaTestLess;
    float mAlphaTestVal;
    bool mWireframe;
    
    std::vector<TextureInfo*> mTextures;
    std::vector<FloatArg*> mFloatArgs;
    // set of passes we are allowed to draw in.
    GHRenderPassMembership mPassMembership;
    // set of materials that we use instead of this one for various render passes.
    typedef std::map<GHIdentifier, GHMaterial*> PassOverrideList;
    PassOverrideList mPassOverrides;
	// when we ask for param "string1", ask the shader for param "string2"
	std::map<GHString, GHString> mParamAliases;

	typedef std::vector< std::pair<GHMaterial*, float> > LODList;
	LODList mLODs;
};
