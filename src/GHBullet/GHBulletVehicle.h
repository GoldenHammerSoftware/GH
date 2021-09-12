// Copyright Golden Hammer Software
#include "GHPhysicsObject.h"
#include "GHUtils/GHController.h"
#include "GHBulletBtInclude.h"
#include "GHString/GHIdentifier.h"

class GHBulletPhysicsSim;
class GHControllerMgr;
class GHBulletPhysicsObject;
class btRaycastVehicle;

class GHBulletVehicle : public GHPhysicsObject
{
public:
    GHBulletVehicle(GHBulletPhysicsSim& physicsSim,
                    GHControllerMgr& controllerMgr,
                    GHBulletPhysicsObject* chassis,
                    const btRaycastVehicle::btVehicleTuning& tuning,
                    const GHIdentifier& inContactProp,
                    const GHIdentifier& disableProp,
                    const GHIdentifier& wheelPointProp,
                    const GHIdentifier& contactNormalProp,
                    float updatePriority);
    virtual ~GHBulletVehicle(void);
    
    virtual void activate(void);
    virtual void deactivate(void);
    
    virtual GHPhysicsObject* clone(GHTransformNode* skeleton);
    
    virtual void getLinearVelocity(GHPoint3& linearVelocity) const;
    virtual void getAngularVelocity(GHPoint3& angularVelocity) const;
    
    virtual void setLinearVelocity(const GHPoint3& linearVelocity);
    virtual void setAngularVelocity(const GHPoint3& angularVelocity);
    
	virtual float getMass(void) const;
	virtual void setFriction(float friction);
	virtual float getFriction(void) const;
    virtual void setRestitution(float restitution);
    virtual float getRestitution(void) const;
    virtual void getCenterOfMass(GHPoint3& outCoM) const;

	virtual void setAngularFactor(const GHPoint3& angularFactor);

    virtual void setCollisionLayer(short collisionLayer);
    
    //used by game to communicate to physics sim
    virtual void applyForce(const GHPoint3& force, const GHPoint3& position);
    virtual void applyImpulse(const GHPoint3& impulse, const GHPoint3& position);
    virtual void applyTorque(const GHPoint3& torque);
    virtual void applyAngularImpulse(const GHPoint3& impulse);
    virtual void resetMotion(void);
    
    bool addConstraintToSim(void);
    void removeConstraintFromSim(void);
    GHBulletPhysicsSim& getPhysicsSim(void) { return mPhysicsSim; }
    
private:
    btRaycastVehicle& createVehicle(GHBulletPhysicsSim& physicsSim, GHBulletPhysicsObject* chassis, const btRaycastVehicle::btVehicleTuning& tuning);
    
    class Updater : public GHController
    {
    public:
        Updater(btRaycastVehicle& vehicle,
                const GHIdentifier& inContactProp,
                const GHIdentifier& disableProp,
                const GHIdentifier& wheelPointProp,
                const GHIdentifier& contactNormalProp,
                GHBulletVehicle& parent);
        virtual ~Updater(void);
        virtual void onActivate(void);
        virtual void onDeactivate(void);
        virtual void update(void);
    private:
        void setInContactProp(void);
        void drawWheels(void);
        void drawWheel(int index);
    private:
        btRaycastVehicle& mVehicle;
        GHBulletVehicle& mParent;
        GHIdentifier mInContactProp;
        GHIdentifier mDisableProp;
        GHIdentifier mWheelPointProp;
        GHIdentifier mWheelPointNormalProp;
        GHPoint3 mCurrentContactPoint;
        GHPoint3 mCurrentContactNormal;
        std::vector<int> mDebugDraws;
    };
    Updater mUpdater;
    
private:
    GHBulletPhysicsSim& mPhysicsSim;
    GHControllerMgr& mControllerMgr;
    GHIdentifier mInContactProp;
    GHIdentifier mDisableProp;
    GHIdentifier mWheelPointProp;
    GHIdentifier mWheelPointNormalProp;
    GHBulletPhysicsObject* mChassis;
    btRaycastVehicle::btVehicleTuning mTuning;
    btRaycastVehicle* mBtVehicle;
    float mUpdatePriority;
    bool mConstraintIsInSim;
};
