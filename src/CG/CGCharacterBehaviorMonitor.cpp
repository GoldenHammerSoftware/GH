// Copyright 2010 Golden Hammer Software
#include "CGCharacterBehaviorMonitor.h"
#include "GHEntity.h"
#include "CGProperties.h"
#include "GHModel.h"
#include "GHFloat.h"
#include "GHTimeVal.h"

CGCharacterBehaviorMonitor::CGCharacterBehaviorMonitor(const GHTimeVal& timeVal,
                                                       GHEntity& entity,
                                                       GHModel& model)
: mTime(timeVal)
, mEntity(entity)
, mModel(model)
, mAttackAnimTime(1.f)
, mTimeSpentAttacking(0)
{
    
}

void CGCharacterBehaviorMonitor::update(void)
{
    checkAttack();
    checkReachedDestination();
}

void CGCharacterBehaviorMonitor::checkReachedDestination(void)
{
    if (mEntity.mProperties.getProperty(CGProperties::PP_HASDESTINATION))
    {
        GHPoint3* dest = mEntity.mProperties.getProperty(CGProperties::PP_DESTINATION);
        if (!dest)
        {
            mEntity.mProperties.setProperty(CGProperties::PP_HASDESTINATION, false);
            return;
        }
        
        GHPoint3 currentPos;
        mModel.getSkeleton()->getTransform().getTranslate(currentPos);
        currentPos[1] = (*dest)[1];
        
        if (GHFloat::isEqual(currentPos, *dest, .5f))
        {
            if ((GHEntity*)mEntity.mProperties.getProperty(CGProperties::PP_ATTACKTARGET) != 0)
            {
                mEntity.mProperties.setProperty(CGProperties::PP_ISATTACKING, true);
                mTimeSpentAttacking = 0;
            }
            
            mEntity.mProperties.setProperty(CGProperties::PP_HASDESTINATION, false);
        }
    }
}

void CGCharacterBehaviorMonitor::checkAttack(void)
{
    mTimeSpentAttacking += mTime.getTimePassed();
    if (mTimeSpentAttacking > mAttackAnimTime)
    {
        bool isAttacking = mEntity.mProperties.getProperty(CGProperties::PP_ISATTACKING);
        if (isAttacking)
        {
            GHEntity* enemy = mEntity.mProperties.getProperty(CGProperties::PP_ATTACKTARGET);
            if (enemy)
            {
                int enemyHP = enemy->mProperties.getProperty(CGProperties::PP_HITPOINTS);
                int ourDamage = mEntity.mProperties.getProperty(CGProperties::PP_ATTACKDAMAGE);
                enemyHP = std::max(0, enemyHP - ourDamage);
                enemy->mProperties.setProperty(CGProperties::PP_HITPOINTS, enemyHP);   
            }
            mTimeSpentAttacking = 0;
            mEntity.mProperties.setProperty(CGProperties::PP_ISATTACKING, false);
            mEntity.mProperties.setProperty(CGProperties::PP_ATTACKTARGET, 0);
        }        
    }
}

void CGCharacterBehaviorMonitor::onActivate(void)
{
    mTimeSpentAttacking = 0;
}

void CGCharacterBehaviorMonitor::onDeactivate(void)
{
    
}