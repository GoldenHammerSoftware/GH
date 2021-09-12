// Copyright Golden Hammer Software
#include "GHXMLObjLoaderGHM.h"
#include "GHMDesc.h"
#include "GHXMLNode.h"
#include "GHUtils/GHResourceFactory.h"
#include "GHTexture.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHRenderProperties.h"
#include "GHMaterialPassOverrideXMLLoader.h"
#include "GHXMLObjFactory.h"
#include "GHMaterial.h"

static void loadVertexMode(const char* str, GHMDesc::VertexMode& mode)
{
    if (!str) return;
    if (!strcmp("alpha", str)) mode = GHMDesc::VM_VERTEXCOLOR;
    else if (!strcmp("color", str)) mode = GHMDesc::VM_VERTEXALPHA;
}

static void loadCullMode(const char* str, GHMDesc::CullMode& mode)
{
    if (!str) return;
    if (!strcmp("none", str)) mode = GHMDesc::CM_NOCULL;
    else if (!strcmp("ccw", str)) mode = GHMDesc::CM_CCW;
    else if (!strcmp("cw", str)) mode = GHMDesc::CM_CW;
}

static void loadWrapMode(const char* str, GHMDesc::WrapMode& mode)
{
    if (!str) return;
    if (!strcmp("wrap", str)) mode=GHMDesc::WM_WRAP;
    else if (!strcmp("clamp", str)) mode=GHMDesc::WM_CLAMP;
}

static void loadBlendMode(const char* str, GHMDesc::BlendMode& mode)
{
    if (!str) return;
    if (!strcmp("alpha", str)) mode=GHMDesc::BM_ALPHA;
    else if (!strcmp("invalpha", str)) mode=GHMDesc::BM_INVALPHA;
    else if (!strcmp("one", str)) mode=GHMDesc::BM_ONE;
}

void GHXMLObjLoaderGHM::loadBillboardType(const char* str, GHMDesc::BillboardType& type)
{
    if (!str) type = GHMDesc::BT_NONE;
    else if (!strcmp("full", str)) type = GHMDesc::BT_FULL;
	else if (!strcmp("BT_FULL", str)) type = GHMDesc::BT_FULL;
	else if (!strcmp("up", str)) type = GHMDesc::BT_UP;
	else if (!strcmp("BT_UP", str)) type = GHMDesc::BT_UP;
	else  type = GHMDesc::BT_NONE;
}

GHXMLObjLoaderGHM::GHXMLObjLoaderGHM(GHResourceFactory& resourceCache,
                                     const GHRenderPassMembershipXMLLoader& passMembershipLoader,
                                     const GHMaterialPassOverrideXMLLoader& poLoader,
									 GHXMLObjFactory& xmlFactory)
: mResourceCache(resourceCache)
, mPassMembershipLoader(passMembershipLoader)
, mPassOverrideLoader(poLoader)
, mXMLFactory(xmlFactory)
{
}

void GHXMLObjLoaderGHM::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHMDesc* ret = (GHMDesc*)obj;
    
    ret->mVertexFile.setConstChars(node.getAttribute("vertex"), GHString::CHT_COPY);
    ret->mPixelFile.setConstChars(node.getAttribute("pixel"), GHString::CHT_COPY);
    node.readAttrFloat("draworder", ret->mDrawOrder);
    node.readAttrBool("zread", ret->mZRead);
    node.readAttrBool("zwrite", ret->mZWrite);
    node.readAttrFloat("offset", ret->mZOffset);
    node.readAttrBool("alphablend", ret->mAlphaBlend);
    node.readAttrBool("alphatest", ret->mAlphaTest);
    node.readAttrBool("alphatestless", ret->mAlphaTestLess);
    node.readAttrFloat("alphatestval", ret->mAlphaTestVal);
    node.readAttrBool("wireframe", ret->mWireframe);
    
    loadBillboardType(node.getAttribute("billboard"), ret->mBillboard);
    loadBlendMode(node.getAttribute("srcblend"), ret->mSrcBlend);
    loadBlendMode(node.getAttribute("dstblend"), ret->mDstBlend);
    loadCullMode(node.getAttribute("cullmode"), ret->mCullMode);
    
	loadTextures(node, *ret, extraData);
	loadFloatArgs(node, *ret);
    
    const GHXMLNode* passMembershipNode = node.getChild("renderPassMembership", false);
    if (passMembershipNode) {
        mPassMembershipLoader.populate(&ret->mPassMembership, *passMembershipNode, extraData);
    }
    
	loadLODs(node, *ret, extraData);
	loadParamAliases(node, *ret);

	const GHProperty oldGHMProp = extraData.getProperty(GHRenderProperties::GP_GHMDESC);
	extraData.setProperty(GHRenderProperties::GP_GHMDESC, ret);
	const GHXMLNode* poNode = node.getChild("materialPassOverrides", false);
	if (poNode) {
		mPassOverrideLoader.populate(&ret->mPassOverrides, *poNode, extraData);
	}
	extraData.setProperty(GHRenderProperties::GP_GHMDESC, oldGHMProp);
}

void* GHXMLObjLoaderGHM::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHMDesc* ret = 0;
	const GHProperty ghmProp = extraData.getProperty(GHRenderProperties::GP_GHMDESC);
	const GHMDesc* parentDesc = ghmProp;
	if (!ghmProp.isValid() || parentDesc == 0)
	{
		ret = new GHMDesc;
	}
	else
	{
		ret = new GHMDesc(*parentDesc);
	}
    populate(ret, node, extraData);
    return ret;
}

void GHXMLObjLoaderGHM::loadParamAliases(const GHXMLNode& node, GHMDesc& ret) const
{
	const GHXMLNode* aliasesNode = node.getChild("paramAliases", false);
	if (aliasesNode)
	{
		for (size_t i = 0; i < aliasesNode->getChildren().size(); ++i)
		{
			const GHXMLNode* aliasNode = aliasesNode->getChildren()[i];
			const char* alias = aliasNode->getAttribute("alias");
			const char* target = aliasNode->getAttribute("target");
			if (!alias || !target) {
				GHDebugMessage::outputString("platform param alias invalid format");
				continue;
			}
			GHString aliasStr(alias, GHString::CHT_COPY);
			GHString targetStr(target, GHString::CHT_COPY);
			ret.mParamAliases[aliasStr] = targetStr;
		}
	}
}

void GHXMLObjLoaderGHM::loadLODs(const GHXMLNode& node, GHMDesc& ret, GHPropertyContainer& extraData) const
{
	const GHXMLNode* lodsNode = node.getChild("lods", false);
	if (lodsNode)
	{
		for (size_t i = 0; i < lodsNode->getChildren().size(); ++i)
		{
			const GHXMLNode* lodNode = lodsNode->getChildren()[i];
			if (!lodNode->getChildren().size()) {
				GHDebugMessage::outputString("No mat specified for ghm lod");
				continue;
			}
			float minDist = 0;
			if (!lodNode->readAttrFloat("minDist", minDist))
			{
				GHDebugMessage::outputString("No minDist specified for ghm lod");
				continue;
			}
			GHMaterial* mat = (GHMaterial*)mXMLFactory.load(*lodNode->getChildren()[0], &extraData);
			if (!mat) {
				GHDebugMessage::outputString("LOD mat failed to load");
				continue;
			}
			mat->acquire();
			ret.mLODs.push_back(std::pair<GHMaterial*, float>(mat, minDist));
		}
	}
}

void GHXMLObjLoaderGHM::loadFloatArgs(const GHXMLNode& node, GHMDesc& ret) const
{
	const GHXMLNode* floatArgsNode = node.getChild("floatargs", false);
	if (floatArgsNode)
	{
		const GHXMLNode::NodeList& floatChildren = floatArgsNode->getChildren();
		for (size_t i = 0; i < floatChildren.size(); ++i)
		{
			const char* handle = floatChildren[i]->getAttribute("handle");
			unsigned int count = 1;
			floatChildren[i]->readAttrUInt("count", count);
			float* fvals = new float[count];
			floatChildren[i]->readAttrFloatArr("value", fvals, count);

			ret.mFloatArgs.push_back(new GHMDesc::FloatArg(handle, count, fvals));
		}
	}
}

void GHXMLObjLoaderGHM::loadTextures(const GHXMLNode& node, GHMDesc& ret, GHPropertyContainer& extraData) const
{
	const GHXMLNode* texturesNode = node.getChild("textures", false);
	if (texturesNode)
	{
		const GHXMLNode::NodeList& texChildren = texturesNode->getChildren();
		for (size_t i = 0; i < texChildren.size(); ++i)
		{
			const char* texName = texChildren[i]->getAttribute("file");
			if (!texName) {
				GHDebugMessage::outputString("No file specified for texture");
				continue;
			}
			bool mipmap = true;
			texChildren[i]->readAttrBool("mipmap", mipmap);
			bool wasNotUsingMipmaps = extraData.getProperty(GHRenderProperties::DONTUSEMIPMAPS);
			extraData.setProperty(GHRenderProperties::DONTUSEMIPMAPS, !mipmap);
			GHTexture* tex = (GHTexture*)mResourceCache.getCacheResource(texName, &extraData);
			extraData.setProperty(GHRenderProperties::DONTUSEMIPMAPS, wasNotUsingMipmaps);
			if (!tex) {
				GHDebugMessage::outputString("Failed to find texture %s", texName);
				continue;
			}
			const char* handle = texChildren[i]->getAttribute("handle");
			GHMDesc::WrapMode wrap = GHMDesc::WM_WRAP;
			loadWrapMode(texChildren[i]->getAttribute("wrap"), wrap);
			// we assume any texture used in a mat doesn't need source data.
			// if this changes, will need to add a loading argument.
			tex->deleteSourceData();

			// if there's already a texture for handle then remove it first.
			for (auto existingTex = ret.mTextures.begin(); existingTex != ret.mTextures.end(); ++existingTex)
			{
				if (!::strcmp(handle, (*existingTex)->getHandle()))
				{
					delete *existingTex;
					ret.mTextures.erase(existingTex);
					break;
				}
			}
			ret.mTextures.push_back(new GHMDesc::TextureInfo(handle, tex, wrap));
		}
	}
}
