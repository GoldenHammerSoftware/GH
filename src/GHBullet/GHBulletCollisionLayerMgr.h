// Copyright Golden Hammer Software
#pragma once

#include <map>

//Manages collision relationships between various layers.
//The layers passed should be powers of 2.
class GHBulletCollisionLayerMgr
{
public:
	void enableCollisionWithAll(short layer);
	void disableCollisionWithAll(short layer);
    void setCollisionWith(short lhsLayer, short rhsLayer, bool collides);
    
    short getMaskForLayer(short layer) const;
    
    
private:
    void setOneWayCollision(short lhsLayer, short rhsLayer, bool collides);
    void applyToMask(short layer, bool collides, short& outMask) const;
    
private:
    typedef std::map<short, short> MaskMap;
    MaskMap mMaskMap;
};
