// Copyright Golden Hammer Software
#include "GHVertexBufferUtil.h"
#include "GHVertexStream.h"
#include "GHVertexStreamFormat.h"
#include "GHVertexBuffer.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHMath/GHFloat.h"

void GHVertexBufferUtil::createUVRect(GHVertexBuffer& buffer, const GHPoint2& min, const GHPoint2& max)
{
    GHVertexStreamPtr* streamPtr = buffer.getStream(0);
    GHVertexStream* stream = streamPtr->get();
    const GHVertexStreamFormat& streamFormat = stream->getFormat();
    
    float* streamVerts = stream->lockWriteBuffer();
    fillUVRect(streamVerts, streamFormat, min, max);
    stream->unlockWriteBuffer();
}

void GHVertexBufferUtil::fillUVRect(float* quadStart, const GHVertexStreamFormat& streamFormat,
                                    const GHPoint2& min, const GHPoint2& max)
{
    const GHVertexStreamFormat::ComponentEntry* uvCompEntry = streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_UV1);
    
    float* uv;
    uv = quadStart + uvCompEntry->mOffset;
    *uv = min[0]; uv++; *uv = min[1];

    quadStart += streamFormat.getVertexSize();
    uv = quadStart + uvCompEntry->mOffset;
    *uv = max[0]; uv++; *uv = min[1];

    quadStart += streamFormat.getVertexSize();
    uv = quadStart + uvCompEntry->mOffset;
    *uv = min[0]; uv++; *uv = max[1];

    quadStart += streamFormat.getVertexSize();
    uv = quadStart + uvCompEntry->mOffset;
    *uv = max[0]; uv++; *uv = max[1];
}

void GHVertexBufferUtil::fillColors(float* streamVerts,
                                    const GHVertexStreamFormat& streamFormat,
                                    const GHVertexStreamFormat::ComponentEntry& colorCompEntry, 
                                    int numVerts, int32_t textColorDword)
{
    for (int i = 0; i < numVerts; ++i)
    {
        float* color = streamVerts + colorCompEntry.mOffset;
        *((int32_t*)color) = textColorDword;
        streamVerts += streamFormat.getVertexSize();
    }
}

void GHVertexBufferUtil::fillRect2D(float* quadStart, const GHVertexStreamFormat& streamFormat,
                                    const GHPoint2& min, const GHPoint2& max)
{
    const GHVertexStreamFormat::ComponentEntry* posCompEntry = streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_POS);
    float* pos = quadStart + posCompEntry->mOffset;
    *pos = min[0]; pos++;
    *pos = min[1]; pos++;
    
    quadStart += streamFormat.getVertexSize();
    pos = quadStart + posCompEntry->mOffset;
    *pos = max[0]; pos++;
    *pos = min[1]; pos++;
    
    quadStart += streamFormat.getVertexSize();
    pos = quadStart + posCompEntry->mOffset;
    *pos = min[0]; pos++;
    *pos = max[1]; pos++;
    
    quadStart += streamFormat.getVertexSize();
    pos = quadStart + posCompEntry->mOffset;
    *pos = max[0]; pos++;
    *pos = max[1]; pos++;
}

void GHVertexBufferUtil::fillGrid2D(float* gridStart, const GHVertexStreamFormat& streamFormat,
									const GHPoint2& min, const GHPoint2& max, 
									unsigned int gridWidth, unsigned int gridHeight, 
									float angleRadians, float aspectRatio)
{
	assert(gridWidth > 0);
	assert(gridHeight > 0);

	const GHVertexStreamFormat::ComponentEntry* posCompEntry = streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_POS);
	float* pos = gridStart + posCompEntry->mOffset;
	
	float xInc = (max[0] - min[0]) / (float)gridWidth;
	float yPos = min[1];
	float yInc = (max[1] - min[1]) / (float)gridHeight;

	if (!GHFloat::isZero(angleRadians)) //Version that also rotates the verts
	{
		GHPoint2 center = max;
		center -= min;
		center *= 0.5f;
		center += min;

		float sinAngle = sin(angleRadians);
		float cosAngle = cos(angleRadians);

		for (unsigned int y = 0; y < gridHeight + 1; ++y)
		{
			float relY = yPos - center[1];

			float xPos = min[0];
			for (unsigned int x = 0; x < gridWidth + 1; ++x)
			{
				float relX = (xPos - center[0])*aspectRatio;


				*pos = ((relX*cosAngle - relY*sinAngle)/aspectRatio) + center[0];
				pos++;
				*pos = (relX*sinAngle + relY*cosAngle) + center[1];
				pos++;

				gridStart += streamFormat.getVertexSize();
				pos = gridStart + posCompEntry->mOffset;
				xPos += xInc;
			}
			yPos += yInc;
		}

	}
	else //version that doesn't include rotation
	{
		for (unsigned int y = 0; y < gridHeight + 1; ++y)
		{
			float xPos = min[0];
			for (unsigned int x = 0; x < gridWidth + 1; ++x)
			{
				*pos = xPos;
				pos++;
				*pos = yPos;
				pos++;

				gridStart += streamFormat.getVertexSize();
				pos = gridStart + posCompEntry->mOffset;
				xPos += xInc;
			}
			yPos += yInc;
		}
	}
}

void GHVertexBufferUtil::createUVGrid(GHVertexBuffer& buffer, const GHPoint2& min, const GHPoint2& max,
	unsigned int gridWidth, unsigned int gridHeight, float uvAngleInRadians)
{
	GHVertexStreamPtr* streamPtr = buffer.getStream(0);
	GHVertexStream* stream = streamPtr->get();
	const GHVertexStreamFormat& streamFormat = stream->getFormat();

	float xPos = min[0];
	float xInc = (max[0] - min[0]) / (float)gridWidth;
	float yPos = min[1];
	float yInc = (max[1] - min[1]) / (float)gridHeight;

	float* streamVerts = stream->lockWriteBuffer();
	const GHVertexStreamFormat::ComponentEntry* uvCompEntry = streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_UV1);
	float* uv;
	float* gridPos = streamVerts;
	uv = gridPos + uvCompEntry->mOffset;

	float sinT = sin(uvAngleInRadians);
	float cosT = cos(uvAngleInRadians);

	for (unsigned int y = 0; y < gridHeight+1; ++y)
	{
		xPos = min[0];
		for (unsigned int x = 0; x < gridWidth+1; ++x)
		{
			*uv = (xPos - 0.5f)*cosT + (yPos - 0.5f)*sinT + 0.5f;
			uv++; 
			*uv = -(xPos - 0.5f)*sinT + (yPos - 0.5f)*cosT + 0.5f;

			gridPos += streamFormat.getVertexSize();
			uv = gridPos + uvCompEntry->mOffset;

			xPos += xInc;
		}
		yPos += yInc;
	}

	stream->unlockWriteBuffer();
}

void GHVertexBufferUtil::clampToScreenEdges(const GHRect<float, 2>& posIn, const GHRect<float, 2>& uvIn, GHRect<float, 2>& posOut, GHRect<float, 2>& uvOut)
{
	for (size_t i = 0; i < 2; ++i)
	{
		float totalPosDist = posIn.mMax[i] - posIn.mMin[i];
		float totalUVDist = uvIn.mMax[i] - uvIn.mMin[i];

		if (posIn.mMin[i] < 0.0f) {
			posOut.mMin[i] = 0.0f;
			float pctClamped = (-1.0f*posIn.mMin[i]) / totalPosDist;
			uvOut.mMin[i] = uvIn.mMin[i] + pctClamped*totalUVDist;
		}
		else if (posIn.mMin[i] > 1.0f) {
			posOut.mMin[i] = 1.0f;
			float pctClamped = (posIn.mMin[i] - 1) / totalPosDist;
			uvOut.mMin[i] = uvIn.mMax[i] - pctClamped*totalUVDist;
		}
		else {
			posOut.mMin[i] = posIn.mMin[i];
			uvOut.mMin[i] = uvIn.mMin[i];
		}

		if (posIn.mMax[i] < 0.0f) {
			posOut.mMax[i] = 0.0f;
			float pctClamped = (-1.0f*posIn.mMax[i]) / totalPosDist;
			uvOut.mMax[i] = uvIn.mMax[i] + pctClamped*totalUVDist;
		}
		else if (posIn.mMax[i] > 1.0f) {
			posOut.mMax[i] = 1.0f;
			float pctClamped = (posIn.mMax[i] - 1) / totalPosDist;
			uvOut.mMax[i] = uvIn.mMax[i] - pctClamped*totalUVDist;
		}
		else {
			posOut.mMax[i] = posIn.mMax[i];
			uvOut.mMax[i] = uvIn.mMax[i];
		}
	}
}

void GHVertexBufferUtil::populateTangents(GHVertexBuffer& vertexBuffer)
{
	// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/#computing-the-tangents-and-bitangents

	if (!vertexBuffer.getBlitter() || vertexBuffer.getBlitter()->get()->getType() != GHVBBlitter::BT_INDEX)
	{
		GHDebugMessage::outputString("GHVertexBufferUtil::populateTangents only works for index vbs.  Implement others if needed.");
		return;
	}
	GHVBBlitterIndex* indexBlitter = (GHVBBlitterIndex*)vertexBuffer.getBlitter()->get();

	const std::vector<GHVertexStreamPtr*>& streams = vertexBuffer.getStreams();

	GHVertexStream* posStream{ 0 };
	const GHVertexStreamFormat::ComponentEntry* posComp{ 0 };
	GHVertexStream* uvStream{ 0 };
	const GHVertexStreamFormat::ComponentEntry* uvComp{ 0 };
	GHVertexStream* tanStream{ 0 };
	const GHVertexStreamFormat::ComponentEntry* tanComp{ 0 };

	// find the relevant components.
	for (auto streamIter = streams.begin(); streamIter != streams.end(); ++streamIter)
	{
		GHVertexStreamPtr* currStreamPtr = *streamIter;
		GHVertexStream* currStream = currStreamPtr->get();
		const GHVertexStreamFormat& currFormat = currStream->getFormat();

		const GHVertexStreamFormat::ComponentEntry* testComp;
		testComp = currFormat.getComponentEntry(GHVertexComponentShaderID::SI_POS);
		if (testComp)
		{
			posStream = currStream;
			posComp = testComp;
		}
		testComp = currFormat.getComponentEntry(GHVertexComponentShaderID::SI_UV1);
		if (testComp)
		{
			uvStream = currStream;
			uvComp = testComp;
		}
		testComp = currFormat.getComponentEntry(GHVertexComponentShaderID::SI_TANGENT);
		if (testComp)
		{
			tanStream = currStream;
			tanComp = testComp;
		}
	}

	if (!posStream || !uvStream || !tanStream)
	{
		GHDebugMessage::outputString("Failed to find component streams for generating tangents");
		return;
	}

	const float* posBuf = posStream->lockReadBuffer();
	const float* uvBuf = uvStream->lockReadBuffer();
	float* tanBuf = tanStream->lockWriteBuffer();
	const unsigned short* ibBuf = indexBlitter->lockReadBuffer();

	// first make sure we're starting with 0's so we can add the results of each triangle.
	float* tanStart = tanBuf + tanComp->mOffset;
	for (size_t vIdx = 0; vIdx < tanStream->getNumVerts(); ++vIdx)
	{
		memset(tanStart, 0, 3 * sizeof(float));
		tanStart += tanStream->getFormat().getVertexSize();
	}

	GHPoint3 pos0, pos1, pos2;
	GHPoint2 uv0, uv1, uv2;
	GHPoint3 tan, tanPart2;
	GHPoint3 deltaPos1, deltaPos2;
	GHPoint2 deltaUV1, deltaUV2;

	for (size_t tri = 0; tri < indexBlitter->getNumIndices(); tri += 3)
	{
		unsigned short v0Idx = ibBuf[tri];
		unsigned short v1Idx = ibBuf[tri+1];
		unsigned short v2Idx = ibBuf[tri+2];
        assert( v0Idx < posStream->getNumVerts());
        assert( v1Idx < posStream->getNumVerts());
        assert( v2Idx < posStream->getNumVerts());
        
		const float* pos0Start = posBuf + posComp->mOffset + posStream->getFormat().getVertexSize()*v0Idx;
		const float* pos1Start = posBuf + posComp->mOffset + posStream->getFormat().getVertexSize()*v1Idx;
		const float* pos2Start = posBuf + posComp->mOffset + posStream->getFormat().getVertexSize()*v2Idx;
		pos0.setCoords(pos0Start);
		pos1.setCoords(pos1Start);
		pos2.setCoords(pos2Start);

		const float* uv0Start = uvBuf + uvComp->mOffset + uvStream->getFormat().getVertexSize()*v0Idx;
		const float* uv1Start = uvBuf + uvComp->mOffset + uvStream->getFormat().getVertexSize()*v1Idx;
		const float* uv2Start = uvBuf + uvComp->mOffset + uvStream->getFormat().getVertexSize()*v2Idx;
		uv0.setCoords(uv0Start);
		uv1.setCoords(uv1Start);
		uv2.setCoords(uv2Start);

		float* tan0Start = tanBuf + tanComp->mOffset + tanStream->getFormat().getVertexSize()*v0Idx;
		float* tan1Start = tanBuf + tanComp->mOffset + tanStream->getFormat().getVertexSize()*v1Idx;
		float* tan2Start = tanBuf + tanComp->mOffset + tanStream->getFormat().getVertexSize()*v2Idx;
		
		deltaPos1 = pos1;
		deltaPos1 -= pos0;
		deltaPos2 = pos2;
		deltaPos2 -= pos0;

		deltaUV1 = uv1;
		deltaUV1 -= uv0;
		deltaUV2 = uv2;
		deltaUV2 -= uv0;

		float r = 1.0f;
		float r_recip = (deltaUV1[0] * deltaUV2[1] - deltaUV1[1] * deltaUV2[0]);
		// catch nan
		if (r_recip != 0)
		{
			r = 1.0f / r_recip;
		}
		tan[0] = ((deltaPos1[0] * deltaUV2[1]) - (deltaPos2[0] * deltaUV1[1])) * r;
		tan[1] = ((deltaPos1[1] * deltaUV2[1]) - (deltaPos2[1] * deltaUV1[1])) * r;
		tan[2] = ((deltaPos1[2] * deltaUV2[1]) - (deltaPos2[2] * deltaUV1[1])) * r;

		//  bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r;

		if (tan[0] == 0 && tan[1] == 0 && tan[2] == 0)
		{
			// This should not have happened and is probably indicative of a 0-volume texture mapping for the triangle.
			// (0,1,0) is a much more graceful garbage tangent than (0,0,0).
			tan.setCoords(0.0f, 1.0f, 0.0f);
		}

		// We can add to existing tangent because we set them to 0 and will have to normalize in the shader anyway.
		// We avoid normalizing before adding in order to give additional weight to larger triangles.
		for (size_t i = 0; i < 3; ++i)
		{
			*tan0Start += tan[i];
			*tan1Start += tan[i];
			*tan2Start += tan[i];
			tan0Start++;
			tan1Start++;
			tan2Start++;
		}
	}

	indexBlitter->unlockReadBuffer();
	posStream->unlockReadBuffer();
	uvStream->unlockReadBuffer();
	tanStream->unlockWriteBuffer();
}
