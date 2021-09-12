#include "GHMetalMaterialParamHandle.h"
#include "GHMetalTexture.h"
#include "GHMath/GHMath.h"

GHMetalMaterialParamHandle::GHMetalMaterialParamHandle(void* buffer, size_t offset)
: mBuffer(buffer)
, mOffset(offset)
{
}

void GHMetalMaterialParamHandle::setValue(HandleType type, const void* value)
{
    if (type == HT_TEXTURE)
    {
        // if we have a mBuffer then the wrong constructor was called.
        assert(!mBuffer);
        GHMaterialParamHandle::TextureHandle* texHandle = (GHMaterialParamHandle::TextureHandle*)value;
        GHMetalTexture* ghMetalTexture = (GHMetalTexture*)texHandle->mTexture;
        if (ghMetalTexture)
        {
            ghMetalTexture->setBindId(mOffset);
            ghMetalTexture->setWrapMode(texHandle->mWrapMode);
            ghMetalTexture->bind();
        }
        return;
    }
    size_t copySize = 0;
    float transposeBuf[16];
    switch (type)
    {
        case HT_MAT16:
            copySize = 16*sizeof(float);
            GHMath::transposeMatrix4((float*)value, transposeBuf);
            value = transposeBuf;
            break;
        case HT_MAT16x64:
            // ?
            // todo: transposeMatrix16x64?
            copySize = 16*64*sizeof(float);
            break;
        case HT_VEC4:
            copySize = 4*sizeof(float);
            break;
        case HT_VEC3:
            copySize = 3*sizeof(float);
            break;
        case HT_VEC2:
            copySize = 2*sizeof(float);
            break;
        case HT_FLOAT:
            copySize = sizeof(float);
            break;
        default:
            copySize = sizeof(float);
            break;
    };
    memcpy((char*)mBuffer + mOffset, value, copySize);
}
