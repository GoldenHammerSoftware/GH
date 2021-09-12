// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHPropertyStringGeneratorXMLLoader;
class GHGUITextXMLLoader;
class GHControllerMgr;

// shortcut loader for loading a GHGUIText that builds a string from properties
class GHGUIPropTextXMLLoader : public GHXMLObjLoader
{
public:
    GHGUIPropTextXMLLoader(const GHGUITextXMLLoader& textLoader,
                           const GHPropertyStringGeneratorXMLLoader& genLoader,
                           GHControllerMgr& controllerMgr);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    const GHPropertyStringGeneratorXMLLoader& mGeneratorLoader;
    const GHGUITextXMLLoader& mTextLoader;
    GHControllerMgr& mControllerMgr;
};
