// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include "GHString/GHString.h"
#include "GHTextureLoaderHFF.h"

class GHFileOpener;
class GHHeightField;
class GHFile;

// a transition that writes a heightfield out as an hff file
class GHHeightFieldHFFSaveModifier : public GHTransition
{
public:
	GHHeightFieldHFFSaveModifier(const GHHeightField& hf, 
		const GHFileOpener& fileOpener, const char* fileName);

	virtual void activate(void);
	virtual void deactivate(void);

private:
	void writeHeader(GHTextureLoaderHFF::HFFHeader& header, GHFile& file);

private:
	const GHHeightField& mHF;
	const GHFileOpener& mFileOpener;
	GHString mFileName;
};
