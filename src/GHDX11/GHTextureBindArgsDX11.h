// Copyright Golden Hammer Software
#pragma once

// arguments that are sent to binding a GHTextureDX11
struct GHTextureBindArgsDX11
{
	// are we binding to a vertex or a pixel shader?
	bool mIsVertex;
	// which texture register are we binding to?
	unsigned int mRegister;
};