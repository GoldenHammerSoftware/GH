// Copyright Golden Hammer Software
#include "GHUtils/GHTransitionSwitch.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHUtils/GHControllerMgr.h"

GHTransitionSwitch::GHTransitionSwitch(const GHPropertyContainer& props, const GHIdentifier& propId,
                                       GHControllerMgr& controllerMgr)
: mProps(props)
, mPropId(propId)
, mOldVal(-1)
, mPropertyMonitor(*this)
, mControllerMgr(controllerMgr)
, mDisplayingDefaultTransitions(false)
{
}

GHTransitionSwitch::~GHTransitionSwitch(void)
{
    for (size_t i = 0; i < mReplacements.size(); ++i)
    {
        delete mReplacements[i].second;
    }
    
    for (size_t i = 0; i < mDefaultTransitions.size(); ++i)
    {
        delete mDefaultTransitions[i];
    }
}

void GHTransitionSwitch::activate(void)
{
    updatePropertyVal();
    mControllerMgr.add(&mPropertyMonitor);
}

void GHTransitionSwitch::deactivate(void)
{
    mControllerMgr.remove(&mPropertyMonitor);
    deactivateTransitions(mOldVal);
    mOldVal = -1;
}

void GHTransitionSwitch::updatePropertyVal(void)
{
    const GHProperty& currVal = mProps.getProperty(mPropId);
    if (currVal != mOldVal) {
        deactivateTransitions(mOldVal);
        activateTransitions(currVal);
    }
}

void GHTransitionSwitch::addTransition(const GHProperty& prop, GHTransition* transition)
{
    mReplacements.push_back(PropTransitionPair(prop,transition));
}

void GHTransitionSwitch::addEmptyPropVal(const GHProperty& prop)
{
    mEmptyProps.push_back(prop);
}

void GHTransitionSwitch::addDefaultTransition(GHTransition* transition)
{
    mDefaultTransitions.push_back(transition);
}

void GHTransitionSwitch::deactivateTransitions(const GHProperty& propVal)
{
    if (mDisplayingDefaultTransitions)
    {
        for (size_t i = 0; i < mDefaultTransitions.size(); ++i)
        {
            mDefaultTransitions[i]->deactivate();
        }
        mDisplayingDefaultTransitions = false;
    }
    
    for (size_t i = 0; i < mReplacements.size(); ++i)
    {
        if (mReplacements[i].first == propVal) {
            mReplacements[i].second->deactivate();
        }
    }
}

void GHTransitionSwitch::activateTransitions(const GHProperty& propVal)
{
    std::vector<GHProperty>::iterator emptyFindIter = std::find(mEmptyProps.begin(), mEmptyProps.end(), propVal);
    mDisplayingDefaultTransitions = (emptyFindIter == mEmptyProps.end());
    for (size_t i = 0; i < mReplacements.size(); ++i)
    {
        if (mReplacements[i].first == propVal) {
            mDisplayingDefaultTransitions = false;
            mReplacements[i].second->activate();
        }
    }
    
    if (mDisplayingDefaultTransitions)
    {
        for (size_t i = 0; i < mDefaultTransitions.size(); ++i)
        {
            mDefaultTransitions[i]->activate();
        }
    }
    
    mOldVal = propVal;
}
