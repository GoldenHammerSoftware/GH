// Copyright Golden Hammer Software
#include "GHRenderPassMembership.h"
#include "GHRenderPass.h"
#include <stddef.h>

GHRenderPassMembership::GHRenderPassMembership(void)
{
}

bool GHRenderPassMembership::supportsPass(const GHIdentifier& passId) const
{
	for (size_t i = 0; i < mExcludedPasses.size(); ++i)
	{
		if (mExcludedPasses[i] == passId) return false;
	}

    if (!mSupportedPasses.size())
    {
        if (!mEmptySupportsAll)
        {
            // if no passes specified, only support main pass.
            if (passId == GHRenderPass::sMainPassId) return true;
            return false;
        }
        else
        {
            return true;
        }
    }

    for (size_t i = 0; i < mSupportedPasses.size(); ++i)
    {
        if (mSupportedPasses[i] == passId) return true;
    }
    return false;
}

void GHRenderPassMembership::addSupportedPass(const GHIdentifier& passId)
{
    mSupportedPasses.push_back(passId);
}

void GHRenderPassMembership::addExcludedPass(const GHIdentifier& passId)
{
	mExcludedPasses.push_back(passId);
}
