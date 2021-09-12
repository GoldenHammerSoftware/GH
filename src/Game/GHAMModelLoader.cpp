// Copyright Golden Hammer Software
#include "GHAMModelLoader.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHModel.h"
#include "GHGeometryRenderable.h"
#include "GHGeometry.h"
#include "GHVBFactory.h"
#include "GHEntityHashes.h"
#include "GHAMAnimFactory.h"
#include "GHFrameAnimControllerVB.h"
#include "GHVBAnimData.h"
#include "GHTransformAnimController.h"
#include "GHTransformAnimData.h"
#include "GHFrameAnimSet.h"
#include "GHSkinAnimController.h"
#include "GHModelTransformAnimCloner.h"
#include "GHModelFrameAnimVBAnimCloner.h"
#include "GHModelSkinAnimCloner.h"
#include "GHSkeletonAnimController.h"
#include "GHModelSkeletonAnimCloner.h"
#include "GHVertexBufferUtil.h"

GHAMModelLoader::GHAMModelLoader(const GHXMLObjFactory& objFactory, const GHStringIdFactory& hashTable,
                                 const GHFileOpener& fileOpener, GHVBFactory& vbFactory,
                                 const GHIdentifierMap<int>& enumStore, GHAMAnimFactory* animFactory)
: GHModelLoader(objFactory, hashTable)
, mFileOpener(fileOpener)
, mVBFactory(vbFactory)
, mEnumStore(enumStore)
, mAnimFactory(animFactory)
{
    assert(mAnimFactory);
}

GHAMModelLoader::~GHAMModelLoader(void)
{
    delete mAnimFactory;
}

void* GHAMModelLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHModel* ret = new GHModel;
    loadTransformNode(*ret, node, false);
    
    void* prevModel = (void*)extraData.getProperty(GHEntityHashes::MODEL);
    extraData.setProperty(GHEntityHashes::MODEL, ret);

    const char* ghamFileName = node.getAttribute("file");
    if (!ghamFileName) {
        GHDebugMessage::outputString("GHAM node with no file listed");
        return ret;
    }
    
    bool combinedBounds = false;
    node.readAttrBool("combinedBounds", combinedBounds);

	GHTransformNode* modelTopNode = 0;
	if (ret->getSkeleton())
	{
		const char* modelTopNodeName = node.getAttribute("ghamTopNode");
		GHIdentifier modelTopNodeId = mIdFactory.generateHash(modelTopNodeName);
		modelTopNode = ret->getSkeleton()->findChild(modelTopNodeId);
		if (!modelTopNode) modelTopNode = ret->getSkeleton();
	}

    GHFile* file = mFileOpener.openFile(ghamFileName, GHFile::FT_BINARY, GHFile::FM_READONLY);
    if (file)
    {
        size_t bufferSize = 0;
        char* bufferStart = 0;
        // todo? read in the file a bit at a time instead of duplicating it.
        if (file->readIntoBuffer() && file->getFileBuffer(bufferStart, bufferSize))
        {
            const char* buffer = bufferStart;
            loadGHAM(*ret, combinedBounds, buffer, modelTopNode);
        }
        delete file;
    }
	else
	{
		GHDebugMessage::outputString("Could not load gham file %s", ghamFileName);
	}

	bool disableBounds = false;
	node.readAttrBool("disableBounds", disableBounds);
	if (disableBounds)
	{
		GHGeometryRenderable* geoRend = ret->getRenderable();
		if (geoRend)
		{
			for (size_t i = 0; i < geoRend->getGeometry().size(); ++i)
			{
				geoRend->getGeometry()[i]->setBounds(0);
			}
		}
	}

    loadChildren(*ret, node);
    loadMatReplace(*ret, node);
    loadAnimSet(*ret, node);
    loadPhysics(*ret, node, extraData);
    
    extraData.setProperty(GHEntityHashes::MODEL, prevModel);
    return ret;
}

void GHAMModelLoader::loadGHAM(GHModel& model, bool combinedBounds, const char* buffer, GHTransformNode* modelTopNode) const
{
    const GHAM::Header* ghamHeader = (const GHAM::Header*)buffer;
    buffer += sizeof(GHAM::Header);
    
    GHGeometryRenderable* renderable = new GHGeometryRenderable();
    model.setRenderable(renderable);
    
    buffer = loadSkeleton(model, ghamHeader->mNumSkeletonNodes, buffer, modelTopNode);
	if (!modelTopNode)
	{
		modelTopNode = model.getSkeleton();
	}

    for (int meshIdx = 0; meshIdx < ghamHeader->mNumMeshes; ++meshIdx)
    {
        buffer = loadMesh(model, combinedBounds, buffer, modelTopNode, *ghamHeader);
    }
}

const char* GHAMModelLoader::loadMesh(GHModel& model, bool combinedBounds, const char* buffer, GHTransformNode* modelTopNode, const GHAM::Header& ghamHeader) const
{
    GHGeometryRenderable* renderable = model.getRenderable();
    assert(renderable);
    
    const GHAM::MeshHeader* meshHeader = (const GHAM::MeshHeader*)buffer;
    buffer += sizeof(GHAM::MeshHeader);

	// test hack.
	//GHDebugMessage::outputString("<mat replace=%s>", meshHeader->mName);
	//GHDebugMessage::outputString("numverts %d", meshHeader->mNumVerts);

    GHGeometry* geo = new GHGeometry;
    geo->setId(mIdFactory.generateHash(meshHeader->mName));
    GHTransformNode* modelParent = modelTopNode->findChild(mIdFactory.generateHash(meshHeader->mSkeletonName));
    if (!modelParent) modelParent = modelTopNode;
    geo->setTransform(modelParent);
    if (meshHeader->mIsSkinned) {
        geo->setCloneType(GHGeometry::CT_CLONEVB);
    } else {
        geo->setCloneType(GHGeometry::CT_SAMEVB);
    }

    std::vector<GHIdentifier> skinIds;
    if (meshHeader->mIsSkinned)
    {
        buffer = loadMeshSkinDesc(skinIds, buffer);
    }

    // todo: add static/dynamic
    GHVBDescription vbDesc(meshHeader->mNumVerts, meshHeader->mNumIndices, GHVBBlitter::BT_INDEX);
    buffer = loadStreamDescriptions(*meshHeader, vbDesc, meshHeader->mNumStreams, buffer);
	bool needGeneratedComps = addGeneratedStreamDescription(vbDesc);

    GHVertexBuffer* vb = mVBFactory.createVB(vbDesc);
    assert(vb);
    geo->setVB(new GHVertexBufferPtr(vb));
    
    buffer = loadStreamData(*vb, meshHeader->mNumStreams, buffer);
    buffer = loadIBData(*vb, buffer);

	// version 1 added an extra padding short if there are an odd number of indices.
	if (ghamHeader.mVersion > 0)
	{
		if (meshHeader->mNumIndices % 2 != 0)
		{
			buffer += 2;
		}
	}

    buffer = loadMeshAnim(*meshHeader, *geo, model, buffer);
    
	if (needGeneratedComps)
	{
		// Do we also need to generate components for the mesh anim frames?
		populateGeneratedVertexComponents(*vb);
	}

    GHSphereBounds* bounds = 0;
    buffer = loadMeshBounds(bounds, buffer);
    if (bounds)
    {
        if (!combinedBounds) {
            geo->setBounds(bounds);
        }
        else {
            // todo: instead of adding a bunch of bounds to the model,
            //  combine them into a single bounds.
            bounds->setTransform(modelParent);
            model.addBounds(*bounds);
            delete bounds;
        }
    }
    
    renderable->addGeometry(geo);
    
    if (meshHeader->mIsSkinned)
    {
        GHSkinAnimController* skinController = mAnimFactory->createSkinAnim(skinIds);
        skinController->setTarget(vb, model.getSkeleton());
        model.addAnimController(skinController, new GHModelSkinAnimCloner(*skinController));
    }
    
    return buffer;
}

bool GHAMModelLoader::addGeneratedStreamDescription(GHVBDescription& vbDesc) const
{
	// Look for SI_TANGENT.  If not found then add a new stream.
	const std::vector<GHVBDescription::StreamDescription>& existingStreams = vbDesc.getStreamDescriptions();
	for (auto streamIter = existingStreams.begin(); streamIter != existingStreams.end(); ++streamIter)
	{
		const GHVBDescription::StreamDescription& streamDesc = *streamIter;
		for (auto compIter = streamDesc.mComps.begin(); compIter != streamDesc.mComps.end(); ++compIter)
		{
			const GHVertexComponent& comp = *compIter;
			if (comp.mID == GHVertexComponentShaderID::SI_TANGENT)
			{
				// Found our missing component, don't add a new stream.
				// This function will need to be more complex if we want to generate more than tangents.
				return false;
			}
		}
	}

	// Need to add a new stream to hold tangents.
	GHVBDescription::StreamDescription generatedDesc;
	generatedDesc.mComps.push_back(GHVertexComponent(GHVertexComponentShaderID::SI_TANGENT, GHVertexComponentType::CT_FLOAT, 3) );
	vbDesc.addStreamDescription(generatedDesc);

	return true;
}

void GHAMModelLoader::populateGeneratedVertexComponents(GHVertexBuffer& vb) const
{
	// It would be better to generate these offline but we need to generate them if not found.
	GHVertexBufferUtil::populateTangents(vb);
}

const char* GHAMModelLoader::loadMeshSkinDesc(std::vector<GHIdentifier>& skinIds, const char* buffer) const
{
    int numBones = *((int*)buffer);
    buffer += sizeof(int);
    for (int i = 0; i < numBones; ++i)
    {
        GHIdentifier nodeId = mIdFactory.generateHash(buffer);
        skinIds.push_back(nodeId);
        buffer += sizeof(char)*256;
    }
    return buffer;
}

const char* GHAMModelLoader::loadMeshAnim(const GHAM::MeshHeader& meshHeader, GHGeometry& geo,
                                          GHModel& model, const char* buffer) const
{
    if (!meshHeader.mNumAnimFrames)
    {
        return buffer;
    }
    // load in the anim frame format, assume all are the same and one stream.
    GHVBDescription animDesc(meshHeader.mNumVerts, 0, GHVBBlitter::BT_UNKNOWN);
    buffer = loadStreamDescriptions(meshHeader, animDesc, 1, buffer);
    
    GHFrameAnimControllerVB* animController = mAnimFactory->createMeshAnim();
    GHVBAnimData* animData = animController->getAnimData();
    for (int i = 0; i < meshHeader.mNumAnimFrames; ++i)
    {
        GHVertexBuffer* frameVB = mVBFactory.createVB(animDesc);
        buffer = loadStreamData(*frameVB, 1, buffer);
        animData->addFrame(new GHVertexBufferPtr(frameVB));
    }
    animController->setTarget(&geo);
    model.addGeometryAnimController(animController, new GHModelFrameAnimVBAnimCloner(*animController));
    return buffer;
}

const char* GHAMModelLoader::loadStreamDescriptions(const GHAM::MeshHeader& meshHeader, GHVBDescription& vbDesc,
                                                    int numStreams, const char* buffer) const
{
    for (int streamIdx = 0; streamIdx < numStreams; ++streamIdx)
    {
        GHVBDescription::StreamDescription streamDesc;
        int numComponents = *((int*)buffer);
        buffer += sizeof(int);
        for (int compIdx = 0; compIdx < numComponents; ++compIdx)
        {
            const GHAM::ComponentDef* compDef = (const GHAM::ComponentDef*)buffer;
            buffer += sizeof(GHAM::ComponentDef);
            
            const int* shaderId = mEnumStore.find(compDef->mShaderID);
            assert(shaderId);
            const int* compType = mEnumStore.find(compDef->mDataType);
            assert(compType);
            streamDesc.mComps.push_back(GHVertexComponent((GHVertexComponentShaderID::Enum)*shaderId, (GHVertexComponentType::Enum)*compType, compDef->mDataCount));
        }
        mAnimFactory->modifyStreamDesc(meshHeader, streamDesc);
        vbDesc.addStreamDescription(streamDesc);
    }
    return buffer;
}

const char* GHAMModelLoader::loadStreamData(GHVertexBuffer& vb, int numStreams, const char* buffer) const
{
    for (int streamIdx = 0; streamIdx < numStreams; ++streamIdx)
    {
        GHVertexStreamPtr* streamPtr = vb.getStream(streamIdx);
        float* vertData = streamPtr->get()->lockWriteBuffer();
        int dataSize = streamPtr->get()->getNumVerts() * streamPtr->get()->getFormat().getVertexSize() * sizeof(float);
        memcpy(vertData, buffer, dataSize);
        buffer += dataSize;
        streamPtr->get()->unlockWriteBuffer();
    }
    return buffer;
}

const char* GHAMModelLoader::loadIBData(GHVertexBuffer& vb, const char* buffer) const
{
    GHVBBlitterIndex* ibBlitter = (GHVBBlitterIndex*)(vb.getBlitter()->get());
    unsigned short* ibBuf = ibBlitter->lockWriteBuffer();
    size_t ibSize = ibBlitter->getNumIndices()*sizeof(unsigned short);
    ::memcpy(ibBuf, buffer, ibSize);
    ibBlitter->unlockWriteBuffer();
    buffer += ibSize;
    return buffer;
}

const char* GHAMModelLoader::loadSkeleton(GHModel& model, int numNodes, const char* buffer, GHTransformNode* modelTopNode) const
{
    const GHAM::SkeletonNode* nodeArr = (const GHAM::SkeletonNode*)buffer;
    
    // first create the nodes
    std::vector<GHTransformNode*> newNodes;
    for (int i = 0; i < numNodes; ++i)
    {
        newNodes.push_back(new GHTransformNode);
        GHTransformNode* currNode = newNodes.back();
        currNode->setId(mIdFactory.generateHash(nodeArr[i].mName));
        currNode->getLocalTransform().getMatrix().setCoords(nodeArr[i].mTransform);
        currNode->getLocalTransform().incrementVersion();
    }
    
    // find which node should be top if one doesn't exist
	if (!modelTopNode)
	{
		if (!model.getSkeleton())
		{
			GHTransformNode* topNode = 0;
			for (int i = 0; i < numNodes; ++i)
			{
				if (nodeArr[i].mParentName[0] == 0 && !nodeArr[i].mNumAnimFrames)
				{
					if (topNode != 0) {
						// two nodes want to be top,
						//  so flag to make a new node instead.
						topNode = 0;
						break;
					}
					else {
						topNode = newNodes[i];
					}
				}
			}

			if (!topNode) {
				GHTransformNode* modelTop = new GHTransformNode();
				modelTop->setId(mIdFactory.generateHash("ModelTop"));
				model.setSkeleton(modelTop);
			}
			else {
				model.setSkeleton(topNode);
			}
			modelTopNode = model.getSkeleton();
		}
		else
		{
			modelTopNode = model.getSkeleton();
		}
	}
    
    // then fix up the parenting
    for (int i = 0; i < numNodes; ++i)
    {
        if (nodeArr[i].mParentName[0] == 0 && newNodes[i] != modelTopNode)
        {
			modelTopNode->addChild(newNodes[i]);
        }
        else
        {
            GHIdentifier nodeId = mIdFactory.generateHash(nodeArr[i].mParentName);
            bool foundParent = false;
            for (int j = 0; j < numNodes; ++j)
            {
                if (j == i) continue;
                if (newNodes[j]->getId() == nodeId)
                {
                    newNodes[j]->addChild(newNodes[i]);
                    foundParent = true;
                    break;
                }
            }
            if (!foundParent && newNodes[i] != modelTopNode)
            {
                GHDebugMessage::outputString("GHAM loading: failed to find parent node %s", nodeArr[i].mParentName);
				modelTopNode->addChild(newNodes[i]);
            }
        }
    }
    
    buffer += sizeof(GHAM::SkeletonNode)*numNodes;
    
    bool hasAnims = false;
    for (int i = 0; i < numNodes; ++i) {
        if (nodeArr[i].mNumAnimFrames) {
            hasAnims = true;
            break;
        }
    }
    GHSkeletonAnimController* skelAnim = 0;
    if (hasAnims) {
        skelAnim = mAnimFactory->createSkeletonAnim(*modelTopNode);
        model.addSkeletonAnimController(skelAnim, new GHModelSkeletonAnimCloner(*skelAnim));
    }
    
    // and load in animations
    for (int i = 0; i < numNodes; ++i)
    {
        if (!nodeArr[i].mNumAnimFrames) continue;
        GHTransformAnimData* animData = skelAnim->getAnimForNode(newNodes[i]->getId());
        std::vector<GHPoint16>& animFrames = animData->getFrames();
        animFrames.resize(nodeArr[i].mNumAnimFrames);
        const int animSize = sizeof(float)*16;
        for (int animFrameIdx = 0; animFrameIdx < nodeArr[i].mNumAnimFrames; ++animFrameIdx)
        {
            memcpy(animFrames[animFrameIdx].getArr(), buffer, animSize);
            buffer += animSize;
        }

// a define for autoplaying any animations on load.
//#define DEBUG_AUTOPLAY_ANIMATION
#ifdef DEBUG_AUTOPLAY_ANIMATION
        GHTransformAnimController* animController = mAnimFactory->createNodeAnim();
        animController->setTarget(newNodes[i]);
        model.addTransformAnimController(animController, new GHModelTransformAnimCloner(*animController));
        
		assert(animController->getAnimData());
		animController->getAnimData()->getFrames() = animFrames;

		// add a dumb anim sequence that just loops through all frames.
        GHFrameAnimSet* animSet = new GHFrameAnimSet;
        GHFrameAnimSequence seq;
        seq.mFirstFrame = 0;
        seq.mEndFrame = nodeArr[i].mNumAnimFrames;
        seq.mLoopFrames = seq.mEndFrame - seq.mFirstFrame;
        seq.mFramesPerSecond = 1;
        seq.mId = mIdFactory.generateHash("default");
        animSet->addSequence(seq);
        animController->setAnimSet(animSet);
        animController->setAnim(seq.mId, 0);
#endif
	}
    
    if (hasAnims) {
        skelAnim->convertToModelSpace();
    }
    
    if (numNodes == 0 && !model.getSkeleton()) {
        // make a default skeleton if none is found.
        GHTransformNode* transform = new GHTransformNode;
        transform->setId(mIdFactory.generateHash("ModelTop"));
        model.setSkeleton(transform);
    }

    return buffer;
}

const char* GHAMModelLoader::loadMeshBounds(GHSphereBounds*& ret, const char* buffer) const
{
    GHRect<float, 3> boundRect((float*)buffer);

	// don't add an empty bounds.
	if (!(boundRect.mMax == boundRect.mMin)) 
	{
		ret = new GHSphereBounds;
		GHSphere sphere;
		sphere.fromAABB(boundRect);
		ret->setSphere(sphere);
	}

    buffer += sizeof(float)*6;
    return buffer;
}
