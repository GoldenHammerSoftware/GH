// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHXMLObjFactory;
class GHStringIdFactory;
class GHModel;

/*
 example:

 <model>
    <transformNode id=ModelTop rotation="0 0 0" translate="0 0 0" scale=1>
        <transformNode id=floorNode translate="0 0 -1"/>
    </transformNode>
 
    <children>
        <child parentNode=ModelTop>
            <md3model file=gothax_lower.md3>
                <transformNode id=Legs rotation="-0.5 0 0.5" translate="0 0.28 0" scale=.85/>
            </md3model>
        </child>
        <child parentNode="tag_torso">
            <md3model file=gothax_upper.md3>
                <transformNode id=Torso rotation="0 0 0" translate="0 0 0" scale=1/>
            </md3model>
        </child>
        <child parentNode="tag_head">
            <cacheModel file=gothax_head_model.xml/>
        </child>
    </children>
 </model>
*/
class GHModelLoader : public GHXMLObjLoader
{
public:
    GHModelLoader(const GHXMLObjFactory& objFactory, const GHStringIdFactory& hashTable);
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;

protected:
    void loadTransformNode(GHModel& ret, const GHXMLNode& node, bool createIfEmpty) const;
    void loadChildren(GHModel& ret, const GHXMLNode& node) const;
    void loadMatReplace(GHModel& ret, const GHXMLNode& node) const;
    void loadAnimSet(GHModel& ret, const GHXMLNode& node) const;
    void loadPhysics(GHModel& ret, const GHXMLNode& node, GHPropertyContainer& extraData) const;
    
protected:
    const GHXMLObjFactory& mObjFactory;
    const GHStringIdFactory& mIdFactory;
};

