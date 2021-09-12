#pragma once

#include "GHMath/GHPoint.h"
#include "GHMDesc.h"
#include "GHInputStructs.h" // for pointer.  

class GHViewInfo;
class GHTransform;
class GHPropertyContainer;
class GHScreenInfo;

// defines a drawing location for a gui
class IGHGUICanvas
{
public:
	virtual ~IGHGUICanvas(void) {}

	virtual bool is2d(void) const = 0;
	virtual bool isInteractable(void) const = 0;

	virtual void createGuiToWorld(const GHViewInfo& viewInfo, GHTransform& outTrans) const = 0;

	virtual void clearPointerOverrides(void) = 0;
	virtual void addPointerOverride(const GHPoint2& pos, unsigned int pointerIndex) = 0;
	virtual const GHInputStructs::PointerList& getPointerOverrides(void) = 0;

	virtual void setPos(const GHPoint3& pos) = 0;
	virtual void setRot(const GHPoint3& rot) = 0;
	virtual void setScale(const GHPoint3& scale) = 0;

	virtual const GHPoint3& getPos(void) const = 0;
	virtual const GHPoint3& getRot(void) const = 0;
	virtual const GHPoint3& getScale(void) const = 0;

	virtual const GHScreenInfo* getScreenInfo(void) const = 0;

	// helper function to look in a prop map and find a canvas marked as is3d.
	static IGHGUICanvas* get3dCanvas(const GHPropertyContainer& props);
};

// default concrete IGHGUICanvas
class GHGUICanvas : public IGHGUICanvas
{
public:
	GHGUICanvas(bool is2d, bool isInteractable, const GHPoint3& pos, const GHPoint3& rot, 
				const GHPoint3& scale, GHMDesc::BillboardType bt, GHScreenInfo* screenInfo);
	~GHGUICanvas(void);
	
	bool is2d(void) const override { return mIs2d; }
	bool isInteractable(void) const override { return mInteractable; }

	void createGuiToWorld(const GHViewInfo& viewInfo, 
						  GHTransform& outTrans) const override;

	void clearPointerOverrides(void) override;
	void addPointerOverride(const GHPoint2& pos, unsigned int pointerIndex) override;
	const GHInputStructs::PointerList& getPointerOverrides(void) override;

	void setPos(const GHPoint3& pos) override;
	void setRot(const GHPoint3& rot) override;
	void setScale(const GHPoint3& scale) override;

	const GHPoint3& getPos(void) const override { return mPos; }
	const GHPoint3& getRot(void) const override { return mRot; }
	const GHPoint3& getScale(void) const override { return mScale; }

	const GHScreenInfo* getScreenInfo(void) const override { return mScreenInfo; }

protected:
	bool mIs2d{ true }; // projected to screen or not.
	bool mInteractable{ true }; // can be pointed at and clicked on.

	GHPoint3 mPos;
	GHPoint3 mRot;
	GHPoint3 mScale;
	GHMDesc::BillboardType mBillboardType;
	GHScreenInfo* mScreenInfo{ nullptr };

	// list of collision pointers for this canvas this frame.
	GHInputStructs::PointerList mPointerOverrides;
};
