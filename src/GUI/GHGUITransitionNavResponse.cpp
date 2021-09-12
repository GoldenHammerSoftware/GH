// Copyright Golden Hammer Software
#include "GHGUITransitionNavResponse.h"
#include "GHUtils/GHTransition.h"

GHGUITransitionNavResponse::GHGUITransitionNavResponse(GHTransition* transition)
	: mTransition(transition)
{

}

GHGUITransitionNavResponse::~GHGUITransitionNavResponse(void)
{
	delete mTransition;
}

bool GHGUITransitionNavResponse::respondToNavigation(const GHPoint2i& dir)
{
	mTransition->activate();
	mTransition->deactivate();
	return true;
}
