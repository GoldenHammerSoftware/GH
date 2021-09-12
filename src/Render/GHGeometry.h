// Copyright Golden Hammer Software
#pragma once

#include "GHVertexBuffer.h"
#include "GHString/GHIdentifier.h"
#include "GHUtils/GHPropertyContainer.h"

class GHRenderList;
class GHMaterial;
class GHTransformNode;
class GHSphereBounds;

// a little struct to wrap a set of info used for making one draw call.
class GHGeometry
{   
public:
    // enum to define behavior when clone is called.
    enum CloneType
    {
        CT_SAMEVB=0, // point to the same GHVertexBufferPtr
        CT_CLONEVB, // make a copy of the vb using stream isShared.
    };
    
public:
    GHGeometry(void);
    ~GHGeometry(void);
    
    void addToRenderList(GHRenderList& list, float drawOrder, float distFromCamera);
    
    void setVB(GHVertexBufferPtr* vb);
    GHVertexBufferPtr* getVB(void) { return mVB; }
    const GHVertexBufferPtr* getVB(void) const { return mVB; }
    
    void setMaterial(GHMaterial* mat);
    GHMaterial* getMaterial(void) { return mMaterial; }

    void setTransform(GHTransformNode* transform);
    const GHTransformNode* getTransform(void) const { return mTransform; }
    GHTransformNode* getTransform(void) { return mTransform; }
    
    const GHIdentifier& getId(void) const { return mId; }
    void setId(const GHIdentifier& id) { mId = id; }
    
    void setCloneType(CloneType type) { mCloneType = type; }
    GHGeometry* clone(void);

    GHPropertyContainer& getProperties(void) { return mProperties; }
    
    void setBounds(GHSphereBounds* bounds);
    const GHSphereBounds* getBounds(void) const;
    
private:
    GHSphereBounds* mBounds;
    GHVertexBufferPtr* mVB;
    GHMaterial* mMaterial;
    GHTransformNode* mTransform;
    GHIdentifier mId;
    CloneType mCloneType;
    GHPropertyContainer mProperties;
};
