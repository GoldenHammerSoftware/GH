// Copyright Golden Hammer Software
#include "GHTransformNode.h"
#include <algorithm>
#include <stddef.h>
#include "GHPlatform/GHDebugMessage.h"

GHTransformNode::GHTransformNode(void)
: mParent(0)
, mId(0)
{
    mLocalTrans.becomeIdentity();
}

GHTransformNode::~GHTransformNode(void)
{
    for (size_t i = 0; i < mChildren.size(); ++i)
    {
        delete mChildren[i];
    }
}

const GHTransform& GHTransformNode::getTransform(void)
{
    if (!mParent) return mLocalTrans;
    mWorldTrans.becomeWorldTransform(mParent->getTransform(), mLocalTrans);
    return mWorldTrans;
}

void GHTransformNode::setWorldTransform(const GHTransform& transform)
{
    if (mParent)
    {
        GHTransform parentInv = mParent->getTransform();
        parentInv.invert();
        transform.mult(parentInv, mLocalTrans);
    }
    else
    {
        mLocalTrans = transform;
    }
}

void GHTransformNode::invalidateWorldTransform(void)
{
    mWorldTrans.resetVersion();
    for (size_t i = 0; i < mChildren.size(); ++i)
    {
        mChildren[i]->invalidateWorldTransform();
    }
}

GHTransform& GHTransformNode::getLocalTransform(void)
{
    return mLocalTrans;
}

const GHTransform& GHTransformNode::getLocalTransform(void) const
{
    return mLocalTrans;
}

void GHTransformNode::setParent(GHTransformNode* parent)
{
    mParent = parent;
    invalidateWorldTransform();
}

void GHTransformNode::detach(void)
{
    if (mParent) mParent->removeChild(this);
}

void GHTransformNode::addChild(GHTransformNode* child)
{
	if (!child)
	{
		GHDebugMessage::outputString("Attempting to add a null GHTransformNode child.");
		return;
	}
    assert(child != this);
    child->setParent(this);
    mChildren.push_back(child);
}

void GHTransformNode::removeChild(GHTransformNode* child)
{
    std::vector<GHTransformNode*>::iterator findIter;
    findIter = std::find(mChildren.begin(), mChildren.end(), child);
    if (findIter != mChildren.end())
    {
        mChildren.erase(findIter);
        child->setParent(0);
    }
    else
    {
        GHDebugMessage::outputString("Failed to find child transformnode for remove");
    }
}

GHTransformNode* GHTransformNode::findChild(const GHIdentifier& id)
{
    return const_cast<GHTransformNode*>(const_cast<const GHTransformNode*>(this)->findChild(id));
}

const GHTransformNode* GHTransformNode::findChild(const GHIdentifier& id) const
{
	if (mId == id) return this;
	for (size_t i = 0; i < mChildren.size(); ++i)
	{
		GHTransformNode* test = mChildren[i]->findChild(id);
		if (test) return test;
	}
	return 0;
}

GHTransformNode* GHTransformNode::clone(CloneCallback* callback)
{
    GHTransformNode* ret = new GHTransformNode;
    ret->setId(mId);
    ret->mLocalTrans.getMatrix() = mLocalTrans.getMatrix();
    for (size_t i = 0; i < mChildren.size(); ++i)
    {
        GHTransformNode* newChild = mChildren[i]->clone(callback);
        ret->addChild(newChild);
    }
    if (callback) callback->wasCloned(*this, *ret);
    return ret;
}

void GHTransformNode::collectTree(std::vector<GHTransformNode*>& ret)
{
    ret.push_back(this);
    for (size_t i = 0; i < mChildren.size(); ++i)
    {
        mChildren[i]->collectTree(ret);
    }
}

