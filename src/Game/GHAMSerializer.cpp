// Copyright Golden Hammer Software
#include "GHAMSerializer.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHModel.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHAMStructs.h"
#include "GHGeometryRenderable.h"
#include <vector>
#include "GHVertexBuffer.h"
#include "GHGeometry.h"
#include "GHVBBlitter.h"
#include <stdio.h>
#include "GHXMLNode.h"
#include "GHXMLSerializer.h"
#include "GHFrameAnimControllerVB.h"
#include "GHVBAnimData.h"
#include "GHMaterial.h"
#include "GHFrameAnimSetXMLSerializer.h"
#include "GHTransformAnimController.h"
#include "GHTransformAnimData.h"
#include "GHSkeletonAnimController.h"

GHAMSerializer::GHAMSerializer(const GHFileOpener& fileOpener, const GHXMLSerializer& xmlSerializer)
: mFileOpener(fileOpener)
, mXMLSerializer(xmlSerializer)
{
}

void GHAMSerializer::save(GHModel& model, const char* fileName) const
{
	GHTransform oldTrans;
	if (model.getSkeleton()) {
		oldTrans = model.getSkeleton()->getLocalTransform();
		model.getSkeleton()->getLocalTransform().becomeIdentity();
	}
    
    saveXML(model, fileName);

    char nameBuf[1024];
    assert(::strlen(fileName) < 1018);
    ::sprintf(nameBuf, "%s.gham", fileName);
    saveGHAM(model, nameBuf);
    
	if (model.getSkeleton()) {
		model.getSkeleton()->getLocalTransform() = oldTrans;
	}
}

void GHAMSerializer::saveXML(const GHModel& model, const char* fileNameRoot) const
{
    char nameBuf[1024];
    assert(::strlen(fileNameRoot) < 1019);

    GHXMLNode topNode(0);
    topNode.setName("gham", GHString::CHT_REFERENCE);
    ::sprintf(nameBuf, "%s.gham", fileNameRoot);
    topNode.setAttribute("file", GHString::CHT_REFERENCE, nameBuf, GHString::CHT_COPY);

    saveXMLMaterials(model, topNode);
    saveXMLAnimSet(model, topNode);
    
    ::sprintf(nameBuf, "%s.xml", fileNameRoot);
    mXMLSerializer.writeXMLFile(nameBuf, topNode);
}

void GHAMSerializer::saveXMLMaterials(const GHModel& model, GHXMLNode& topNode) const
{
    GHXMLNode* matReplaceTop = new GHXMLNode(0);
    matReplaceTop->setName("matreplace", GHString::CHT_REFERENCE);
    topNode.addChild(matReplaceTop);
  
    saveXMLMaterialNode(*matReplaceTop, "default", "defaultmat.xml");
    
    const GHGeometryRenderable* renderable = model.getRenderable();
    if (renderable)
    {
        const std::vector<GHGeometry*>& geos = renderable->getGeometry();
        for (size_t i = 0; i < geos.size(); ++i)
        {
            const char* matName = "defaultmat.xml";
            if (geos[i]->getMaterial()) {
                if (geos[i]->getMaterial()->getResourceId() != GHIdentifier(0)) {
                    matName = geos[i]->getMaterial()->getResourceId().getString();
                }
            }
            saveXMLMaterialNode(*matReplaceTop, geos[i]->getId().getString(), matName);
        }
    }
}

void GHAMSerializer::saveXMLMaterialNode(GHXMLNode& topNode, const char* meshName, const char* matName) const
{
    GHXMLNode* replaceNode = new GHXMLNode(0);
    replaceNode->setName("mat", GHString::CHT_REFERENCE);
    replaceNode->setAttribute("replace", GHString::CHT_REFERENCE, meshName, GHString::CHT_REFERENCE);
    topNode.addChild(replaceNode);
    
    GHXMLNode* matNode = new GHXMLNode(0);
    matNode->setName("resource", GHString::CHT_REFERENCE);
    matNode->setAttribute("file", GHString::CHT_REFERENCE, matName, GHString::CHT_REFERENCE);
    replaceNode->addChild(matNode);
}

void GHAMSerializer::saveXMLAnimSet(const GHModel& model, GHXMLNode& topNode) const
{
    const std::vector<GHAnimController*>& controllers = model.getAnimControllers();
    if (!controllers.size()) return;
    // find the anim set of the first frame anim controller in the list.
    const GHFrameAnimSet* animSet = 0;
    for (size_t i = 0; i < controllers.size(); ++i)
    {
        if (controllers[i]->getAnimType() == GHAnimController::AT_FRAME) {
            GHFrameAnimController* frameController = (GHFrameAnimController*)controllers[i];
            animSet = frameController->getAnimSet();
        }
    }
    if (!animSet) return;
    GHFrameAnimSetXMLSerializer::saveAnimSet(*animSet, topNode);
}

void GHAMSerializer::saveGHAM(GHModel& model, const char* fileName) const
{
    GHFile* file = mFileOpener.openFile(fileName, GHFile::FT_BINARY, GHFile::FM_WRITEONLY);
    if (!file)
    {
        GHDebugMessage::outputString("Failed to open GHAM for writing");
        return;
    }
    GHAM::Header ghamHeader;
    GHGeometryRenderable* renderable = model.getRenderable();
    std::vector<GHGeometry*> geometries;
    if (!renderable) {
        GHDebugMessage::outputString("Trying to save a GHAM with no renderable");
    }
    else {
        geometries = renderable->getGeometry();
    }
    ghamHeader.mNumMeshes = (int)geometries.size();
    ghamHeader.mNumSkeletonNodes = countSkeletonNodes(model.getSkeleton());

    file->writeBuffer(&ghamHeader, sizeof(GHAM::Header));

    saveSkeleton(*file, model.getTransformAnimControllers(), model.getSkelAnimControllers(), model.getSkeleton());
    saveSkeletonAnims(*file, model.getTransformAnimControllers(), model.getSkelAnimControllers(), model.getSkeleton());
    
    for (int i = 0; i < ghamHeader.mNumMeshes; ++i)
    {
        saveMesh(*file, model, *geometries[i]);
    }
    
    file->closeFile();
    delete file;
}

void GHAMSerializer::saveMesh(GHFile& file, GHModel& model, GHGeometry& geometry) const
{
    GHAM::MeshHeader meshHeader;
    
    GHVertexBufferPtr* vb = geometry.getVB();
    assert(vb);

    GHVBBlitterPtr* blitter = vb->get()->getBlitter();
    assert(blitter->get()->getType() == GHVBBlitter::BT_INDEX);
    GHVBBlitterIndex* indexBlitter = (GHVBBlitterIndex*)blitter->get();
    meshHeader.mNumIndices = indexBlitter->getNumIndices();
    
    const std::vector<GHVertexStreamPtr*>& streams = vb->get()->getStreams();
    assert(streams.size());
    meshHeader.mNumStreams = (int)streams.size();
    meshHeader.mNumVerts = (int)streams[0]->get()->getNumVerts();
    strncpy(meshHeader.mName, geometry.getId().getString(), GHAM::kNameLen);
	if (geometry.getTransform())
	{
		strncpy(meshHeader.mSkeletonName, geometry.getTransform()->getId().getString(), GHAM::kNameLen);
	}
    
    const GHModel::GeoAnimList& geoAnims = model.getGeoAnimControllers();
    GHVBAnimData* frameAnimData = 0;
    for (size_t i = 0; i < geoAnims.size(); ++i)
    {
        if (geoAnims[i]->getTarget() == &geometry) {
            frameAnimData = geoAnims[i]->getAnimData();
            // note: assuming max 1 anim controller per mesh
            break;
        }
    }
    if (frameAnimData) {
        meshHeader.mNumAnimFrames = (int)frameAnimData->getFrames().size();
    }
    else {
        meshHeader.mNumAnimFrames = 0;
    }
    meshHeader.mIsSkinned = 0; // todo.
    
    file.writeBuffer(&meshHeader, sizeof(GHAM::MeshHeader));

    for (int i = 0; i < meshHeader.mNumStreams; ++i)
    {
        saveStreamHeader(file, *streams[i]->get());
    }
    for (int i = 0; i < meshHeader.mNumStreams; ++i)
    {
        saveStreamBuffer(file, *streams[i]->get());
    }
    saveIBBuffer(file, *indexBlitter);
    
    if (frameAnimData && frameAnimData->getFrames().size())
    {
        // assume all frames have the same format.
        // assume that format is one stream
        saveStreamHeader(file, *frameAnimData->getFrames()[0]->get()->getStreams()[0]->get());
        
        for (size_t i = 0; i < frameAnimData->getFrames().size(); ++i)
        {
            GHVertexBuffer* frameVB = frameAnimData->getFrames()[i]->get();
            assert(frameVB->getStreams().size() == 1);
            GHVertexStream* frameStream = frameVB->getStreams()[0]->get();
            const GHVertexStreamFormat& frameVBFormat = frameStream->getFormat();
            assert(frameVBFormat.getComponents().size() == 2);
            saveStreamBuffer(file, *frameStream);
        }
    }
    
    GHRect<float, 3> boundRect(GHPoint3(0,0,0), GHPoint3(0,0,0));
    if (geometry.getBounds()) {
        geometry.getBounds()->getLocalSphere().toAABB(boundRect);
    }
    file.writeBuffer(boundRect.mMin.getCoords(), sizeof(float)*3);
    file.writeBuffer(boundRect.mMax.getCoords(), sizeof(float)*3);
}

void GHAMSerializer::saveStreamHeader(GHFile& file, GHVertexStream& stream) const
{
    const GHVertexStreamFormat& format = stream.getFormat();
    const std::vector<GHVertexStreamFormat::ComponentEntry>& components = format.getComponents();
    int numComps = (int)components.size();
    file.writeBuffer(&numComps, sizeof(int));
    
    GHAM::ComponentDef compDef;
    for (int i = 0; i < numComps; ++i)
    {
        compDef.mDataCount= components[i].mCount;
        ::sprintf(compDef.mDataType, "%s", GHVertexComponent::getStringForComponentType(components[i].mType));
        ::sprintf(compDef.mShaderID, "%s", GHVertexComponent::getStringForShaderID(components[i].mComponent));
        file.writeBuffer(&compDef, sizeof(GHAM::ComponentDef));
    }
}

void GHAMSerializer::saveStreamBuffer(GHFile& file, GHVertexStream& stream) const
{
    const GHVertexStreamFormat& format = stream.getFormat();
    const float* buffer = stream.lockReadBuffer();
    file.writeBuffer(buffer, sizeof(float)*format.getVertexSize()*stream.getNumVerts());
    stream.unlockReadBuffer();
}

void GHAMSerializer::saveIBBuffer(GHFile& file, GHVBBlitterIndex& indexBlitter) const
{
    const unsigned short* ib = indexBlitter.lockReadBuffer();
    file.writeBuffer(ib, sizeof(unsigned short)*indexBlitter.getNumIndices());
    indexBlitter.unlockReadBuffer();

	// add 2 bytes of padding if there are an uneven number of indices.
	// added in version 1.
	if (indexBlitter.getNumIndices() % 2 != 0)
	{
		const unsigned short padding = 0;
		file.writeBuffer(&padding, sizeof(unsigned short));
	}
}

int GHAMSerializer::countSkeletonNodes(const GHTransformNode* top) const
{
    if (!top) return 0;
    int numSkeletons = 1; // the top node;
    const std::vector<GHTransformNode*>& children = top->getChildren();
    for (size_t i = 0; i < children.size(); ++i)
    {
        numSkeletons += countSkeletonNodes(children[i]);
    }
    return numSkeletons;
}

void GHAMSerializer::saveSkeleton(GHFile& file, const GHModel::TransformAnimList& transformAnims, const GHModel::SkelAnimList& skelAnims, const GHTransformNode* top) const
{
    if (!top) return;
    GHAM::SkeletonNode nodeStruct;
    memcpy(nodeStruct.mTransform, top->getLocalTransform().getMatrix().getCoords(), 16*sizeof(float));
    strncpy(nodeStruct.mName, top->getId().getString(), GHAM::kNameLen);
    
    if (top->getParent()) {
        strncpy(nodeStruct.mParentName, top->getParent()->getId().getString(), GHAM::kNameLen);
    }
    else {
        nodeStruct.mParentName[0] = 0;
    }
    
    GHTransformAnimData* nodeAnim = findNodeAnimController(*top, transformAnims, skelAnims);
    if (!nodeAnim) nodeStruct.mNumAnimFrames = 0;
    else nodeStruct.mNumAnimFrames = (int)nodeAnim->getFrames().size();
    
    file.writeBuffer(&nodeStruct, sizeof(GHAM::SkeletonNode));

    const std::vector<GHTransformNode*>& children = top->getChildren();
    for (size_t i = 0; i < children.size(); ++i)
    {
        saveSkeleton(file, transformAnims, skelAnims, children[i]);
    }
}

void GHAMSerializer::saveSkeletonAnims(GHFile& file, const GHModel::TransformAnimList& transformAnims, const GHModel::SkelAnimList& skelAnims, const GHTransformNode* top) const
{
    if (!top) return;
    GHTransformAnimData* nodeAnim = findNodeAnimController(*top, transformAnims, skelAnims);
    if (nodeAnim) {
        for (size_t i = 0; i < nodeAnim->getFrames().size(); ++i)
        {
            file.writeBuffer(nodeAnim->getFrames()[i].getCoords(), sizeof(float)*16);
        }
    }
    const std::vector<GHTransformNode*>& children = top->getChildren();
    for (size_t i = 0; i < children.size(); ++i)
    {
        saveSkeletonAnims(file, transformAnims, skelAnims, children[i]);
    }
}

GHTransformAnimData* GHAMSerializer::findNodeAnimController(const GHTransformNode& node, const GHModel::TransformAnimList& transformAnims, const GHModel::SkelAnimList& skelAnims) const
{
    GHTransformAnimData* nodeAnim = 0;
    for (size_t i = 0; i < transformAnims.size(); ++i)
    {
        if (transformAnims[i]->getTarget() == &node) {
            nodeAnim = transformAnims[i]->getAnimData();
            break;
        }
    }
    if (nodeAnim) {
        return nodeAnim;
    }
    
    // we could be using a skeleton anim instead of a bunch of transform anims.
    for (size_t i = 0; i < skelAnims.size(); ++i)
    {
        nodeAnim = skelAnims[i]->getAnimForNode(node.getId());
        if (nodeAnim) break;
    }
    
    return nodeAnim;
}
