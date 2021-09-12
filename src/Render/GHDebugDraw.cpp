// Copyright Golden Hammer Software
#include "GHDebugDraw.h"
#include <algorithm>
#include "GHGeometry.h"
#include "GHGeoCreationUtil.h"
#include "GHMaterial.h"
#include "GHVBFactory.h"

GHDebugDraw* GHDebugDraw::mSingleton = 0;

GHDebugDraw::GHDebugDraw(const GHVBFactory& vbFactory, GHMaterial* debugMaterial)
: mVBFactory(vbFactory)
, mMaterial(0)
, mCurrentIdentifier(0)
, mLineBlitter(0)
, mLinesPerBuffer(1000)
{
    GHRefCounted::changePointer((GHRefCounted*&)mMaterial, debugMaterial);
}

GHDebugDraw::~GHDebugDraw(void)
{
    GHRefCounted::changePointer((GHRefCounted*&)mMaterial, 0);
    GHRefCounted::changePointer((GHRefCounted*&)mLineBlitter, 0);
}

void GHDebugDraw::removeShape(int identifier)
{
    IDMap::iterator mapIter = mIdentifierMap.find(identifier);
    if (mapIter != mIdentifierMap.end())
    {
        GeoEntry& entry = mapIter->second;
        mLinePool.push_back(entry);
        
        // set verts all to 0.
        GHVertexStreamPtr* streamPtr = entry.mGeo->getVB()->get()->getStream(0);
        GHVertexStream* stream = streamPtr->get();
        const GHVertexStreamFormat& streamFormat = stream->getFormat();
        unsigned short stride = streamFormat.getVertexSize();
        float* streamVerts = stream->lockWriteBuffer();
        streamVerts += entry.mLineOffset*stride*mVertsPerLine;
        memset(streamVerts, 0, sizeof(float)*stride*mVertsPerLine);
        stream->unlockWriteBuffer();
        
        mIdentifierMap.erase(mapIter);
    }
}

void GHDebugDraw::collect(GHRenderPass& pass, const GHFrustum* frustum)
{
	GHGeometryRenderable::collect(pass, frustum);
}
 
int GHDebugDraw::storeGeometry(GeoEntry& entry)
{
    int id = mCurrentIdentifier++;
    mIdentifierMap.insert(IDMap::value_type(id, entry));
    return id;
}

int GHDebugDraw::addLine(const GHPoint3& lhsPoint, const GHPoint3& rhsPoint, const GHPoint3& color, float lineThickness) 
{
    if (mLinePool.size() == 0) {
        expandPool();
    }
    GeoEntry entry = mLinePool.back();
    mLinePool.pop_back();
    
    GHGeometry* geo = entry.mGeo;
    GHGeoCreationUtil::setLineVerts(*geo->getVB()->get(), lhsPoint, rhsPoint, &color, entry.mLineOffset*mVertsPerLine, lineThickness);
    return storeGeometry(entry);
}

void GHDebugDraw::setLine(int identifier, const GHPoint3& lhsPoint, const GHPoint3& rhsPoint,
                          const GHPoint3& color, float lineThickness)
{
    IDMap::iterator mapIter = mIdentifierMap.find(identifier);
    if (mapIter != mIdentifierMap.end())
    {
        GeoEntry& entry = mapIter->second;
        GHGeometry* geo = entry.mGeo;
        GHGeoCreationUtil::setLineVerts(*geo->getVB()->get(), lhsPoint, rhsPoint, &color, entry.mLineOffset*mVertsPerLine, lineThickness);
    }
}

void GHDebugDraw::expandPool(void)
{
    GHGeometry* geo = new GHGeometry;
    
    GHVertexBuffer* vertexBuffer = GHGeoCreationUtil::createBoxBuffer(mVBFactory, mLinesPerBuffer, mLineBlitter, mVertsPerLine, mIndexPerLine);
    geo->setVB(new GHVertexBufferPtr(vertexBuffer));
    geo->setMaterial(mMaterial);
    addGeometry(geo);
    
    if (mLineBlitter == 0) {
        mLineBlitter = vertexBuffer->getBlitter();
        mLineBlitter->acquire();
    }
    
	for (unsigned int i = 0; i < mLinesPerBuffer; ++i)
    {
        GeoEntry entry;
        entry.mGeo = geo;
        entry.mLineOffset = i;
        mLinePool.push_back(entry);
    }
}

//---------------------------------------------------------------------------------------------------------------------------

DebugCross::DebugCross(void)
{
	mColors[0].setCoords(1,0,0);
	mColors[1].setCoords(0,1,0);
	mColors[2].setCoords(0,0,1);

	mHalfSizes[0] = mHalfSizes[1] = mHalfSizes[2] = .5f;
	mLineIds[0] = mLineIds[1] = mLineIds[2] = -1;

	mTransform.becomeIdentity();
}

DebugCross::~DebugCross(void)
{
	deactivate();
}

void DebugCross::setColors(const GHPoint3& x, const GHPoint3& y, const GHPoint3& z)
{
	mColors[0] = x;
	mColors[1] = y;
	mColors[2] = z;
	redrawLines();
}

void DebugCross::setSizes(float x, float y, float z)
{
	mHalfSizes[0] = x * .5f;
	mHalfSizes[1] = y * .5f;
	mHalfSizes[2] = z * .5f;
}

void DebugCross::setTransform(const GHTransform& origin)
{
	mTransform = origin;
	redrawLines();
}

void DebugCross::setTranslate(const GHPoint3& translate)
{
	mTransform.setTranslate(translate);
	redrawLines();
}

void DebugCross::redrawLines(void)
{
	if (!checkInit()) { return; }

	for (int i = 0; i < 3; ++i)
	{
		GHPoint3 lhsPoint(0,0,0);
		GHPoint3 rhsPoint(0,0,0);

		lhsPoint[i] = -mHalfSizes[i];
		rhsPoint[i] = mHalfSizes[i];

		mTransform.multDir(lhsPoint, lhsPoint);
		mTransform.multDir(rhsPoint, rhsPoint);

		GHPoint3 translate;
		mTransform.getTranslate(translate);
		lhsPoint += translate;
		rhsPoint += translate;

		GHDebugDraw::getSingleton().setLine(mLineIds[i], lhsPoint, rhsPoint, mColors[i]);
	}
}

bool DebugCross::checkInit(void) 
{
	if (!&GHDebugDraw::getSingleton()) { return false; }

	if (mInitialized) { return true; }

	for (int i = 0; i < 3; ++i)
	{
		mLineIds[i] = GHDebugDraw::getSingleton().addLine(GHPoint3(), GHPoint3(), GHPoint3());
	}

	mInitialized = true;
	return true;
}

void DebugCross::deactivate(void)
{
	if (!mInitialized) { return; }

	for (int i = 0; i < 3; ++i)
	{
		GHDebugDraw::getSingleton().removeShape(mLineIds[i]);
	}

	mInitialized = false;
}

//---------------------------------------------------------------------------------------------------------------------------

DebugRect::DebugRect(void)
    : mColor(1, 0, 0)
{
    for (int i = 0; i < kNumLines; ++i)
    {
        mLineIds[i] = -1;
    }
}

DebugRect::~DebugRect(void)
{
    deactivate();
}

void DebugRect::setRect(const GHRect<float, 3>& rect)
{
    //Draw the top box
    setLine(0, GHPoint3(rect.mMin[0], rect.mMax[1], rect.mMin[2]), GHPoint3(rect.mMin[0], rect.mMax[1], rect.mMax[2]), mColor);
    setLine(1, GHPoint3(rect.mMin[0], rect.mMax[1], rect.mMax[2]), GHPoint3(rect.mMax[0], rect.mMax[1], rect.mMax[2]), mColor);
    setLine(2, GHPoint3(rect.mMax[0], rect.mMax[1], rect.mMax[2]), GHPoint3(rect.mMax[0], rect.mMax[1], rect.mMin[2]), mColor);
    setLine(3, GHPoint3(rect.mMax[0], rect.mMax[1], rect.mMin[2]), GHPoint3(rect.mMin[0], rect.mMax[1], rect.mMin[2]), mColor);

    //Draw the bottom box
    setLine(4, GHPoint3(rect.mMin[0], rect.mMin[1], rect.mMin[2]), GHPoint3(rect.mMin[0], rect.mMin[1], rect.mMax[2]), mColor);
    setLine(5, GHPoint3(rect.mMin[0], rect.mMin[1], rect.mMax[2]), GHPoint3(rect.mMax[0], rect.mMin[1], rect.mMax[2]), mColor);
    setLine(6, GHPoint3(rect.mMax[0], rect.mMin[1], rect.mMax[2]), GHPoint3(rect.mMax[0], rect.mMin[1], rect.mMin[2]), mColor);
    setLine(7, GHPoint3(rect.mMax[0], rect.mMin[1], rect.mMin[2]), GHPoint3(rect.mMin[0], rect.mMin[1], rect.mMin[2]), mColor);

    //Draw the four lines between them
    setLine(8, GHPoint3(rect.mMin[0], rect.mMin[1], rect.mMin[2]), GHPoint3(rect.mMin[0], rect.mMax[1], rect.mMin[2]), mColor);
    setLine(9, GHPoint3(rect.mMin[0], rect.mMin[1], rect.mMax[2]), GHPoint3(rect.mMin[0], rect.mMax[1], rect.mMax[2]), mColor);
    setLine(10, GHPoint3(rect.mMax[0], rect.mMin[1], rect.mMax[2]), GHPoint3(rect.mMax[0], rect.mMax[1], rect.mMax[2]), mColor);
    setLine(11, GHPoint3(rect.mMax[0], rect.mMin[1], rect.mMin[2]), GHPoint3(rect.mMax[0], rect.mMax[1], rect.mMin[2]), mColor);

    //Draw the X on top face
    setLine(12, GHPoint3(rect.mMin[0], rect.mMax[1], rect.mMin[2]), GHPoint3(rect.mMax[0], rect.mMax[1], rect.mMax[2]), mColor);
    setLine(13, GHPoint3(rect.mMin[0], rect.mMax[1], rect.mMax[2]), GHPoint3(rect.mMax[0], rect.mMax[1], rect.mMin[2]), mColor);

    //bottom face
    setLine(14, GHPoint3(rect.mMin[0], rect.mMin[1], rect.mMin[2]), GHPoint3(rect.mMax[0], rect.mMin[1], rect.mMax[2]), mColor);
    setLine(15, GHPoint3(rect.mMin[0], rect.mMin[1], rect.mMax[2]), GHPoint3(rect.mMax[0], rect.mMin[1], rect.mMin[2]), mColor);

    //left face
    setLine(16, GHPoint3(rect.mMin[0], rect.mMin[1], rect.mMin[2]), GHPoint3(rect.mMin[0], rect.mMax[1], rect.mMax[2]), mColor);
    setLine(17, GHPoint3(rect.mMin[0], rect.mMax[1], rect.mMin[2]), GHPoint3(rect.mMin[0], rect.mMin[1], rect.mMax[2]), mColor);

    //right face
    setLine(18, GHPoint3(rect.mMax[0], rect.mMin[1], rect.mMin[2]), GHPoint3(rect.mMax[0], rect.mMax[1], rect.mMax[2]), mColor);
    setLine(19, GHPoint3(rect.mMax[0], rect.mMax[1], rect.mMin[2]), GHPoint3(rect.mMax[0], rect.mMin[1], rect.mMax[2]), mColor);

    //front face
    setLine(20, GHPoint3(rect.mMin[0], rect.mMin[1], rect.mMin[2]), GHPoint3(rect.mMax[0], rect.mMax[1], rect.mMin[2]), mColor);
    setLine(21, GHPoint3(rect.mMin[0], rect.mMax[1], rect.mMin[2]), GHPoint3(rect.mMax[0], rect.mMin[1], rect.mMin[2]), mColor);

    //back face
    setLine(22, GHPoint3(rect.mMin[0], rect.mMin[1], rect.mMax[2]), GHPoint3(rect.mMax[0], rect.mMax[1], rect.mMax[2]), mColor);
    setLine(23, GHPoint3(rect.mMin[0], rect.mMax[1], rect.mMax[2]), GHPoint3(rect.mMax[0], rect.mMin[1], rect.mMax[2]), mColor);

    //internal X's
    setLine(24, GHPoint3(rect.mMin[0], rect.mMin[1], rect.mMin[2]), GHPoint3(rect.mMax[0], rect.mMax[1], rect.mMax[2]), mColor);
    setLine(25, GHPoint3(rect.mMin[0], rect.mMax[1], rect.mMax[2]), GHPoint3(rect.mMax[0], rect.mMin[1], rect.mMin[2]), mColor);
    setLine(26, GHPoint3(rect.mMin[0], rect.mMax[1], rect.mMin[2]), GHPoint3(rect.mMax[0], rect.mMin[1], rect.mMax[2]), mColor);
    setLine(27, GHPoint3(rect.mMin[0], rect.mMin[1], rect.mMax[2]), GHPoint3(rect.mMax[0], rect.mMax[1], rect.mMin[2]), mColor);
}

void DebugRect::setColor(const GHPoint3& color)
{
    mColor = color;
}

void DebugRect::setLineThickness(float thickness)
{
    mLineThickness = thickness;
}

void DebugRect::setLine(int lineIndex, const GHPoint3& lhs, const GHPoint3& rhs, const GHPoint3& color)
{
    if (mLineIds[lineIndex] == -1)
    {
        mLineIds[lineIndex] = GHDebugDraw::getSingleton().addLine(lhs, rhs, color, mLineThickness);
    }
    else
    {
        GHDebugDraw::getSingleton().setLine(mLineIds[lineIndex], lhs, rhs, color, mLineThickness);
    }
}

void DebugRect::deactivate(void)
{
    for (int i = 0; i < kNumLines; ++i)
    {
        if (mLineIds[i] != -1)
        {
            GHDebugDraw::getSingleton().removeShape(mLineIds[i]);
            mLineIds[i] = -1;
        }
    }
}
