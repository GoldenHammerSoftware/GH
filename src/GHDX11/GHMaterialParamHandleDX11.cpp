// Copyright Golden Hammer Software
#include "GHMaterialParamHandleDX11.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHMaterialShaderInfoDX11.h"

void GHMaterialParamHandleDX11::addShaderHandle(GHMaterialShaderInfoDX11* shader, 
												const GHShaderParamList::Param* param)
{
	mHandles.push_back(Entry(shader, param));
}

void GHMaterialParamHandleDX11::setValue(HandleType type, const void* value)
{
	for (unsigned int i = 0; i < mHandles.size(); ++i)
	{
		if (mHandles[i].second->mHandleType != type) {
			GHDebugMessage::outputString("Shader handle type mismatch in callback");
			continue;
		}
		if (mHandles[i].second->mHandleType == GHMaterialParamHandle::HT_TEXTURE) 
		{
			GHMaterialShaderInfoDX11::TextureList& textureList = mHandles[i].first->mTextures[mHandles[i].second->mCBType];
			for (unsigned int j = 0; j < textureList.size(); ++j)
			{
				if (textureList[j]->getRegister() == mHandles[i].second->mRegister)
				{
					GHMaterialParamHandle::TextureHandle* texHandle = (GHMaterialParamHandle::TextureHandle*)value;
					textureList[j]->setTexture(texHandle->mTexture, texHandle->mWrapMode);
				}
			}
		}
		else {
			unsigned int byteSize = GHMaterialParamHandle::calcHandleSizeBytes(mHandles[i].second->mHandleType);
			GHMaterialShaderInfoDX11::CBuffer* cbuffer = mHandles[i].first->mCBuffers[mHandles[i].second->mCBType];
			::memcpy(cbuffer->mBuffer + mHandles[i].second->mOffsetBytes, value, byteSize);

			//elements in arrays are always padded to 4 floats
			const unsigned int arrayAlignment = 4 * sizeof(float);
			unsigned int arrayPadding = arrayAlignment - (byteSize % arrayAlignment);
			if (arrayPadding == arrayAlignment)
			{
				arrayPadding = 0;
			}
			unsigned int bufferOffset = mHandles[i].second->mOffsetBytes + byteSize + arrayPadding;
			byte* elementPtr = ((byte*)value) + byteSize;
			for (int count = mHandles[i].second->mCount - 1; count > 0; --count, bufferOffset += byteSize + arrayPadding, elementPtr += byteSize)
			{
				::memcpy(cbuffer->mBuffer + bufferOffset, elementPtr, byteSize);
			}
		}
	}
}

