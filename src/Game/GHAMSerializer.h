// Copyright Golden Hammer Software
#pragma once

#include "GHAMStructs.h"
#include "GHModel.h"

class GHFileOpener;
class GHFile;
class GHGeometry;
class GHVertexStream;
class GHVBBlitterIndex;
class GHTransformNode;
class GHXMLSerializer;
class GHXMLNode;
class GHTransformAnimData;

// class to save a model in GHAM format
class GHAMSerializer
{
public:
    GHAMSerializer(const GHFileOpener& fileOpener, const GHXMLSerializer& xmlSerializer);
    
    // save out an xml wrapper which is named [fileName]+".xml"
    // save out a binary file which is named [fileName]+".gham"
    void save(GHModel& model, const char* fileName) const;

    // save the xml wrapper for the gham named [fileNameRoot]+".gham"
    void saveXML(const GHModel& model, const char* fileNameRoot) const;
    // save the binary part
    void saveGHAM(GHModel& model, const char* fileName) const;

private:
    // create the material replace node in the xml file.
    void saveXMLMaterials(const GHModel& model, GHXMLNode& topNode) const;
    // create the material replace node for one mesh
    void saveXMLMaterialNode(GHXMLNode& topNode, const char* meshName, const char* matName) const;
    void saveXMLAnimSet(const GHModel& model, GHXMLNode& topNode) const;

    void saveMesh(GHFile& file, GHModel& model, GHGeometry& geometry) const;
    void saveStreamHeader(GHFile& file, GHVertexStream& stream) const;
    void saveStreamBuffer(GHFile& file, GHVertexStream& stream) const;
    void saveIBBuffer(GHFile& file, GHVBBlitterIndex& indexBlitter) const;

    // recurse and count the number of nodes in the tree
    int countSkeletonNodes(const GHTransformNode* top) const;
    // recurse and write out GHAMSkeletonNode structs
    void saveSkeleton(GHFile& file, const GHModel::TransformAnimList& transformAnims, const GHModel::SkelAnimList& skelAnims, const GHTransformNode* top) const;
    void saveSkeletonAnims(GHFile& file, const GHModel::TransformAnimList& transformAnims, const GHModel::SkelAnimList& skelAnims, const GHTransformNode* top) const;
    GHTransformAnimData* findNodeAnimController(const GHTransformNode& node, const GHModel::TransformAnimList& transformAnims, const GHModel::SkelAnimList& skelAnims) const;

private:
    const GHFileOpener& mFileOpener;
    const GHXMLSerializer& mXMLSerializer;
};

