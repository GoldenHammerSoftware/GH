// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHRefCounted.h"
#include <vector>
#include "GHVertexBuffer.h"

class GHVBAnimData : public GHRefCounted
{
public:
	void addFrame(GHVertexBufferPtr* frame);
	GHVertexBufferPtr* getFrame(unsigned short id);
    const std::vector<GHVertexBufferPtr*>& getFrames(void) { return mFrames; }

protected:
    ~GHVBAnimData(void);

private:
    std::vector<GHVertexBufferPtr*> mFrames;
};
