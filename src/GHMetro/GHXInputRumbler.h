// Copyright Golden Hammer Software
#include "GHRumbler.h"

class GHPropertyContainer;

class GHXInputRumbler : public GHRumbler
{
public:

	void setPropertyContainer(GHPropertyContainer* props);

	virtual void setRumble(int controllerIndex, float leftRumble, float rightRumble);

private:
	const GHPropertyContainer* mProps;
};
