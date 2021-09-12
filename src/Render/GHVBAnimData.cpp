// Copyright Golden Hammer Software
#include "GHVBAnimData.h"
#include <stddef.h>

GHVBAnimData::~GHVBAnimData(void)
{
    for (size_t i = 0; i < mFrames.size(); ++i)
    {
        mFrames[i]->release();
    }
}

void GHVBAnimData::addFrame(GHVertexBufferPtr* frame)
{
    frame->acquire();
    mFrames.push_back(frame);
}

GHVertexBufferPtr* GHVBAnimData::getFrame(unsigned short id)
{
	size_t numFrames = mFrames.size();
	if (!numFrames || id >= numFrames) {
		return 0;
	}
	return mFrames[id];
}

