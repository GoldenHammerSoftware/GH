// Copyright Golden Hammer Software
#pragma once

#include "GHGeometryRenderable.h"
#include <map>
#include "GHMath/GHPoint.h"
#include "GHVBBlitter.h"
#include "GHMath/GHTransform.h"
#include "GHMath/GHRect.h"

class GHVBFactory;
class GHMaterial;

class GHDebugDraw : public GHGeometryRenderable
{
public:
    static void setSingleton(GHDebugDraw* debugDraw) { delete mSingleton; mSingleton = debugDraw; }
	static GHDebugDraw& getSingleton(void) { return *mSingleton; }
    
    GHDebugDraw(const GHVBFactory& vbFactory, GHMaterial* debugMaterial);
    virtual ~GHDebugDraw(void);
    
    //line thickness is in units of default debug draw line
    int addLine(const GHPoint3& lhsPoint, const GHPoint3& rhsPoint, const GHPoint3& color, float lineThickness=1.0f);
    void setLine(int identifier, const GHPoint3& lhsPoint, const GHPoint3& rhsPoint, const GHPoint3& color, float lineThickness=1.0f);
    
    void removeShape(int identifier);

	virtual void collect(GHRenderPass& pass, const GHFrustum* frustum);

private:
    // struct to say where a line lives.
    struct GeoEntry
    {
        GeoEntry(void) : mGeo(0), mLineOffset(0) {}
        
        GHGeometry* mGeo;
        // how many lines to skip in the geo buffer before our entry.
        unsigned int mLineOffset;
    };

private:
    int storeGeometry(GeoEntry& entry);
    void expandPool(void);
    
private:
    static GHDebugDraw* mSingleton;
    
    const GHVBFactory& mVBFactory;
    GHMaterial* mMaterial;
    
    // how many lines per geo we use.
    unsigned int mLinesPerBuffer;
    
    // the list of line geos currently in use.
    int mCurrentIdentifier;
    typedef std::map<int, GeoEntry> IDMap;
    IDMap mIdentifierMap;
    
    // the pool of unused line geos.
    std::vector<GeoEntry> mLinePool;
    unsigned int mVertsPerLine;
    unsigned int mIndexPerLine;
    
    // shared ib for lines
    GHVBBlitterPtr* mLineBlitter;
    
    
};

class DebugCross
{
public:
	DebugCross(void);
	~DebugCross(void);

	void setColors(const GHPoint3& x, const GHPoint3& y, const GHPoint3& z);
	void setSizes(float x, float y, float z);
	void setTransform(const GHTransform& origin);
	void setTranslate(const GHPoint3& translate);
	void deactivate(void);

private:
	bool checkInit(void);
	void redrawLines(void);

	GHTransform mTransform;
	GHPoint3 mColors[3];
	int mLineIds[3];
	float mHalfSizes[3];
	bool mInitialized { false };
};

class DebugRect
{
public:
    DebugRect(void);
    ~DebugRect(void);

    void setRect(const GHRect<float, 3>& rect);
    void setColor(const GHPoint3& color);
    //in units of default debug draw line thickness
    void setLineThickness(float lineThickness);
    void deactivate(void);

private:
    void setLine(int lineIndex, const GHPoint3& lhs, const GHPoint3& rhs, const GHPoint3& color);

private:
    static const int kNumLines = 28;
    int mLineIds[kNumLines];
    GHPoint3 mColor;
    float mLineThickness{ 1.0f };
};
