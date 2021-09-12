// Copyright Golden Hammer Software
#pragma once

#include "GHHFUVType.h"

class GHHeightField;

// makes a set of uv coordinates for a heightfield.
class GHHeightFieldUVMaker
{
public:
	GHHeightFieldUVMaker(const GHHeightField& hf, GHHFUVType::Enum uvType);
	~GHHeightFieldUVMaker(void);

	const float* getUV(void) const { return mUV; }

private:
	// create uv coords that go from 0 to 1 with a top down projection.
	void createProjectedUV(const GHHeightField& hf);
	// create a uv set that tries to de-stretch the cliffs.
	void createWrappedUV(const GHHeightField& hf);

private:
	float* mUV;
};
