// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"

class GHGUIText;
class GHPropertyStringGenerator;

// a class to apply the results of a prop string generator to a gui text.
class GHGUIPropTextUpdater : public GHController
{
public:
    // we take ownership of the generator.
    GHGUIPropTextUpdater(GHGUIText& text, GHPropertyStringGenerator* generator);
    ~GHGUIPropTextUpdater(void);
    
    virtual void update(void);
    // we call update on activate or else we get a single bad frame.
	virtual void onActivate(void) { update(); }
	virtual void onDeactivate(void) {}
    
protected:
    GHGUIText& mText;
    GHPropertyStringGenerator* mGenerator;
};
