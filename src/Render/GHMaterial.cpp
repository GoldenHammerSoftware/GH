// Copyright Golden Hammer Software
#include "GHMaterial.h"
#include "GHPlatform/GHStlUtil.h"
#include "GHMaterialCallback.h"
#include <assert.h>

GHMaterial::~GHMaterial(void)
{
	for (GHMaterialCallbackType::Enum type = GHMaterialCallbackType::CT_PERFRAME;
		type < GHMaterialCallbackType::CT_MAX; type = (GHMaterialCallbackType::Enum)((int)type + 1))
	{
		GHStlUtil::deletePointerList(mCallbacks[(int)type]);
	}
	std::map<GHIdentifier, GHMaterial*>::const_iterator poIter;
	for (poIter = mPassOverrides.begin(); poIter != mPassOverrides.end(); ++poIter)
	{
		poIter->second->release();
	}
	for (size_t i = 0; i < mLODs.size(); ++i)
	{
		mLODs[i].mMat->release();
	}
}

void GHMaterial::addCallback(GHMaterialCallbackType::Enum type, GHMaterialCallback* cb)
{
    if (!cb) return;
    mCallbacks[type].push_back(cb);
}

void GHMaterial::applyCallbacks(GHMaterialCallbackType::Enum type, const void* data)
{
    size_t numCallbacks = mCallbacks[(int)type].size();
    for (size_t i = 0; i < numCallbacks; ++i)
    {
        mCallbacks[(int)type][i]->apply(data);
    }
}

GHMaterial* GHMaterial::getPassOverride(const GHIdentifier& id)
{
    std::map<GHIdentifier, GHMaterial*>::const_iterator findIter;
    findIter = mPassOverrides.find(id);
    if (findIter == mPassOverrides.end()) return this;
    return findIter->second;
}

void GHMaterial::addPassOverride(const GHIdentifier& id, GHMaterial* mat)
{
    std::map<GHIdentifier, GHMaterial*>::const_iterator findIter;
    findIter = mPassOverrides.find(id);
    if (findIter != mPassOverrides.end()) findIter->second->release();
    mPassOverrides[id] = mat;
    mat->acquire();
}

void GHMaterial::addLOD(GHMaterial* mat, float minDist)
{
	// assume they are added in order from closest to farthest.
	// todo: sorting.

	assert(mat);
	mat->acquire();
	MaterialLOD lod;
	lod.mMat = mat;
	lod.mMinDist = minDist;
	mLODs.push_back(lod);
}

GHMaterial* GHMaterial::getLOD(float dist)
{
	GHMaterial* ret = this;
	for (size_t i = 0; i < mLODs.size(); ++i)
	{
		if (dist > mLODs[i].mMinDist) {
			ret = mLODs[i].mMat;
		}
	}
	return ret;
}
