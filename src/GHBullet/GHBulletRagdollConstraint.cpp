// Copyright Golden Hammer Software
#include "GHBulletRagdollConstraint.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHBulletBtInclude.h"

GHBulletRagdollConstraint::GHBulletRagdollConstraint(btTypedConstraint* constraint, int parentIndex, int childIndex, bool bodyCollisionDisabled)
: mConstraint(constraint)
, mParentIndex(parentIndex)
, mChildIndex(childIndex)
, mBodyCollisionDisabled(bodyCollisionDisabled)
{
    
}

GHBulletRagdollConstraint* GHBulletRagdollConstraint::clone(btRigidBody* parent, btRigidBody* child) const
{
    btTypedConstraint* clonedConstraint = 0;
    switch(mConstraint->getConstraintType())
    {
        case HINGE_CONSTRAINT_TYPE:
            clonedConstraint = cloneHinge(parent, child);
            break;
        case POINT2POINT_CONSTRAINT_TYPE:
            clonedConstraint = clonePoint2Point(parent, child);
            break;
        case CONETWIST_CONSTRAINT_TYPE:
            clonedConstraint = cloneConeTwist(parent, child);
            break;
        case D6_CONSTRAINT_TYPE:
            clonedConstraint = clone6DOF(parent, child);
            break;
        default:
            GHDebugMessage::outputString("Error cloning constraint. Clone method for the constraint type hasn't been implemented. No constraint will be created.");
            break;
    }

    if (!clonedConstraint) {
        return 0;
    }
        
    return new GHBulletRagdollConstraint(clonedConstraint, mParentIndex, mChildIndex, mBodyCollisionDisabled);
}

btTypedConstraint* GHBulletRagdollConstraint::cloneHinge(btRigidBody* parent, btRigidBody* child) const
{
    const btHingeConstraint* hingeConstraint = reinterpret_cast<const btHingeConstraint*>(mConstraint);
    btHingeConstraint* ret = new btHingeConstraint(*parent, *child, hingeConstraint->getAFrame(), hingeConstraint->getBFrame());

    ret->setLimit(hingeConstraint->getLowerLimit(), hingeConstraint->getUpperLimit());
    
    return ret;
}

btTypedConstraint* GHBulletRagdollConstraint::clonePoint2Point(btRigidBody* parent, btRigidBody* child) const
{
    const btPoint2PointConstraint* point2pointConstraint = reinterpret_cast<const btPoint2PointConstraint*>(mConstraint);
    btPoint2PointConstraint* ret = new btPoint2PointConstraint(*parent, *child, point2pointConstraint->getPivotInA(), point2pointConstraint->getPivotInB());
    return ret;
}

btTypedConstraint* GHBulletRagdollConstraint::cloneConeTwist(btRigidBody* parent, btRigidBody* child) const
{
    const btConeTwistConstraint* coneTwistConstraint = reinterpret_cast<const btConeTwistConstraint*>(mConstraint);
    btConeTwistConstraint* ret = new btConeTwistConstraint(*parent, *child, coneTwistConstraint->getFrameOffsetA(), coneTwistConstraint->getFrameOffsetB());
    
    //const cast because these getter methods should be const int the API
    btConeTwistConstraint* ncConstraint = const_cast<btConeTwistConstraint*>(coneTwistConstraint);
    
    ret->setLimit(ncConstraint->getSwingSpan1(), ncConstraint->getSwingSpan2(), ncConstraint->getTwistSpan());
    
    return ret;
}

btTypedConstraint* GHBulletRagdollConstraint::clone6DOF(btRigidBody* parent, btRigidBody* child) const
{
    const btGeneric6DofConstraint* oldConstraint = reinterpret_cast<const btGeneric6DofConstraint*>(mConstraint);
    btGeneric6DofConstraint* ret = new btGeneric6DofConstraint(*parent, *child, oldConstraint->getFrameOffsetA(), oldConstraint->getFrameOffsetB(), true);
    
    //shame on Bullet for not being const-correct
    btGeneric6DofConstraint* pretendConstConstraint = const_cast<btGeneric6DofConstraint*>(oldConstraint);
    
    btVector3 vec;
    pretendConstConstraint->getLinearLowerLimit(vec);
    ret->setLinearLowerLimit(vec);
    
    pretendConstConstraint->getLinearUpperLimit(vec);
    ret->setLinearUpperLimit(vec);
    
    pretendConstConstraint->getAngularLowerLimit(vec);
    ret->setAngularLowerLimit(vec);
    
    pretendConstConstraint->getAngularUpperLimit(vec);
    ret->setAngularUpperLimit(vec);
    
    return ret;
}
