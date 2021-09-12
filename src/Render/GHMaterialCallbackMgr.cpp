// Copyright Golden Hammer Software
#include "GHMaterialCallbackMgr.h"
#include "GHPlatform/GHStlUtil.h"
#include <stddef.h>

GHMaterialCallbackMgr::~GHMaterialCallbackMgr(void)
{
    GHStlUtil::deletePointerList(mFactories);
}

void GHMaterialCallbackMgr::createCallbacks(GHMaterial& mat) const
{
    for (size_t i = 0; i < mFactories.size(); ++i)
    {
        mFactories[i]->createCallbacks(mat);
    }
}

void GHMaterialCallbackMgr::addFactory(GHMaterialCallbackFactory* factory)
{
    mFactories.push_back(factory);
}

void GHMaterialCallbackMgr::removeFactory(GHMaterialCallbackFactory* factory)
{
    std::vector<GHMaterialCallbackFactory*>::iterator findIter;
    findIter = std::find(mFactories.begin(), mFactories.end(), factory);
    if (findIter != mFactories.end()) {
        mFactories.erase(findIter);
    }
}

