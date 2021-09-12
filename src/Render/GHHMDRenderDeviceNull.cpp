#include "GHHMDRenderDeviceNull.h"
#include "GHViewInfo.h"

void GHHMDRenderDeviceNull::createStereoView(const GHViewInfo& mainView, const GHCamera& camera,
	GHViewInfo& leftView, GHViewInfo& rightView, GHFrustum& combinedFrustum) const
{
	leftView = mainView;
	rightView = mainView;
	GHPoint4 clip = mainView.getViewportClip();
	float clipWidth = clip[2] - clip[0];

	GHPoint4 leftClip = clip;
	leftClip[2] = leftClip[0] + clipWidth / 2.0f;
	leftView.setViewportClip(leftClip);

	GHPoint4 rightClip = clip;
	rightClip[0] = clip[0] + clipWidth / 2.0f;
	rightClip[2] = rightClip[0] + clipWidth / 2.0f;
	rightView.setViewportClip(rightClip);

	// not correct but whatever.  if we start using renderdevicenull for anything might want to revisit.
	combinedFrustum = mainView.getFrustum();
}
