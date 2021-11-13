#pragma once

#include "dxgiformat.h"
#include "Render/GHTextureFormat.h"

namespace GHDXGIUtil
{
	DXGI_FORMAT convertGHFormatToDXGI(GHTextureFormat::Enum ghFormat)
	{
		if (ghFormat == GHTextureFormat::TF_DXT1)
		{
			return DXGI_FORMAT_BC1_UNORM;
		}
		if (ghFormat == GHTextureFormat::TF_DXT5)
		{
			return DXGI_FORMAT_BC3_UNORM;
		}
		if (ghFormat == GHTextureFormat::TF_RGB8)
		{
			// there is no 24 bit d3d texture format.
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		}
		if (ghFormat == GHTextureFormat::TF_RGBA8)
		{
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		}
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	}
};
