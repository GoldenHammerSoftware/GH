// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include "GHString/GHIdentifier.h"
#include "GHScreenPosUnprojector.h"

class GHPropertyContainer;
class GHGUIPanel;

//Controller that updates a GUI panel's position to render on top of a 3D point in a property map
class GHGUI3DPositionFollower : public GHController
{
public:
	GHGUI3DPositionFollower(const GHPropertyContainer& props,
							const GHIdentifier& pointIndex,
							const GHViewInfo& viewInfo,
							GHGUIPanel& targetPanel);

	virtual void onActivate(void);
	virtual void onDeactivate(void);
	virtual void update(void);

private:
	const GHPropertyContainer& mProps;
	GHIdentifier mPointIndex;
	GHScreenPosUnprojector mUnprojector;
	GHGUIPanel& mTargetPanel;
};
