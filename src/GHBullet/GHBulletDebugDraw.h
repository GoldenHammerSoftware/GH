// Copyright Golden Hammer Software
#pragma once

#include "GHBulletBtInclude.h"
#include <vector>

class btDynamicsWorld;

class GHBulletDebugDraw : public btIDebugDraw
{
public:
    GHBulletDebugDraw(btDynamicsWorld* world);
    ~GHBulletDebugDraw(void);
    
    virtual void	drawLine(const btVector3& from,const btVector3& to,const btVector3& color);
    
    virtual void	drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);
	virtual void	reportErrorWarning(const char* warningString);
	virtual void	draw3dText(const btVector3& location,const char* textString);
	virtual void	setDebugMode(int debugMode);
	virtual int		getDebugMode() const;
    
    void drawWorld(void);
    void clear(void);
    
private:
    void clearFrame(void);
    void clearExcessLines(void);
    
private:
    btDynamicsWorld* mWorld;
    int mDebugMode;
    size_t mLinesThisFrame;
    std::vector<int> mIdentifiers;
};
