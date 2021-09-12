// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHIdentifier.h"
#include <vector>

// A class to manage which passes something should draw in.
// if we are empty, we only support main pass.
// otherwise we only support specified passes.
class GHRenderPassMembership
{
public:
    GHRenderPassMembership(void);
    
	// query if this should draw in a given pass id.
    bool supportsPass(const GHIdentifier& passId) const;

    // explicitly add a supported pass.
	void addSupportedPass(const GHIdentifier& passId);
	// explicitly add an unsupported pass.
	void addExcludedPass(const GHIdentifier& passId);
    
    void setEmptySupportsAll(bool val) { mEmptySupportsAll = val; }
    
private:
    std::vector<GHIdentifier> mSupportedPasses;
    // a flag to say whether an empty supported passes list means support all, or just support main.
    bool mEmptySupportsAll{ false };
	std::vector<GHIdentifier> mExcludedPasses;
};
