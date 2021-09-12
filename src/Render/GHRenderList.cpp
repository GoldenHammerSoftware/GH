// Copyright Golden Hammer Software
#include "GHRenderList.h"
#include "GHMath/GHTransform.h"
#include "GHMaterial.h"
#include "GHVertexBuffer.h"
#include "GHRenderStats.h"
#include "GHRenderDevice.h"
#include "GHUtils/GHProfiler.h"

GHRenderList::GHRenderList(int startSize)
: mOrderedGeometry(startSize)
, mEntData(0)
, mSupportsStereo(false)
{
}

void GHRenderList::initPass(const GHPoint3& viewPos, const GHViewInfo* viewInfos,
							const GHIdentifier& passId, bool supportsStereo)
{
	mViewPos = viewPos;
    mPassId = passId;
	mSupportsStereo = supportsStereo;

	for (size_t eye = (size_t)GHViewInfo::EYE_MONOCLE; eye < (size_t)GHViewInfo::EYE_MAX; ++eye)
	{
		mViewInfos[(GHViewInfo::EyeIndex)eye] = &viewInfos[(GHViewInfo::EyeIndex)eye];
	}
}

float GHRenderList::calcDistFromCam(const GHTransform& trans)
{
	GHPoint3 objPos;
	trans.getTranslate(objPos);
	GHPoint3 posDiff;
	objPos.minus(mViewPos, posDiff);
	return posDiff.lenSqr();
}

void GHRenderList::addEntry(GHVertexBuffer* vb, GHMaterial* mat, const GHTransform* trans, 
	const GHPropertyContainer* geoData, float drawOrder)
{
	if (!vb || !mat) {
		return;
	}
    if (!mat->getPassMembership().supportsPass(mPassId)) {
        return;
    }
    GHMaterial* poMat = mat->getPassOverride(mPassId);
    
	GHRenderListEntry* entry = mOrderedGeometry.getEntryForAdd();
	entry->mVertexBuffer = vb;
	entry->mMaterial = poMat;
	entry->mTransform = trans;
    entry->mGeoData = geoData;
    entry->mEntityData = mEntData;
    entry->mDrawOrder = entry->mMaterial->getDrawOrder() + drawOrder;
    
	if (entry->mMaterial->isAlpha()) {
        if (trans) {
            entry->mDistFromCam = calcDistFromCam(*trans);
        }
        else {
            entry->mDistFromCam = calcDistFromCam(GHTransform::IDENTITY);
        }
	}
}

void GHRenderList::flushToDevice(GHRenderDevice& renderDevice)
{
	mOrderedGeometry.sort();

	if (!mSupportsStereo || !renderDevice.getIsStereo())
	{
		renderDevice.applyViewInfo(*mViewInfos[GHViewInfo::EYE_MONOCLE]); 
		blit(GHViewInfo::EYE_MONOCLE);
	}
	else
	{
		if (mStereoRenderType == GHViewInfo::EYE_LEFT || mStereoRenderType == GHViewInfo::EYE_MONOCLE)
		{
			renderDevice.applyViewInfo(*mViewInfos[GHViewInfo::EYE_LEFT]);
			blit(GHViewInfo::EYE_LEFT);
		}
		if (mStereoRenderType == GHViewInfo::EYE_RIGHT || mStereoRenderType == GHViewInfo::EYE_MONOCLE)
		{
			renderDevice.applyViewInfo(*mViewInfos[GHViewInfo::EYE_RIGHT]);
			blit(GHViewInfo::EYE_RIGHT);
		}
	}
}

void GHRenderList::blit(GHViewInfo::EyeIndex eyeIndex)
{
	GHPROFILESCOPE("GHRenderList::blit", GHString::CHT_REFERENCE);

	GHMaterial* lastMat = 0;
	GHVertexBuffer* lastVB = 0;
	const GHTransform* lastTrans = 0;
	bool lastTransApplied = false; // some objects have null transform, so the 0 check gets confused.
	const void* lastGeo = 0;
	bool lastGeoApplied = false;
	const void* lastEntity = 0;
	bool lastEntityApplied = false;

	std::vector<GHRenderListEntry*>::const_iterator geoIter;
	std::vector<GHRenderListEntry*>::const_iterator geoBegin = mOrderedGeometry.getEntries().begin();
	std::vector<GHRenderListEntry*>::const_iterator geoEnd = mOrderedGeometry.getEntries().end();

	for (geoIter = geoBegin; geoIter < geoEnd; ++geoIter)
	{
		const GHRenderListEntry* currEntry = (*geoIter);

		GHMaterial* currMat = currEntry->mMaterial;
		if (currMat != lastMat)
		{
			//GHPROFILESCOPE("GHRenderList::blit materialSwap", GHString::CHT_REFERENCE);

			GHRenderStats::recordMaterialChange();
			if (lastMat) {
				lastMat->endMaterial();
			}
			lastMat = currMat;
			currMat->beginMaterial(*mViewInfos[eyeIndex]);
			// reset our values for a new material.
			lastGeo = 0;
			lastGeoApplied = false;
			lastTrans = 0;
			lastTransApplied = false;
			lastEntity = 0;
			lastEntityApplied = false;

			// Redundantly changing the vb settings between materials might not be necessary.
			// It seems unlikely that we'll have the same vb used for a bunch of materials often enough to worry about.
			if (lastVB != 0)
			{
				lastVB->endVB();
				lastVB = 0;
			}
		}

		if (currEntry->mVertexBuffer != lastVB)
		{
			if (lastVB) {
				lastVB->endVB();
			}
			lastVB = currEntry->mVertexBuffer;
			currEntry->mVertexBuffer->prepareVB();
            currMat->beginVB(*currEntry->mVertexBuffer);
		}

		if (lastTrans != currEntry->mTransform || !lastTransApplied)
		{
			currMat->beginTransform(*currEntry->mTransform, *mViewInfos[eyeIndex]); 
			lastTrans = currEntry->mTransform;
			lastTransApplied = true;
		}

		if (lastEntity != currEntry->mEntityData || !lastEntityApplied)
		{
			currMat->beginEntity(currEntry->mEntityData, *mViewInfos[eyeIndex]);
			lastEntity = currEntry->mEntityData;
			lastEntityApplied = true;
		}

		if (lastGeo != currEntry->mGeoData || !lastGeoApplied)
		{
			currMat->beginGeometry(currEntry->mGeoData, *mViewInfos[eyeIndex]);
			lastGeo = currEntry->mGeoData;
			lastGeoApplied = true;
		}
		{
			//GHPROFILESCOPE("GHRenderList::blit vbBlit", GHString::CHT_REFERENCE);
			currEntry->mVertexBuffer->blit();
		}
	}
	if (lastVB)
	{
		lastVB->endVB();
	}
	if (lastMat) {
		lastMat->endMaterial();
	}
}
