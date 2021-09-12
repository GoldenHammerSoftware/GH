// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include "GHRenderPassMembershipXMLLoader.h"
#include "GHMDesc.h"

class GHResourceFactory;
class GHMaterialPassOverrideXMLLoader;
class GHXMLObjFactory;

/*
 <ghm vertex=fontvertex.glsl pixel=fontpixel.glsl 
    billboard=none draworder=0 cullmode=none
    zread="true" zwrite=false
    alphablend=true srcblend=alpha destblend=one wireframe=false>
    <textures>
        <texture file=ericfont.png handle=Texture0 wrap=wrap mipmap=true/> 
    </textures>
    <renderPassMembership/>
    <materialPassOverrides>
        <po pass="shadow"><ghm blahblah/></po>
    </materialPassOverrides>
	<lods>
		<matLOD minDist=400><ghm blahblah/></matLOD>
		<matLOD minDist=800><ghm blahblah/></matLOD>
	</lods>
	<paramAliases>
		<alias target="shaderblah" alias="blah"/>
	</paramAliases>
 </ghm>
*/
class GHXMLObjLoaderGHM : public GHXMLObjLoader
{
public:
    GHXMLObjLoaderGHM(GHResourceFactory& resourceCache,
                      const GHRenderPassMembershipXMLLoader& passMembershipLoader,
                      const GHMaterialPassOverrideXMLLoader& poLoader,
					  GHXMLObjFactory& xmlFactory);

    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;

	static void loadBillboardType(const char* str, GHMDesc::BillboardType& type);

private:
	void loadParamAliases(const GHXMLNode& node, GHMDesc& ret) const;
	void loadLODs(const GHXMLNode& node, GHMDesc& ret, GHPropertyContainer& extraData) const;
	void loadFloatArgs(const GHXMLNode& node, GHMDesc& ret) const;
	void loadTextures(const GHXMLNode& node, GHMDesc& ret, GHPropertyContainer& extraData) const;

protected:
    GHResourceFactory& mResourceCache;
    const GHRenderPassMembershipXMLLoader& mPassMembershipLoader;
    const GHMaterialPassOverrideXMLLoader& mPassOverrideLoader;
	GHXMLObjFactory& mXMLFactory;
};
