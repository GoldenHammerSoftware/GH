// Copyright Golden Hammer Software
#include "GHGUINavResponse.h"

class GHTransition;

class GHGUITransitionNavResponse : public GHGUINavResponse
{
public:
	GHGUITransitionNavResponse(GHTransition* transition);
	virtual ~GHGUITransitionNavResponse(void);

	virtual bool respondToNavigation(const GHPoint2i& dir);

private:
	GHTransition* mTransition;
};
