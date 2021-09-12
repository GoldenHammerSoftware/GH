// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"
#include "GHRenderListContainer.h"
#include "GHString/GHIdentifier.h"
#include "GHViewInfo.h"

class GHVertexBuffer;
class GHMaterial;
class GHTransform;
class GHRenderDevice;
class GHPropertyContainer;

class GHRenderList
{
public:
    GHRenderList(int startSize);
    
	void initPass(const GHPoint3& viewPos, const GHViewInfo* viewInfos,
                  const GHIdentifier& passId, bool supportsStereo);

	void addEntry(GHVertexBuffer* vb, GHMaterial* mat, const GHTransform* trans, 
		const GHPropertyContainer* geoData, float drawOrder);
    // sort and blit.
	void flushToDevice(GHRenderDevice& renderDevice);
    // empty out our list for next frame.
	void clear(void) { mOrderedGeometry.reset(); }
    
    // set a chunk of data for everything that is added afterwards.
    void setEntityData(const GHPropertyContainer* entData) { mEntData = entData; }
    
	void setStereoRenderType(GHViewInfo::EyeIndex eye) { mStereoRenderType = eye; }
	GHViewInfo::EyeIndex getStereoRenderType(void) const { return mStereoRenderType; }

protected:
	float calcDistFromCam(const GHTransform& trans);
	void blit(GHViewInfo::EyeIndex eyeIndex);

protected:
	GHRenderListContainer mOrderedGeometry;
	GHPoint3 mViewPos;
    const GHViewInfo* mViewInfos[GHViewInfo::EYE_MAX];
	GHViewInfo::EyeIndex mStereoRenderType{ GHViewInfo::EYE_MONOCLE };
    GHIdentifier mPassId;
    const GHPropertyContainer* mEntData;
	bool mSupportsStereo;
};
