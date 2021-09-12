// Copyright 2010 Golden Hammer Software
#include "MCRenderer.h"
#include "Base/GHAppShard.h"
#include "Render/GHSceneRenderer.h"
#include "Render/GHRenderServices.h"
#include "Render/GHRenderPass.h"
#include "Render/GHMaterialCallbackFactoryViewInfo.h"
#include "Render/GHMaterialCallbackMgr.h"
#include "Render/GHMaterialCallbackFactoryTime.h"
#include "Render/GHDebugDraw.h"
#include "Render/GHLightMgr.h"
#include "MCUpdatePriority.h"
#include "MCIdentifiers.h"
#include "GHPlatform/GHDeletionHandle.h"
#include "Base/GHSystemServices.h"

#include "Render/GHRenderTargetFactory.h"
#include "Render/GHRenderTargetMaterialCallbackFactory.h"
#include "Render/GHShadowCameraPositioner.h"
#include "Render/GHRenderTarget.h"
#include "Game/GHPositionFollowerEntXMLLoader.h"
#include "GUI/GHGUIQuadRenderer.h"

MCRenderer::MCRenderer(GHSceneRenderer& sceneRenderer, GHAppShard& appShard,
                       const GHTimeVal& realTimeVal, GHRenderServices& renderServices,
                       GHSystemServices& systemServices)
: mSceneRenderer(sceneRenderer)
{
    appShard.mControllerMgr.add(&mSceneRenderer, MCUpdatePriority::UP_RENDER);
    
    mCamera.setNearClip(0.3f);
    mCamera.setFarClip(20000.0f);
    mCamera.setFOV(60);
    GHPoint4 clearColor(0,0.2,0,0);
    
	GHPoint3 lightDir(0.2f, -1.0f, -0.5f);
	lightDir.normalize();
    renderServices.getLightMgr().setLightDir(0, lightDir);
    
    GHRenderPass* renderPass = new GHRenderPass(mCamera, renderServices.getScreenInfo(), 0,
                                                *renderServices.getDevice(), true,
                                                &clearColor, GHRenderPass::sMainPassId, false);
    mSceneRenderer.addRenderPass(renderPass);
    
    GHMaterialCallbackFactory* viewCB = new GHMaterialCallbackFactoryViewInfo(renderPass->getViewInfo(GHViewInfo::EYE_MONOCLE), "Main");
    renderServices.getMaterialCallbackMgr()->addFactory(viewCB);
    
    GHMaterialCallbackFactory* timeCB = new GHMaterialCallbackFactoryTime(realTimeVal, "Time");
    renderServices.getMaterialCallbackMgr()->addFactory(timeCB);
    
    GHMaterial* debugMat = (GHMaterial*)appShard.mXMLObjFactory.load("debugmat.xml");
    GHDebugDraw* debugDraw = new GHDebugDraw(*renderServices.getVBFactory(), debugMat);
    debugDraw->setDrawOrder(10);
    GHDebugDraw::setSingleton(debugDraw);
    mSceneRenderer.getRenderGroup().addRenderable(GHDebugDraw::getSingleton(), 0);
}

MCRenderer::~MCRenderer(void)
{
	GHDebugDraw::setSingleton(0);
}
