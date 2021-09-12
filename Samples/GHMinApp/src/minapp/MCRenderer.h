// Copyright 2010 Golden Hammer Software
#pragma once

#include "Render/GHCamera.h"

class GHSceneRenderer;
class GHAppShard;
class GHTimeVal;
class GHRenderServices;
class GHSystemServices;

// A place to initialize the rendering
class MCRenderer
{
public:
	MCRenderer(GHSceneRenderer& sceneRenderer, GHAppShard& appShard,
               const GHTimeVal& realTimeVal, GHRenderServices& renderServices,
               GHSystemServices& systemServices);
	~MCRenderer(void);

    GHCamera& getCamera(void) { return mCamera; }
    GHSceneRenderer& getSceneRenderer(void) { return mSceneRenderer; }

private:
    GHSceneRenderer& mSceneRenderer;
    GHCamera mCamera;
};
