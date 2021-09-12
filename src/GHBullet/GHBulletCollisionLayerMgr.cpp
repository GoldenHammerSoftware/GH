// Copyright Golden Hammer Software
#include "GHBulletCollisionLayerMgr.h"
#include "GHBulletBtInclude.h"
#include <assert.h>

namespace
{
    bool isPowerOfTwo(short num)
    {
        return ! (num & (num - 1));
    }
}

void GHBulletCollisionLayerMgr::applyToMask(short layer, bool collides, short& outMask) const
{
    assert(isPowerOfTwo(layer));
    if (collides) {
        outMask |= layer;
    }
    else {
        outMask &= ~layer;
    }
}

void GHBulletCollisionLayerMgr::enableCollisionWithAll(short layer)
{
	MaskMap::iterator iter = mMaskMap.find(layer);
	if (iter != mMaskMap.end())
	{
		iter->second = btBroadphaseProxy::AllFilter;
	}
	//no else -- not being in the map defaults to all
}

void GHBulletCollisionLayerMgr::disableCollisionWithAll(short layer)
{
	MaskMap::iterator iter = mMaskMap.find(layer);
	if (iter == mMaskMap.end())
	{
		short mask = 0;
		std::pair<MaskMap::iterator, bool> insertion = mMaskMap.insert(MaskMap::value_type(layer, mask));
		assert(insertion.second && "Error in GHBulletCollisionLayerMgr: failed to insert into map.");
	}
	else
	{
		iter->second = 0;
	}
}

void GHBulletCollisionLayerMgr::setCollisionWith(short lhsLayer, short rhsLayer, bool collides)
{
    setOneWayCollision(lhsLayer, rhsLayer, collides);
    setOneWayCollision(rhsLayer, lhsLayer, collides);
}

void GHBulletCollisionLayerMgr::setOneWayCollision(short lhsLayer, short rhsLayer, bool collides)
{    
    MaskMap::iterator iter = mMaskMap.find(lhsLayer);
    if (iter == mMaskMap.end())
    {
        short mask = (short)btBroadphaseProxy::AllFilter;
        applyToMask(rhsLayer, collides, mask);
        
        std::pair<MaskMap::iterator, bool> insertion = mMaskMap.insert(MaskMap::value_type(lhsLayer, mask));
        assert(insertion.second && "Error in GHBulletCollisionLayerMgr: failed to insert into map.");
    }
    else
    {
        applyToMask(rhsLayer, collides, iter->second);
    }
}

short GHBulletCollisionLayerMgr::getMaskForLayer(short layer) const
{
    MaskMap::const_iterator iter = mMaskMap.find(layer);
    if (iter == mMaskMap.end())
    {
        return (short)btBroadphaseProxy::AllFilter;
    }
    return iter->second;
}
