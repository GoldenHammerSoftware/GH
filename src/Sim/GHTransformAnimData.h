// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHRefCounted.h"
#include "GHMath/GHPoint.h"
#include <vector>
#include <stddef.h>

// source data for running anims that move a transform around.
class GHTransformAnimData : public GHRefCounted
{
public:
	GHTransformAnimData(size_t expectedFrameCount = 0)
    {
        mFrames.reserve(expectedFrameCount);
    }
    
	// add a frame to our data.  
	// this should be done in order of frame number.
	void addFrame(const GHPoint16& frame) { mFrames.push_back(frame); }
    
	// get access to a frame at a given index.
	// can return 0 if out of bounds or no frames exist.
	const GHPoint16* getFrame(size_t id) const;
    GHPoint16* getFrame(size_t id);
    const std::vector<GHPoint16>& getFrames(void) const { return mFrames; }
    std::vector<GHPoint16>& getFrames(void) { return mFrames; }
    
protected:
	// our frame information.
	std::vector<GHPoint16> mFrames;
};

inline const GHPoint16* GHTransformAnimData::getFrame(size_t id) const
{
	size_t numFrames = mFrames.size();
	if (!numFrames || id >= numFrames) {
		return 0;
	}
	return &mFrames[id];
}

inline GHPoint16* GHTransformAnimData::getFrame(size_t id)
{
	size_t numFrames = mFrames.size();
	if (!numFrames || id >= numFrames) {
		return 0;
	}
	return &mFrames[id];
}
