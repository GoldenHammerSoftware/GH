// Copyright Golden Hammer Software
#pragma once

/*
 FORMAT
 
 GHAMHeader
 (numSkeletonNodes instances of)
    GHAMSkeletonNode
 (numSkeletonNodes instances of)
    (skeletonNode.numAnimFrames instances of)
        float[16]
 (numMeshes instances of)
    GHAMMeshHeader
    if meshHeader.mIsSkinned:
        numBones
        (numBones instances of)
            char [256] boneName
    (numStreams instances of)
        numComponents
        (numComponents instances of)
            GHAMComponentDef
    (numStreams instances of)
        stream vert data block
    mesh index data block
    if numAnimFrames
        // we assume all anim frames are same format.
        numComponents
        (numComponents instances of)
            GHAMComponentDef
        (numAnimFrames instances of)
            stream vert data block
    GHRect<float, 3> AABB bounds
*/

// Data structures for loading/saving GHAM files
/***************************************************
 * !!!
 * !!! CHANGING THE ORDER OR SIZE OF ANY OF THESE 
 * !!! STRUCTS REQUIRES CHANGING ANY EXPORTER,
 * !!! AND ANY EXPORTED MODEL
 * !!!
 ***************************************************/
namespace GHAM
{
    static const int kNameLen = 256;

    // The first block of data in the file
    struct Header
    {
		int mMagic{ 1296123975 }; // int value of "GHAM"
		int mVersion{ 1 };
		int mNumMeshes{ 0 };
        int mNumSkeletonNodes{ 0 };
        int mUnused[12] = { 0 };

		// version 1: added insertion of an extra short if an odd number of indices.
    };
    
    // A block that represents a vertex component found in a GHAM
    struct ComponentDef
    {
        // number of mDataTypes in a row that represent this thing.
        int mDataCount{ 0 };
        // should match a value GHVertexComponent::ComponentType
		char mDataType[kNameLen] = { 0 };
        // should match one of the values found in GHVertexComponent::ShaderId
        char mShaderID[kNameLen] = { 0 };
    };
    
    // header to say what is found in a mesh
	// this must be 4 byte aligned
    struct MeshHeader
    {
        int mNumStreams{ 0 };
        int mNumVerts{ 0 };
        int mNumIndices{ 0 };
        // we might store pos&normal for each anim frame if mesh anims exist.
        int mNumAnimFrames{ 0 };
        char mName[kNameLen] = { 0 };
        char mSkeletonName[kNameLen] = { 0 };
        int mIsSkinned{ 0 };
        int mUnused[7] = { 0 };
    };
    
    // one skeleton node.
    // after loading them we go through and fix up the parenting.
    struct SkeletonNode
    {
        char mName[kNameLen] = { 0 };
        char mParentName[kNameLen] = { 0 };
        float mTransform[16];
        int mNumAnimFrames{ 0 };
    };
};
