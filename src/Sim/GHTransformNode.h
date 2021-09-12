// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHTransform.h"
#include "GHString/GHIdentifier.h"
#include <vector>

// A class for handling parenting relationships between transforms.
class GHTransformNode
{
public:
    // class to handle doing things to new nodes once they are cloned.
    class CloneCallback
    {
    public:
        virtual ~CloneCallback(void) {}
        virtual void wasCloned(const GHTransformNode& oldNode, GHTransformNode& newNode) = 0;
    };
    
public:
    GHTransformNode(void);
    ~GHTransformNode(void);
    
    void setId(const GHIdentifier& id) { mId = id; }
    const GHIdentifier& getId(void) const { return mId; }
    
    GHTransformNode* getParent(void) const { return mParent; }
    void detach(void);
    
    // get the world transform
    // if we have a parent then it's local * parent->world
    //  otherwise it's just local.
    const GHTransform& getTransform(void);
    // Local transform is in relation to the parent.
    GHTransform& getLocalTransform(void);
    const GHTransform& getLocalTransform(void) const;
    
    //Calculates a new local transform such that parent*local results in the input parameter
    void setWorldTransform(const GHTransform& transform);
    // ensure that the world transform will be recalculated next time
    void invalidateWorldTransform(void);
    
    void addChild(GHTransformNode* child);
    void removeChild(GHTransformNode* child);
    const std::vector<GHTransformNode*>& getChildren(void) const { return mChildren; }
    GHTransformNode* findChild(const GHIdentifier& id);
	const GHTransformNode* findChild(const GHIdentifier& id) const;

    GHTransformNode* clone(CloneCallback* callback);
    // recurse the tree and turn it into an array of nodes.
    void collectTree(std::vector<GHTransformNode*>& ret);
    
private:
    void setParent(GHTransformNode* parent);
    
private:
    GHTransform mLocalTrans;
    GHTransform mWorldTrans;
    GHTransformNode* mParent;
    std::vector<GHTransformNode*> mChildren;
    // possible identifier for finding nodes from the top.
    GHIdentifier mId;
};
