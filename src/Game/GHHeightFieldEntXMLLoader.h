// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include "GHHeightFieldNMeshLoader.h"
#include "GHHeightFieldOMeshLoader.h"
#include "GHString/GHIdentifierMap.h"

class GHHeightFieldXMLLoader;
class GHEntityXMLLoader;
class GHVBFactory;
class GHEntity;
class GHGeometryRenderable;
class GHTransformNode;
class GHStringIdFactory;
class GHThreadFactory;
class GHResourceFactory;

// xml loader for an ent that is built from a height field.
/*
 <heightFieldEnt>
    <heightField tex="blah.hff" nodeDist=0.1 maxHeight=5/>
	<heightFieldModifiers/>
    <heightFieldRenderable>
        <heightFieldMesh size="256 256" pos="0 0" divisions="1 1" vertsPerDivision="1000">
            <resource file=bms2-1mat_x0y0.xml/>
        </heightFieldMesh>
    </heightFieldRenderable>
    // other basic entity stuff.
 </heightFieldEnt>
*/
class GHHeightFieldEntXMLLoader : public GHXMLObjLoader
{
public:
    GHHeightFieldEntXMLLoader(GHXMLObjFactory& objFactory, const GHHeightFieldXMLLoader& heightFieldLoader, 
		const GHEntityXMLLoader& entLoader, GHVBFactory& vbFactory, GHStringIdFactory& hashTable, 
		const GHThreadFactory& threadFactory, const GHIdentifierMap<int>& enumMap,
		GHResourceFactory& resourceFactory);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    void loadModel(GHEntity& ent, const GHXMLNode& node, GHPropertyContainer& extraData) const;

private:
    GHXMLObjFactory& mObjFactory;
    const GHHeightFieldXMLLoader& mHeightFieldLoader;
    const GHEntityXMLLoader& mEntLoader;
    GHStringIdFactory& mIdFactory;
    GHHeightFieldNMeshLoader mNMeshLoader;
    GHHeightFieldOMeshLoader mOMeshLoader;
};
