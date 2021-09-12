#pragma once

#include "GHUtils/GHController.h"

// Point an indicator entity in the world at a 3d pointer position.
class GHPointerWorldIndicator : public GHController
{
public:
	GHPointerWorldIndicator(void);
	~GHPointerWorldIndicator(void);

	virtual void update(void);
	virtual void onActivate(void) {}
	virtual void onDeactivate(void) {}
};
