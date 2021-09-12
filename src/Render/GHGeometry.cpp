// Copyright Golden Hammer Software
#include "GHGeometry.h"
#include "GHMaterial.h"
#include "GHTransformNode.h"
#include "GHRenderList.h"
#include "GHSphereBounds.h"

GHGeometry::GHGeometry(void)
: mVB(0)
, mMaterial(0)
, mTransform(0)
, mId(0)
, mCloneType(CT_SAMEVB)
, mBounds(0)
{
}

GHGeometry::~GHGeometry(void)
{
    GHRefCounted::changePointer((GHRefCounted*&)mMaterial, 0);
    GHRefCounted::changePointer((GHRefCounted*&)mVB, 0);
    if (mBounds) delete mBounds;
}

void GHGeometry::setVB(GHVertexBufferPtr* vb)
{
    GHRefCounted::changePointer((GHRefCounted*&)mVB, vb);
}

void GHGeometry::setMaterial(GHMaterial* mat)
{
    GHRefCounted::changePointer((GHRefCounted*&)mMaterial, mat);
}

void GHGeometry::setTransform(GHTransformNode* transform)
{
    mTransform = transform;
    if (mBounds) {
        mBounds->setTransform(mTransform);
    }
}

void GHGeometry::addToRenderList(GHRenderList& list, float drawOrder, float dist)
{
    if (!mVB || !mMaterial) return;
    
    const GHTransform* renderTrans = &GHTransform::IDENTITY;
    if (mTransform) renderTrans = &mTransform->getTransform();
    list.addEntry(mVB->get(), mMaterial->getLOD(dist), renderTrans, &mProperties, drawOrder);
}

GHGeometry* GHGeometry::clone(void)
{
    GHGeometry* ret = new GHGeometry;
    ret->setMaterial(mMaterial);
    ret->setId(mId);
    if (mCloneType == CT_SAMEVB)
    {
        ret->setVB(mVB);
    }
    else 
    {
        GHVertexBuffer* newBuffer = mVB->get()->clone();
        ret->setVB(new GHVertexBufferPtr(newBuffer));
    }
    if (mBounds) {
        GHSphereBounds* cloneBounds = new GHSphereBounds;
        cloneBounds->setSphere(mBounds->getLocalSphere());
        ret->setBounds(cloneBounds);
    }
    return ret;
}

void GHGeometry::setBounds(GHSphereBounds* bounds)
{
	if (bounds == mBounds) return;
    if (mBounds) delete mBounds;
    mBounds = bounds;
    if (mBounds) {
        mBounds->setTransform(mTransform);
    }
}

const GHSphereBounds* GHGeometry::getBounds(void) const
{
    return mBounds;
}
