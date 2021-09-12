// Copyright 2010 Golden Hammer Software
#include "GHAppleIAPStoreFactory.h"
#include "GHAppleIAPStore.h"
#include "GHXMLSerializer.h"
#include "GHXMLNode.h"

GHAppleIAPStoreFactory::GHAppleIAPStoreFactory(GHEventMgr& gameThreadEventMgr, GHMessageQueue& gameThreadMessageQueue)
: mGameThreadEventMgr(gameThreadEventMgr)
, mGameThreadMessageQueue(gameThreadMessageQueue)
{
   
}

void GHAppleIAPStoreFactory::fillIAPStore(GHAppleIAPStore& ret,
                                          const char* configFilename,
                                          const GHXMLSerializer& xmlSerializer) const
{
    GHXMLNode* xmlNode = xmlSerializer.loadXMLFile(configFilename);
    assert(xmlNode);
    const GHXMLNode::NodeList& children = xmlNode->getChildren();
    size_t numChildren = children.size();
    for (size_t i = 0; i < numChildren; ++i)
    {
        const GHXMLNode* child = children[i];
        NSString* storeKitID = [NSString stringWithCString:child->getName().getChars() encoding:[NSString defaultCStringEncoding]];
        
        int ghID = 0;
        if (child->readAttrInt("id", ghID))
        {
            ret.addItemID(ghID, storeKitID);
        }
    }
    
    delete xmlNode;
}
