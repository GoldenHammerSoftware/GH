#pragma once

#include "GHUtils/GHController.h"
#include "GHOpenVRSkeletalAvatar.h"
#include <vector>

class GHOpenVRAvatarDebugDraw : public GHController
{
public:
	GHOpenVRAvatarDebugDraw(void);
	~GHOpenVRAvatarDebugDraw(void);

	typedef GHOpenVRSkeletalAvatar::SkeletalData SkeletalData;

	void addSkeleton(const SkeletalData* skeleton);

	virtual void onActivate(void);
	virtual void onDeactivate(void);
	virtual void update(void);

private:
	void updateSkeleton(const SkeletalData& skeleton, const int* debugDrawIds) const;
	void drawModelSkeleton(GHTransformNode* transformNode, int*& debugDrawHandle) const;
	void drawModelSkeleton(const SkeletalData& skeleton, const int* debugDrawIds) const;

private:
	std::vector<const SkeletalData*> mSkeletons;
	std::vector<int*> mDebugDrawIds;
	std::vector<int*> mModelDebugDrawIds;
};
