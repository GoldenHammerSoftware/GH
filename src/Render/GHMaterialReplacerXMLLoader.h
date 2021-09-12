// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHXMLObjFactory;
class GHStringIdFactory;

/* example
 <matreplace>
    <mat replace=default>
        <ghm vertex=worldvertex.glsl pixel=redpixel.glsl 
            billboard=none draworder=0 cullmode=none
            zread="true" zwrite=false offset=0
            alphablend=false invalpha=false 
            alphatest=false alphatestless=true alphatestval=0.9>
            <textures>
                <texture file=snowtree.png handle=Texture0 wrap=wrap mipmap=true/> 
            </textures>
        </ghm>
    </mat>
    <mat replace=head>
        <cacheObject file="tree_head.xml"/>
    </mat>
 </matreplace>
*/
class GHMaterialReplacerXMLLoader : public GHXMLObjLoader
{
public:
    GHMaterialReplacerXMLLoader(GHXMLObjFactory& objFactory,
                                GHStringIdFactory& hashTable);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;

private:
    GHXMLObjFactory& mObjFactory;
    GHStringIdFactory& mIdFactory;
};
