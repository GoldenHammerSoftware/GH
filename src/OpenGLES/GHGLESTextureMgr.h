// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHGLESInclude.h"
#include <vector>
#include <list>

class GHGLESTexture;

class GHGLESTextureMgr
{
public:
    GHGLESTextureMgr(void);
    ~GHGLESTextureMgr(void);
    
    /// get a texture slot handle that has not been used yet.
	GLuint getUnusedHandle(void);
	/// release a handle when we no longer need the texture.
	void returnHandleToPool(GLuint id);
    
    void registerTexture(GHGLESTexture* texture);
    void unregisterTexture(GHGLESTexture* texture);
    void reinitTextures(void);
    
private:
    std::vector<GLuint> mActiveTextures;
    
    typedef std::list<GHGLESTexture*> RegisteredTextures;
    RegisteredTextures mRegisteredTextures;
};
