// Copyright Golden Hammer Software
#pragma once

#include "GHRenderTargetFactory.h"

class GHRenderDeviceDX11;
class GHEventMgr;

class GHRenderTargetFactoryDX11 : public GHRenderTargetFactory
{
public:
	GHRenderTargetFactoryDX11(GHRenderDeviceDX11& device, GHEventMgr& eventMgr);

	virtual GHRenderTarget* createRenderTarget(const GHRenderTarget::Config& args) const;
	virtual void resize(GHRenderTarget& target, const GHRenderTarget::Config& args) const;

private:
	GHRenderDeviceDX11& mDevice;
	GHEventMgr& mEventMgr;
};
