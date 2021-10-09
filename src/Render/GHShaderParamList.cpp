#include "GHShaderParamList.h"

GHShaderParamList::GHShaderParamList(void)
{
	for (int i = 0; i < (int)GHMaterialCallbackType::CT_MAX; ++i)
	{
		mBufferSizes[i] = 0;
	}
}

inline unsigned int padValue(unsigned int val, unsigned int boundarySize)
{
	if (val % boundarySize) {
		val += boundarySize - (val % boundarySize);
	}
	return val;
}

void GHShaderParamList::calcParamOffsets(void)
{
	// NOTE: If we use this outside of dx11/dx12 then the offsets in here might not be correct.
	// d3d pads out variable location to work with 4 byte boundaries with some special rules.
	for (unsigned int cbType = 0; cbType < (int)GHMaterialCallbackType::CT_MAX; ++cbType)
	{
		for (unsigned int paramIdx = 0; paramIdx < mParams.size(); ++paramIdx)
		{
			auto& param = mParams[paramIdx];
			if (param.mCBType == cbType && param.mHandleType != GHMaterialParamHandle::HT_TEXTURE)
			{
				unsigned int currSize = GHMaterialParamHandle::calcHandleSizeBytes(mParams[paramIdx].mHandleType);

				// pad out bytes to floats.
				currSize = padValue(currSize, sizeof(float));

				// elements in an array must be on 4-float boundary, so float[N] is the same size as float4[N]
				if (param.mCount > 1)
				{
					currSize = padValue(currSize, sizeof(float) * 4);
					currSize *= param.mCount;
				}

				// make sure vars don't cross a 16 byte boundary
				unsigned int bufferOffsetFromBoundary = mBufferSizes[cbType] % 16;
				if (bufferOffsetFromBoundary)
				{
					if (currSize >= 16)
					{
						// anything 4 floats or bigger is guaranteed to cross a threshold
						mBufferSizes[cbType] += 16 - bufferOffsetFromBoundary;
					}
					else if (bufferOffsetFromBoundary + currSize > 16)
					{
						mBufferSizes[cbType] += 16 - bufferOffsetFromBoundary;
					}
				}

				mParams[paramIdx].mOffsetBytes = mBufferSizes[cbType];
				mBufferSizes[cbType] += currSize;
			}
		}
	}
}

unsigned int GHShaderParamList::getBufferSize(GHMaterialCallbackType::Enum type) const
{
	return mBufferSizes[(int)type];
}

const GHShaderParamList::Param* GHShaderParamList::getParam(const GHString& semantic) const
{
	for (unsigned int i = 0; i < mParams.size(); ++i)
	{
		if (mParams[i].mSemantic == semantic) {
			return &mParams[i];
		}
	}
	return 0;
}

void GHShaderParamList::addParam(const Param& param)
{
	mParams.push_back(param);
}


