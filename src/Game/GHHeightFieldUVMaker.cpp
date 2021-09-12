// Copyright Golden Hammer Software
#include "GHHeightFieldUVMaker.h"
#include "GHHFUVType.h"
#include "GHHeightField.h"
#include "GHPlatform/GHDebugMessage.h"

GHHeightFieldUVMaker::GHHeightFieldUVMaker(const GHHeightField& hf, GHHFUVType::Enum uvType)
: mUV(0)
{
	if (uvType == GHHFUVType::HFUV_PROJECT)
	{
		GHDebugMessage::outputString("Generating projected uv");
		createProjectedUV(hf);
	}
	else if (uvType == GHHFUVType::HFUV_CLIFF)
	{
		GHDebugMessage::outputString("Generating wrapped uv");
		createWrappedUV(hf);
	}
}

GHHeightFieldUVMaker::~GHHeightFieldUVMaker(void)
{
	delete [] mUV;
}

void GHHeightFieldUVMaker::createProjectedUV(const GHHeightField& hf)
{
	unsigned int width, height;
	hf.getDimensions(width, height);
	mUV = new float[width*height * 2];

	float uPerX = 1.0f / (float)width;
	float vPerY = 1.0f / (float)height;
	for (unsigned int x = 0; x < width; ++x)
	{
		for (unsigned int y = 0; y < height; ++y)
		{
			unsigned int idx = (y*(width * 2)) + x * 2;
			mUV[idx] = (float)x * uPerX;
			mUV[idx + 1] = (float)y * vPerY;
		}
	}
}

void GHHeightFieldUVMaker::createWrappedUV(const GHHeightField& hf)
{
	unsigned int width, height;
	hf.getDimensions(width, height);
	mUV = new float[width*height * 2];

	float fwidth = (float)width * hf.getDistBetweenNodes();
//	float fheight = (float)height * hf.getDistBetweenNodes();
	float uPerDist = 1.0f / fwidth;
	float vPerDist = 1.0f / fwidth;

	// steepness value for determining iscliff
//	static const float cliffThreshold = 1.75f;
	// number of non-cliff nodes before we reset uv
//	static const int cliffFalloff = 40;

	// make a pass to set the u values.
	GHPoint3 posDiff;
	GHPoint3 lastPos(0, 0, 0);
	GHPoint3 currPos(0, 0, 0);
	for (unsigned int y = 0; y < height; ++y)
	{
		//GHDebugMessage::outputString("new row");
		float lastU = 0.0f;
		lastPos.setCoords(0, 0, 0);
		currPos.setCoords(0, 0, 0);

		for (unsigned int x = 0; x < width; ++x)
		{
			unsigned int idx = (y * width * 2) + (x * 2);
			
			if (x == 0) {
				mUV[idx] = 0.0f;
				lastPos[1] = hf.getHeight(x, y);
			}
			else {
				currPos[1] = hf.getHeight(x, y);
				float heightChange = currPos[1] - lastPos[1];
				lastU += hf.getDistBetweenNodes()*uPerDist;
				lastU += heightChange*uPerDist;
				mUV[idx] = lastU;
				lastPos = currPos;

				/*
				currPos[0] = (float)x * hf.getDistBetweenNodes();
				currPos[1] = hf.getHeight(x, y);
				posDiff = currPos;
				posDiff -= lastPos;
				float dist = posDiff.length();
				lastU += (dist*uPerDist);
				mUV[idx] = lastU;
				lastPos = currPos;
				//GHDebugMessage::outputString("U %f", lastU);
				*/

				/*
				bool isCliff = false;
				if (dist > hf.getDistBetweenNodes()*cliffThreshold) isCliff = true;
				if (!lastWasCliff && !isCliff)
				{
					lastU = x*uPerDist;
					mUV[idx] = x*uPerDist;
				}
				if (isCliff) {
					lastWasCliff = cliffFalloff;
				}
				else {
					lastWasCliff--;
					if (lastWasCliff < 0) lastWasCliff = 0;
				}
				*/
			}
			//mUV[idx] = 0.0f;
		}
	}
	// make a second pass to set the v values.
	for (unsigned int x = 0; x < width; ++x)
	{
		float lastV = 0.0f;
		lastPos.setCoords(0, 0, 0);
		currPos.setCoords(0, 0, 0);

		for (unsigned int y = 0; y < height; ++y)
		{
			unsigned int idx = (y * width * 2) + (x * 2);
			if (y == 0) {
				mUV[idx+1] = 0.0f;
				lastPos[1] = hf.getHeight(x, y);
			}
			else {
				currPos[1] = hf.getHeight(x, y);
				float heightChange = currPos[1] - lastPos[1];
				lastV += hf.getDistBetweenNodes()*vPerDist;
				lastV += heightChange*vPerDist;
				mUV[idx + 1] = lastV;
				lastPos = currPos;

				/*
				currPos[2] = (float)y * hf.getDistBetweenNodes();
				currPos[1] = hf.getHeight(x, y);
				posDiff = currPos;
				posDiff -= lastPos;
				float dist = posDiff.length();
				lastV += (dist*vPerDist);
				mUV[idx+1] = lastV;
				lastPos = currPos;
				*/
				/*
				bool isCliff = false;
				if (dist > hf.getDistBetweenNodes()*cliffThreshold) isCliff = true;
				if (!lastWasCliff && !isCliff)
				{
					lastV = y*vPerDist;
					mUV[idx+1] = y*vPerDist;
				}
				if (isCliff) {
					lastWasCliff = cliffFalloff;
				}
				else {
					lastWasCliff--;
					if (lastWasCliff < 0) lastWasCliff = 0;
				}
				*/
			}
			//mUV[idx + 1] = 0.0f;
		}
	}
}
