// Copyright 2010 Golden Hammer Software
#include "GHAndroidURLOpenerTransitionLoader.h"
#include "GHAndroidURLOpenerTransition.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHXMLNode.h"

GHAndroidURLOpenerTransitionLoader::GHAndroidURLOpenerTransitionLoader(GHJNIMgr& jniMgr, jobject jObject)
: mJNIMgr(jniMgr)
, mJavaObj(jObject)
{
}

void* GHAndroidURLOpenerTransitionLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    const char* url = node.getAttribute("url");
    if (!url) {
        GHDebugMessage::outputString("URLTrans with no url specified");
        return 0;
    }
    return new GHAndroidURLOpenerTransition(mJNIMgr, mJavaObj, url);
}
