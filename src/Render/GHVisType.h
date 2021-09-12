// Copyright Golden Hammer Software
#pragma once

namespace GHVisType 
{
	enum VisType
	{
		// offscreen.
		VT_CULL = 0,
		// partially within view.
		VT_PARTIAL,
		// fully within view.
		VT_FULL
	};
};
