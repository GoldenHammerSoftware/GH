// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include "GHString/GHIdentifier.h"
#include "GHUtils/GHProperty.h"
#include "GHUtils/GHController.h"
#include <vector>

class GHControllerMgr;
class GHPropertyContainer;

// A transition that manages the activate/deactivate of other transitions
// based on a property value.
class GHTransitionSwitch : public GHTransition
{
private:
    class PropertyMonitor : public GHController
    {
    public:
        PropertyMonitor(GHTransitionSwitch& parent) : mParent(parent) {}
        
        virtual void update(void) { mParent.updatePropertyVal(); }
        virtual void onActivate(void) {}
        virtual void onDeactivate(void) {}
        
    private:
        GHTransitionSwitch& mParent;
    };
    
public:
    GHTransitionSwitch(const GHPropertyContainer& props, const GHIdentifier& propId,
                       GHControllerMgr& controllerMgr);
    ~GHTransitionSwitch(void);
    
    virtual void activate(void);
	virtual void deactivate(void);
    
    void addEmptyPropVal(const GHProperty& prop);
    void addTransition(const GHProperty& prop, GHTransition* transition);
    void addDefaultTransition(GHTransition* transition);
    
private:
    void deactivateTransitions(const GHProperty& propVal);
    void activateTransitions(const GHProperty& propVal);
    void updatePropertyVal(void);
    
private:
    const GHPropertyContainer& mProps;
    GHIdentifier mPropId;
    GHProperty mOldVal;
    PropertyMonitor mPropertyMonitor;
    GHControllerMgr& mControllerMgr;
    
    std::vector<GHProperty> mEmptyProps;
    std::vector<GHTransition*> mDefaultTransitions;
    bool mDisplayingDefaultTransitions;
    
    typedef std::pair<GHProperty, GHTransition*> PropTransitionPair;
    typedef std::vector<PropTransitionPair> ReplacementSet;
    ReplacementSet mReplacements;
};
