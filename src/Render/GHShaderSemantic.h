#pragma once

#include "GHVertexComponentShaderID.h"
#include <assert.h>

namespace GHShaderSemantic
{
	// Get the expected shader semantic string for a vertex component.
	static const char* getSemanticName(GHVertexComponentShaderID::Enum shaderId, int& semanticIdx)
	{
		semanticIdx = 0;
		if (shaderId == GHVertexComponentShaderID::SI_POS) return "POSITION";
		if (shaderId == GHVertexComponentShaderID::SI_NORMAL) return "NORMAL";
		if (shaderId == GHVertexComponentShaderID::SI_TANGENT) return "TANGENT";
		if (shaderId == GHVertexComponentShaderID::SI_DIFFUSE) return "DIFFUSE";
		if (shaderId == GHVertexComponentShaderID::SI_SPECULAR) return "SPECULAR";
		if (shaderId == GHVertexComponentShaderID::SI_UV1) return "UV";
		if (shaderId == GHVertexComponentShaderID::SI_BONEIDX) return "BONEIDX";
		if (shaderId == GHVertexComponentShaderID::SI_BONEWEIGHT) return "BONEWEIGHT";

		if (shaderId == GHVertexComponentShaderID::SI_UV2) {
			semanticIdx = 0; // was 1 with "UV"
			return "UVTWO";
		}

		assert(false);
		return "UNKNOWN";
	}
};