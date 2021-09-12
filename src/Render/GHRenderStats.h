// Copyright Golden Hammer Software
#pragma once

class GHPropertyContainer;

// debug class for tracking render overhead
class GHRenderStats
{
public:
	static void beginFrame(void);
	static void recordDraw(int numVerts);
	static void recordMaterialChange(void);
	static void endFrame(GHPropertyContainer& props);

private:
	static int mNumVerts;
	static int mNumDraws;
	// number of material changes in a frame.
	static int mNumMats;
};
