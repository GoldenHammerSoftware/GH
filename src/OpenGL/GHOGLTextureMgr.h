// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHOGLInclude.h"
#include <vector>

// A class to manage opengl's texture handles
class GHOGLTextureMgr
{
public:
	GHOGLTextureMgr(void);
	~GHOGLTextureMgr(void);
	
	/// get a texture slot handle that has not been used yet.
	GLuint getUnusedHandle(void);
	/// release a handle when we no longer need the texture.
	void returnHandleToPool(GLuint id);
	
private:
    std::vector<GLuint> mActiveTextures;
};
