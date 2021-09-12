// Copyright Golden Hammer Software
#include "GHBulletDebugDraw.h"
#include "GHDebugDraw.h"
#include "GHMath/GHPoint.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHMath/GHFloat.h"
#include "GHBulletBtInclude.h"

GHBulletDebugDraw::GHBulletDebugDraw(btDynamicsWorld* world)
: mDebugMode(btIDebugDraw::DBG_DrawWireframe)
, mLinesThisFrame(0)
, mWorld(world)
{
    
}

GHBulletDebugDraw::~GHBulletDebugDraw(void)
{
    clear();
}

void GHBulletDebugDraw::drawWorld(void)
{
    clearFrame();
    mWorld->debugDrawWorld();
    clearExcessLines();
}

namespace {
    bool isNormal(const btVector3& color)
    {
        return GHFloat::isEqual(color.x(), 1.f)
            && GHFloat::isEqual(color.y(), 1.f)
            && GHFloat::isZero(color.z());
    }
    
    bool isMeshShape(const btVector3& color)
    {
        return GHFloat::isEqual(color.x(), 1.f)
            && GHFloat::isEqual(color.y(), 1.f)
            && GHFloat::isEqual(color.z(), 1.f);
    }
    
    bool isBallShape(const btVector3& color)
    {
        return GHFloat::isZero(color.x())
            && GHFloat::isEqual(color.y(), 1.f)
            && GHFloat::isEqual(color.z(), 1.f);
    }
    
    bool isPlaneOrSphere(const btVector3& color)
    {
        return GHFloat::isZero(color.x())
            && GHFloat::isEqual(color.y(), 1.f)
            && GHFloat::isZero(color.z());
    }
    
    bool isTransform(const btVector3& color)
    {
        return    (GHFloat::isZero(color.x())
                   && GHFloat::isZero(color.y())
                   && GHFloat::isEqual(color.z(), .7f))
        ||      (GHFloat::isZero(color.x())
                && GHFloat::isEqual(color.y(), .7f)
                 && GHFloat::isZero(color.z()))
        ||      (GHFloat::isEqual(color.x(), .7f)
                 && GHFloat::isZero(color.y())
                 && GHFloat::isZero(color.z()));
        
    }
}

void GHBulletDebugDraw::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
{
    if (isTransform(color)) { return; }
    //if (isPlaneOrSphere(color)) { return; }
    //if (isNormal(color)) { return; }
    //if (isMeshShape(color)) { return; }
    //if (isBallShape(color)) { return; }
    
    ++mLinesThisFrame;
    
    GHPoint3 lhsPoint(from.x(), from.y(), from.z());
    GHPoint3 rhsPoint(to.x(), to.y(), to.z());
    
    size_t numIdentifiers = mIdentifiers.size();
    GHPoint3 ghColor(color.x(), color.y(), color.z());
    if (numIdentifiers < mLinesThisFrame)
    {
        int id = GHDebugDraw::getSingleton().addLine(lhsPoint, rhsPoint, ghColor);
        mIdentifiers.push_back(id);
    }
    else
    {
        int id = mIdentifiers[mLinesThisFrame-1];
        GHDebugDraw::getSingleton().setLine(id, lhsPoint, rhsPoint, ghColor);
    }
}

void GHBulletDebugDraw::drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color)
{
    //not implemented
}

void GHBulletDebugDraw::reportErrorWarning(const char* warningString)
{
    GHDebugMessage::outputString(warningString);
}

void GHBulletDebugDraw::draw3dText(const btVector3& location,const char* textString)
{
    //not implemented
}

void GHBulletDebugDraw::setDebugMode(int debugMode)
{
    mDebugMode = debugMode;
}

int GHBulletDebugDraw::getDebugMode() const
{
    return mDebugMode;
}

void GHBulletDebugDraw::clear(void)
{
    size_t numIds = mIdentifiers.size();
    for (size_t i = 0; i < numIds; ++i)
    {
        int id = mIdentifiers[i];
        GHDebugDraw::getSingleton().removeShape(id);
    }
    mIdentifiers.resize(0);
}

void GHBulletDebugDraw::clearFrame()
{
    mLinesThisFrame = 0;
}

void GHBulletDebugDraw::clearExcessLines(void)
{
    if (mIdentifiers.size() <= mLinesThisFrame) {
        return;
    }
    
    std::vector<int>::iterator iterStart = mIdentifiers.begin() + mLinesThisFrame;
    std::vector<int>::iterator iterEnd = mIdentifiers.end();
    std::vector<int>::iterator iter = iterStart;
    for (; iter != iterEnd; ++iter)
    {
        int id = *iter;
        GHDebugDraw::getSingleton().removeShape(id);
    }
    mIdentifiers.erase(iterStart, iterEnd);
}
