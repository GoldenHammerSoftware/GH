// Copyright Golden Hammer Software
#include "GHRenderStats.h"
#include "GHRenderProperties.h"
#include "GHUtils/GHPropertyContainer.h"

int GHRenderStats::mNumVerts = 0;
int GHRenderStats::mNumDraws = 0;
int GHRenderStats::mNumMats = 0;

void GHRenderStats::beginFrame(void)
{
	mNumVerts = 0;
	mNumDraws = 0;
	mNumMats = 0;
}

void GHRenderStats::recordDraw(int numVerts)
{
	mNumDraws++;
	mNumVerts += numVerts;
}

void GHRenderStats::recordMaterialChange(void)
{
	mNumMats++;
}

void GHRenderStats::endFrame(GHPropertyContainer& props)
{
	props.setProperty(GHRenderProperties::GP_NUMDRAWS, mNumDraws);
	props.setProperty(GHRenderProperties::GP_NUMVERTS, mNumVerts);
	props.setProperty(GHRenderProperties::GP_NUMMATS, mNumMats);
}
