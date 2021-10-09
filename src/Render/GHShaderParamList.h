#pragma once

#pragma once

#include "GHString/GHString.h"
#include "GHMaterialCallbackType.h"
#include "GHMaterialParamHandle.h"
#include <vector>

// Optional struct for loading shader params info from xml when reflection is not available.
// A struct to say what params a shader supports, and where those live.
class GHShaderParamList
{
public:
	struct Param
	{
		// an arbitrary name for the texture slot
		// this is the handle the game uses to find params.
		GHString mSemantic;
		// which register the arg fits into. (texture only)
		unsigned int mRegister;
		// when the param is sent to the shader.
		GHMaterialCallbackType::Enum mCBType;
		// the type of the handle, also defines its size.
		GHMaterialParamHandle::HandleType mHandleType;
		//the number of objects to be passed at once. Defaults to 1
		unsigned int mCount{ 1 };
		// offset of the value in the buffer in bytes (non-texture only).
		unsigned int mOffsetBytes;
	};

public:
	GHShaderParamList(void);

	// calculate the size of the buffer and where items live based on 4 byte register boundaries.
	void calcParamOffsets(void);
	// calculate the size of the cbuffer needed for a callback type.
	unsigned int getBufferSize(GHMaterialCallbackType::Enum type) const;
	// find the param desc for a given semantic if it exists.
	const Param* getParam(const GHString& semantic) const;
	const std::vector<Param>& getParams(void) const { return mParams; }

	// insert a new value into the list.  used for loading.
	void addParam(const Param& param);

private:
	std::vector<Param> mParams;
	unsigned int mBufferSizes[GHMaterialCallbackType::CT_MAX];
};
