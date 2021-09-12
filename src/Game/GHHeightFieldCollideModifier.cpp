// Copyright Golden Hammer Software
#include "GHHeightFieldCollideModifier.h"
#include "GHHeightField.h"
#include "GHPhysicsSim.h"

GHHeightFieldCollideModifier::GHHeightFieldCollideModifier(GHHeightField& hf, 
	const GHPhysicsSim& physics, float worldHeight, int collisionLayer, 
	unsigned int edgeSmooth, float edgeSmoothPct,
	unsigned int colSmooth, float colSmoothPct,
	GHHeightField* noiseHF)
: mHF(hf)
, mPhysics(physics)
, mWorldHeight(worldHeight)
, mCollisionLayer(collisionLayer)
, mEdgeSmooth(edgeSmooth)
, mColSmooth(colSmooth)
, mColSmoothPct(colSmoothPct)
, mEdgeSmoothPct(edgeSmoothPct)
, mNoiseHF(noiseHF)
{
}

GHHeightFieldCollideModifier::~GHHeightFieldCollideModifier(void)
{
	delete mNoiseHF;
}

void GHHeightFieldCollideModifier::activate(void)
{
	GHDebugMessage::outputString("Running GHHeightFieldCollideModifier");

	unsigned int nodeWidth;
	unsigned int nodeHeight;
	mHF.getDimensions(nodeWidth, nodeHeight);
	float nodeDist = mHF.getDistBetweenNodes();
	float* heights = mHF.getHeights();

	// keep track of which nodes collided.
	bool* collided = new bool[nodeWidth*nodeHeight];

	// go through and run collisions.
	GHPoint3 rayStart(0, mWorldHeight, 0);
	GHPoint3 rayEnd(0, -mWorldHeight, 0);
	GHPhysicsCollisionData::GHPhysicsCollisionObject colRes;
	for (unsigned int x = 0; x < nodeWidth; ++x)
	{
		rayStart[2] = 0;
		rayEnd[2] = 0;
		for (unsigned int y = 0; y < nodeHeight; ++y)
		{
			rayStart[2] += nodeDist;
			rayEnd[2] += nodeDist;

			unsigned int nodeIdx = (y*nodeWidth) + x;
			collided[nodeIdx] = mPhysics.castRay(rayStart, rayEnd, mCollisionLayer, colRes);
			if (collided[nodeIdx])
			{
				heights[nodeIdx] = colRes.mHitPos[1];
			}
		}
		rayStart[0] += nodeDist;
		rayEnd[0] += nodeDist;
	}

	addCollisionNoise(collided, nodeWidth, nodeHeight);
	smoothCollisionEdges(collided, nodeWidth, nodeHeight);

	delete[] collided;
	GHDebugMessage::outputString("finished GHHeightFieldCollideModifier");
}

void GHHeightFieldCollideModifier::deactivate(void)
{
}

void GHHeightFieldCollideModifier::addCollisionNoise(bool* collided, unsigned int nodeWidth, unsigned int nodeHeight)
{
	if (!mNoiseHF) {
		return;
	}
	// todo? support height fields that don't have identical sizes
	unsigned int noiseWidth, noiseHeight;
	mHF.getDimensions(noiseWidth, noiseHeight);
	if (noiseWidth != nodeWidth || noiseHeight != nodeHeight)
	{
		GHDebugMessage::outputString("Noise HF size does not match source HF.");
		return;
	}

	GHDebugMessage::outputString("Adding collision noise");
	for (unsigned int x = 0; x < nodeWidth; ++x)
	{
		for (unsigned int y = 0; y < nodeHeight; ++y)
		{
			unsigned int nodeIdx = (y*nodeWidth) + x;
			if (collided[nodeIdx])
			{
				float noiseheight = mNoiseHF->getHeight(x, y);
				// todo: move scale into data
				//noiseheight *= 0.5f;
				//noiseheight -= 0.1f;
				float srcHeight = mHF.getHeight(x, y);
				//GHDebugMessage::outputString("%f + %f", srcHeight, noiseheight);
				mHF.setHeight(x, y, noiseheight + srcHeight);
			}
		}
	}
}

void GHHeightFieldCollideModifier::smoothCollisionEdges(bool* collided, unsigned int nodeWidth, unsigned int nodeHeight)
{
	// if smoothing is called for:
	//  make a copy of the vert heights
	//  detect edges
	//  smooth any edge nodes.
	if (!mEdgeSmooth && !mColSmooth) {
		return;
	}
	GHDebugMessage::outputString("Smoothing edges");

	float* sourceHeights = new float[nodeWidth*nodeHeight];
	memcpy(sourceHeights, mHF.getHeights(), nodeWidth*nodeHeight*sizeof(float));
	for (unsigned int x = 0; x < nodeWidth; ++x)
	{
		for (unsigned int y = 0; y < nodeHeight; ++y)
		{
			unsigned int nodeIdx = (y*nodeWidth) + x;

			unsigned int smoothSize = 0;
			float smoothPct = 0.0f;
			if (collided[nodeIdx]) {
				if (!mColSmooth) continue;
				smoothSize = mColSmooth;
				smoothPct = mColSmoothPct;
			}
			else {
				if (!mEdgeSmooth) continue;
				smoothSize = mEdgeSmooth;
				smoothPct = mEdgeSmoothPct;
			}

			// see if is edge and calc the average.
			unsigned int minX = std::max(0, (int)x - (int)smoothSize);
			unsigned int maxX = std::min(nodeWidth, x + smoothSize + 1);
			unsigned int minY = std::max(0, (int)y - (int)smoothSize);
			unsigned int maxY = std::min(nodeHeight, y + smoothSize + 1);
			int numNeighbors = 0;
			bool hasCollided = false;
			bool hasNotCollided = false;
			float heightSum = 0;
			for (unsigned int testX = minX; testX < maxX; ++testX)
			{
				for (unsigned int testY = minY; testY < maxY; ++testY)
				{
					unsigned int testIdx = (testY*nodeWidth) + testX;
					if (collided[testIdx]) {
						hasCollided = true;
					}
					else {
						hasNotCollided = true;
					}
					numNeighbors++;
					heightSum += sourceHeights[testIdx];
				}
			}
			if ((hasCollided && hasNotCollided) || collided[nodeIdx])
			{
				float heightAve = heightSum / (float)numNeighbors;
				float oldHeight = mHF.getHeight(x, y);
				float heightDiff = heightAve - oldHeight;
				heightDiff *= smoothPct;
				mHF.setHeight(x, y, oldHeight + heightDiff);
			}
		}
	}

	delete[] sourceHeights;
}
