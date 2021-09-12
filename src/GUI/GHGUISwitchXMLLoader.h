// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHPropertyContainer;
class GHStringIdFactory;
class GHControllerMgr;

/*
 <guiSwitch prop=GP_GUIDEARROWS>
    <switches>
        <switch>
            <properties>
                <boolProp val=false/>
            </properties>
            <widgets>
                <guiText text="^999Guide Arrows: Off" font=ericfont.xml textHeight=0.4 fill=FT_PCT hAlign=A_CENTER vAlign=A_CENTER/>
            </widgets>
            <transitions>
            </trasitions>
        </switch>
        <switch>
            <properties>
                <boolProp val=false/>
            </properties>
            <widgets>
                <guiText text="^999Guide Arrows: On" font=ericfont.xml textHeight=0.4 fill=FT_PCT hAlign=A_CENTER vAlign=A_CENTER/>
            </widgets>
            <transitions>
            </trasitions>
        </switch>
    </switches>
 </guiSwitch>
*/
class GHGUISwitchXMLLoader : public GHXMLObjLoader
{
public:
    GHGUISwitchXMLLoader(const GHPropertyContainer& props, const GHStringIdFactory& hashTable,
                         GHXMLObjFactory& objFactory, GHControllerMgr& controllerMgr);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;
    
private:
    const GHPropertyContainer& mProps;
    const GHStringIdFactory& mIdFactory;
    GHXMLObjFactory& mObjFactory;
    GHControllerMgr& mControllerMgr;
};
