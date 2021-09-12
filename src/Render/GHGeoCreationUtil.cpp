// Copyright Golden Hammer Software
#include "GHGeoCreationUtil.h"
#include "GHVBFactory.h"
#include "GHVBDescription.h"
#include "GHVertexBuffer.h"
#include "GHVertexBufferUtil.h"
#include "GHVBBlitter.h"
#include "GHIndexBufferUtil.h"
#include "GHMath/GHColor.h"
#include "GHSphereBounds.h"

GHVertexBuffer* GHGeoCreationUtil::createBoxBuffer(const GHVBFactory& vbFactory, unsigned int numBoxes,
                                                   GHVBBlitterPtr* blitter,
                                                   unsigned int& vertsPerBox, unsigned int& indexPerBox)
{
    vertsPerBox = 8;
    indexPerBox = 36;
    
    GHVBBlitter::Type blitterType = GHVBBlitter::BT_INDEX;
    if (blitter) blitterType = GHVBBlitter::BT_UNKNOWN;
    GHVBDescription vbDesc(vertsPerBox*numBoxes, indexPerBox*numBoxes, blitterType);
    GHVBDescription::StreamDescription streamDesc;
    streamDesc.mIsShared = true;
    streamDesc.mUsage = GHVBUsage::DYNAMIC;
    streamDesc.mComps.reserve(3);
    streamDesc.mComps.push_back(GHVertexComponent::AP_POS);
    streamDesc.mComps.push_back(GHVertexComponent::AP_DIFFUSE);
	streamDesc.mComps.push_back(GHVertexComponent::AP_UV1);
    vbDesc.addStreamDescription(streamDesc);
    
    GHVertexBuffer* ret = vbFactory.createVB(vbDesc);
    
    GHVertexStreamPtr* streamPtr = ret->getStream(0);
    GHVertexStream* stream = streamPtr->get();
    const GHVertexStreamFormat& streamFormat = stream->getFormat();
    unsigned short stride = streamFormat.getVertexSize();
    float* streamVerts = stream->lockWriteBuffer();
    memset(streamVerts, 0, stride*vertsPerBox*numBoxes*sizeof(float));
    stream->unlockWriteBuffer();
    
    if (blitter)
    {
        ret->replaceBlitter(blitter);
    }
    else
    {
        GHVBBlitterPtr* blitterPtr = ret->getBlitter();
        GHVBBlitterIndex* indexBlitter = (GHVBBlitterIndex*)blitterPtr->get();
        
        unsigned int indexStart = 0;
        unsigned int vertStart = 0;
		for (size_t i = 0; i < numBoxes; ++i)
        {
            fillBoxBlitter(*indexBlitter, vertStart, indexStart);
            indexStart += indexPerBox;
            vertStart += vertsPerBox;
        }
    }
    return ret;
}

GHVertexBuffer* GHGeoCreationUtil::createBox(const GHVBFactory& vbFactory, const GHRect<float, 3>& lhsFace, const GHRect<float, 3>& rhsFace, const GHPoint3* color, GHVBBlitterPtr* blitter)
{
    unsigned int vertsPerBox, indexPerBox;
    GHVertexBuffer* ret = createBoxBuffer(vbFactory, 1, blitter, vertsPerBox, indexPerBox);
    
    //
    //    Vertex Indices in Buffer
    //
    //           5----6   
    //          /|   /|
    //         / 4--/-7 
    //        / /  / /
    //       / /  / /
    //      1----2 /
    //      |/   |/
    //      0----3
    //

    setBoxVerts(*ret, lhsFace, rhsFace, color, 0);
    
    return ret;
}

void GHGeoCreationUtil::fillBoxBlitter(GHVBBlitterIndex& indexBlitter, unsigned int vertStart, unsigned int indexStart)
{
    unsigned short* ibuf = (unsigned short*)(indexBlitter.lockWriteBuffer());
    ibuf += indexStart;
    
    //Order is clockwise if you are facing at the triangle (update comment if this changes)
    
    //front face
    ibuf = GHIndexBufferUtil::insertTriangleIndices(ibuf, vertStart+0, vertStart+1, vertStart+3);
    ibuf = GHIndexBufferUtil::insertTriangleIndices(ibuf, vertStart+1, vertStart+2, vertStart+3);
    
    //rear face
    ibuf = GHIndexBufferUtil::insertTriangleIndices(ibuf, vertStart+6, vertStart+5, vertStart+7);
    ibuf = GHIndexBufferUtil::insertTriangleIndices(ibuf, vertStart+5, vertStart+4, vertStart+7);
    
    //left face
    ibuf = GHIndexBufferUtil::insertTriangleIndices(ibuf, vertStart+5, vertStart+1, vertStart+0);
    ibuf = GHIndexBufferUtil::insertTriangleIndices(ibuf, vertStart+5, vertStart+0, vertStart+4);
    
    //right face
    ibuf = GHIndexBufferUtil::insertTriangleIndices(ibuf, vertStart+3, vertStart+2, vertStart+7);
    ibuf = GHIndexBufferUtil::insertTriangleIndices(ibuf, vertStart+2, vertStart+6, vertStart+7);
    
    //top face
    ibuf = GHIndexBufferUtil::insertTriangleIndices(ibuf, vertStart+1, vertStart+5, vertStart+2);
    ibuf = GHIndexBufferUtil::insertTriangleIndices(ibuf, vertStart+5, vertStart+6, vertStart+2);
    
    //bottom face
    ibuf = GHIndexBufferUtil::insertTriangleIndices(ibuf, vertStart+0, vertStart+3, vertStart+4);
    GHIndexBufferUtil::insertTriangleIndices(ibuf, vertStart+3, vertStart+7, vertStart+4);
    
    indexBlitter.unlockWriteBuffer();
}

void GHGeoCreationUtil::setBoxVerts(GHVertexBuffer& vertexBuffer, const GHRect<float, 3>& lhsFace, const GHRect<float, 3>& rhsFace, const GHPoint3* color, unsigned int vertStart)
{
    //
    //    Vertex Indices in Buffer
    //
    //           5----6
    //          /|   /|
    //         / 4--/-7 
    //        / /  / /
    //       / /  / /
    //      1----2 /
    //      |/   |/
    //      0----3
    //
    
    GHVertexStreamPtr* streamPtr = vertexBuffer.getStream(0);
    GHVertexStream* stream = streamPtr->get();
    const GHVertexStreamFormat& streamFormat = stream->getFormat();
    const GHVertexStreamFormat::ComponentEntry* posCompEntry = streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_POS);
    
    unsigned short stride = streamFormat.getVertexSize();
    float* streamVerts = stream->lockWriteBuffer();
    
    float* pos = streamVerts + posCompEntry->mOffset;
    pos += (vertStart*stride);
    
    pos = fillVertexBufferPoint(pos, lhsFace.mMin, stride);
    pos = fillVertexBufferPoint(pos, GHPoint3(lhsFace.mMin[0], lhsFace.mMax[1], lhsFace.mMin[2]), stride);
    pos = fillVertexBufferPoint(pos, lhsFace.mMax, stride);
    pos = fillVertexBufferPoint(pos, GHPoint3(lhsFace.mMax[0], lhsFace.mMin[1], lhsFace.mMax[2]), stride);
    
    pos = fillVertexBufferPoint(pos, rhsFace.mMin, stride);
    pos = fillVertexBufferPoint(pos, GHPoint3(rhsFace.mMin[0], rhsFace.mMax[1], rhsFace.mMin[2]), stride);
    pos = fillVertexBufferPoint(pos, rhsFace.mMax, stride);
    fillVertexBufferPoint(pos, GHPoint3(rhsFace.mMax[0], rhsFace.mMin[1], rhsFace.mMax[2]), stride);
    
    const GHVertexStreamFormat::ComponentEntry* colorCompEntry = streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_DIFFUSE);
    if (colorCompEntry)
    {
        float* colorBuf = streamVerts + colorCompEntry->mOffset;
        colorBuf += (vertStart*stride);
        
        int32_t colorDword;
        if (color) {
            colorDword = GHColor::convertToDword(*color); 
        } else {
            GHPoint3 dummyColor(1,1,1);
            colorDword = GHColor::convertToDword(dummyColor);
        }
        for (int i = 0; i < 8; ++i) {
            colorBuf = fillVertexBufferColor(colorBuf, colorDword, stride);
        }
    }
    
    stream->unlockWriteBuffer();
}

void GHGeoCreationUtil::setBoxVerts(GHVertexBuffer& vertexBuffer, std::vector<GHPoint3>& lhsFace, std::vector<GHPoint3>& rhsFace, const GHPoint3* color, unsigned int vertStart)
{
    assert(lhsFace.size() == 4);
    assert(rhsFace.size() == 4);
    
    //
    //    Vertex Indices in Buffer
    //
    //           5----6
    //          /|   /|
    //         / 4--/-7
    //        / /  / /
    //       / /  / /
    //      1----2 /
    //      |/   |/
    //      0----3
    //
    
    GHVertexStreamPtr* streamPtr = vertexBuffer.getStream(0);
    GHVertexStream* stream = streamPtr->get();
    const GHVertexStreamFormat& streamFormat = stream->getFormat();
    const GHVertexStreamFormat::ComponentEntry* posCompEntry = streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_POS);
    
    unsigned short stride = streamFormat.getVertexSize();
    float* streamVerts = stream->lockWriteBuffer();
    
    float* pos = streamVerts + posCompEntry->mOffset;
    pos += (vertStart*stride);
    
    pos = fillVertexBufferPoint(pos, lhsFace[0], stride);
    pos = fillVertexBufferPoint(pos, lhsFace[1], stride);
    pos = fillVertexBufferPoint(pos, lhsFace[2], stride);
    pos = fillVertexBufferPoint(pos, lhsFace[3], stride);
    
    pos = fillVertexBufferPoint(pos, rhsFace[0], stride);
    pos = fillVertexBufferPoint(pos, rhsFace[1], stride);
    pos = fillVertexBufferPoint(pos, rhsFace[2], stride);
    fillVertexBufferPoint(pos, rhsFace[3], stride);
    
    const GHVertexStreamFormat::ComponentEntry* colorCompEntry = streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_DIFFUSE);
    if (colorCompEntry)
    {
        float* colorBuf = streamVerts + colorCompEntry->mOffset;
        colorBuf += (vertStart*stride);
        
        int32_t colorDword;
        if (color) {
            colorDword = GHColor::convertToDword(*color);
        } else {
            GHPoint3 dummyColor(1,1,1);
            colorDword = GHColor::convertToDword(dummyColor);
        }
        for (int i = 0; i < 8; ++i) {
            colorBuf = fillVertexBufferColor(colorBuf, colorDword, stride);
        }
    }
    
    stream->unlockWriteBuffer();
}

float* GHGeoCreationUtil::fillVertexBufferPoint(float* buffer, const GHPoint3& point, unsigned short stride)
{
    *buffer = point[0];
    *(buffer+1) = point[1];
    *(buffer+2) = point[2];
    return buffer + stride;
}

float* GHGeoCreationUtil::fillVertexBufferColor(float* buffer, const int32_t color, unsigned short stride)
{
    *((int32_t*)buffer) = color;
    return buffer + stride;
}

GHVertexBuffer* GHGeoCreationUtil::createLine(const GHVBFactory& vbFactory, const GHPoint3& lhsPoint,
                                              const GHPoint3& rhsPoint, const GHPoint3* color,
                                              GHVBBlitterPtr* blitter)
{
    GHRect<float, 3> lhsFace, rhsFace;
    getBoxFacesForLine(lhsPoint, rhsPoint, lhsFace, rhsFace);
    return createBox(vbFactory, lhsFace, rhsFace, color, blitter);
}

void GHGeoCreationUtil::setLineVerts(GHVertexBuffer& vertexBuffer, const GHPoint3& lhsPoint,
                                     const GHPoint3& rhsPoint, const GHPoint3* color, unsigned int vertStart, float lineThickness)
{
    GHRect<float, 3> lhsFace, rhsFace;
    getBoxFacesForLine(lhsPoint, rhsPoint, lhsFace, rhsFace, lineThickness);
    setBoxVerts(vertexBuffer, lhsFace, rhsFace, color, vertStart);
}

void GHGeoCreationUtil::getBoxFacesForLine(const GHPoint3& lhsPoint, const GHPoint3& rhsPoint, GHRect<float, 3>& lhsFace, GHRect<float, 3>& rhsFace, float thickness)
{
    constexpr static float sDefaultHalfThickness = .001f;
    float halfThickness = sDefaultHalfThickness * thickness;

    lhsFace.mMin.setCoords(lhsPoint[0]-halfThickness, lhsPoint[1]-halfThickness, lhsPoint[2]-halfThickness);
    lhsFace.mMax.setCoords(lhsPoint[0]+halfThickness, lhsPoint[1]+halfThickness, lhsPoint[2]+halfThickness);
    
    rhsFace.mMin.setCoords(rhsPoint[0]-halfThickness, rhsPoint[1]-halfThickness, rhsPoint[2]-halfThickness);
    rhsFace.mMax.setCoords(rhsPoint[0]+halfThickness, rhsPoint[1]+halfThickness, rhsPoint[2]+halfThickness);
}

GHVertexBuffer* GHGeoCreationUtil::createQuad(const GHVBFactory& vbFactory, const GHPoint<float, 3>& ul, const GHPoint<float, 3>& br, 
	GHVBBlitterPtr* blitter)
{
	// create the buffers.
	GHVBBlitter::Type blitterType = GHVBBlitter::BT_INDEX;
	if (blitter) blitterType = GHVBBlitter::BT_UNKNOWN;
	GHVBDescription vbDesc(4, 6, blitterType);
	GHVBDescription::StreamDescription streamDesc;
	streamDesc.mIsShared = true;
	streamDesc.mUsage = GHVBUsage::DYNAMIC;
	streamDesc.mComps.reserve(3);
	streamDesc.mComps.push_back(GHVertexComponent::AP_POS);
	streamDesc.mComps.push_back(GHVertexComponent::AP_UV1);
	streamDesc.mComps.push_back(GHVertexComponent::AP_NORMAL);
	vbDesc.addStreamDescription(streamDesc);

	GHVertexBuffer* ret = vbFactory.createVB(vbDesc);

	GHVertexStreamPtr* streamPtr = ret->getStream(0);
	GHVertexStream* stream = streamPtr->get();
	const GHVertexStreamFormat& streamFormat = stream->getFormat();
	unsigned short stride = streamFormat.getVertexSize();
	float* streamVerts = stream->lockWriteBuffer();
	memset(streamVerts, 0, stride * 4 * sizeof(float));
	stream->unlockWriteBuffer();

	// fill in the index buffer.
	if (blitter)
	{
		ret->replaceBlitter(blitter);
	}
	else
	{
		GHVBBlitterPtr* blitterPtr = ret->getBlitter();
		GHVBBlitterIndex* indexBlitter = (GHVBBlitterIndex*)blitterPtr->get();

		// 0 1
		// 2 3
		unsigned short* ibuf = (unsigned short*)(indexBlitter->lockWriteBuffer());
		*ibuf = 0;
		ibuf++;
		*ibuf = 1;
		ibuf++;
		*ibuf = 2;
		ibuf++;
		*ibuf = 2;
		ibuf++;
		*ibuf = 1;
		ibuf++;
		*ibuf = 3;
		indexBlitter->unlockWriteBuffer();
	}

	// fill in the verts.
	const GHVertexStreamFormat::ComponentEntry* posCompEntry = streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_POS);

	streamVerts = stream->lockWriteBuffer();

	float* pos = streamVerts + posCompEntry->mOffset;
	pos = fillVertexBufferPoint(pos, ul, stride);
	pos = fillVertexBufferPoint(pos, GHPoint3(br[0], ul[1], ul[2]), stride);
	pos = fillVertexBufferPoint(pos, GHPoint3(ul[0], br[1], br[2]), stride);
	pos = fillVertexBufferPoint(pos, br, stride);

	const GHVertexStreamFormat::ComponentEntry* uvCompEntry = streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_UV1);
	if (uvCompEntry)
	{
		float* uv = streamVerts + uvCompEntry->mOffset;
		*uv = 1.0f;
		uv++;
		*uv = 0.0f;
		uv = streamVerts + stride + uvCompEntry->mOffset;
		*uv = 0.0f;
		uv++;
		*uv = 0.0f;
		uv = streamVerts + stride*2 + uvCompEntry->mOffset;
		*uv = 1.0f;
		uv++;
		*uv = 1.0f;
		uv = streamVerts + stride*3 + uvCompEntry->mOffset;
		*uv = 0.0f;
		uv++;
		*uv = 1.0f;
	}

	stream->unlockWriteBuffer();

	return ret;
}

template<typename PosType>
void calcSphereBoundsPosType(const float* vbData, unsigned int numVerts, unsigned short stride, const GHVertexStreamFormat::ComponentEntry* posCompEntry, GHSphereBounds& bounds)
{
	GHSphere sphere;
	sphere.mCenter.setCoords(0,0,0);
	sphere.mRadius = 0;

	if (numVerts == 0) {
		return;
	}

	//first calculate the center
	for (unsigned int i = 0; i < numVerts; ++i)
	{
		PosType* posData = (PosType*)( vbData + i*stride + posCompEntry->mOffset );

		//our spheres are explicitly 3d. 2d positions are acceptable (with zero in the z value), but we will ignore dimensions greater than 3
		const int MAX_SPHERE_DIM = 3;
		for (int c = 0; c < posCompEntry->mCount && c < MAX_SPHERE_DIM; ++c)
		{
			sphere.mCenter[c] += (float)posData[c];
		}
	}

	sphere.mCenter /= (float)numVerts;

	//then calculate the radius squared
	for (unsigned int i = 0; i < numVerts; ++i)
	{
		PosType* posData = (PosType*) (vbData + i*stride + posCompEntry->mOffset);

		//our spheres are explicitly 3d. 2d positions are acceptable (with zero in the z value), but we will ignore dimensions greater than 3
		const int MAX_SPHERE_DIM = 3;
		GHPoint3 pos(0,0,0);
		for (int c = 0; c < posCompEntry->mCount && c < MAX_SPHERE_DIM; ++c)
		{
			pos[c] = (float)posData[c];
		}

		pos -= sphere.mCenter;
		float distSqr = pos.lenSqr();
		if (distSqr > sphere.mRadius)
		{
			sphere.mRadius = distSqr;
		}
	}

	sphere.mRadius = sqrt(sphere.mRadius);

	bounds.setSphere(sphere);
}

bool GHGeoCreationUtil::calcSphereBounds(const GHVertexBuffer& vertexBuffer, GHSphereBounds& bounds)
{
	auto& streams = vertexBuffer.getStreams();
	GHVertexStreamPtr* posStream = 0;
	const GHVertexStreamFormat::ComponentEntry* posCompEntry = 0;
	for (size_t i = 0; i < streams.size(); ++i)
	{
		auto& format = streams[i]->get()->getFormat();
		auto componentEntry = format.getComponentEntry(GHVertexComponentShaderID::SI_POS);
		if (componentEntry)
		{
			posCompEntry = componentEntry;
			posStream = streams[i];
			break;
		}
	}

	if (posStream == 0) {
		return false;
	}

	unsigned int numVerts = posStream->get()->getNumVerts();
	unsigned short stride = posStream->get()->getFormat().getVertexSize();

	const float* vbData = posStream->get()->lockReadBuffer();

	switch (posCompEntry->mType)
	{
		case GHVertexComponentType::CT_BYTE: 
			calcSphereBoundsPosType<int8_t>(vbData, numVerts, stride, posCompEntry, bounds);
			break;
		case GHVertexComponentType::CT_UBYTE: 
			calcSphereBoundsPosType<uint8_t>(vbData, numVerts, stride, posCompEntry, bounds);
			break;
		case GHVertexComponentType::CT_SHORT: 
			calcSphereBoundsPosType<short>(vbData, numVerts, stride, posCompEntry, bounds);
			break;
		case GHVertexComponentType::CT_FLOAT: 
			calcSphereBoundsPosType<float>(vbData, numVerts, stride, posCompEntry, bounds);
			break;
	}

	posStream->get()->unlockReadBuffer();

	return true;
}
