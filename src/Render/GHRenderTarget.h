// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHString.h"

class GHTexture;

// a wrapper for a secondary render target texture.
class GHRenderTarget
{
public:
    enum RTType
    {
        RT_DEFAULT,
        RT_DEPTHONLY
    };
    class Config
    {
    public:
        Config(unsigned int width, unsigned int height, bool mipmap, bool msaa, const char* label)
            : mWidth(width)
            , mHeight(height)
            , mMipmap(mipmap)
            , mMsaa(msaa)
            , mLabel(label, GHString::CHT_COPY)
        {}
        Config() {}

    public:
        unsigned int mWidth{ 256 };
        unsigned int mHeight{ 256 };
        bool mMipmap{ false };
        bool mMsaa{ false };
        GHRenderTarget::RTType mType{ RT_DEFAULT };
        GHString mLabel;
    };

public:
	virtual ~GHRenderTarget(void) {}
    
	// make the target the place we render to.
	virtual void apply(void) = 0;
	// switch rendering back to how it was before apply.
	virtual void remove(void) = 0;
    
	virtual GHTexture* getTexture(void) = 0;
};
