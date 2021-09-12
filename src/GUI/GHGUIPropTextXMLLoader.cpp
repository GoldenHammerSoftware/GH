// Copyright Golden Hammer Software
#include "GHGUIPropTextXMLLoader.h"
#include "GHGUITextXMLLoader.h"
#include "GHPropertyStringGeneratorXMLLoader.h"
#include "GHGUIText.h"
#include "GHUtils/GHPropertyStringGenerator.h"
#include "GHUtils/GHControllerTransition.h"
#include "GHGUIPropTextUpdater.h"
#include "GHWidgetStates.h"

GHGUIPropTextXMLLoader::GHGUIPropTextXMLLoader(const GHGUITextXMLLoader& textLoader,
                                               const GHPropertyStringGeneratorXMLLoader& genLoader,
                                               GHControllerMgr& controllerMgr)
: mTextLoader(textLoader)
, mGeneratorLoader(genLoader)
, mControllerMgr(controllerMgr)
{
}

void* GHGUIPropTextXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHGUIWidgetResource* textResource = (GHGUIWidgetResource*)mTextLoader.create(node, extraData);
    if (!textResource) return 0;
    GHGUIText* text = (GHGUIText*)textResource->get();
    if (!text) return 0;

    GHPropertyStringGenerator* psg = (GHPropertyStringGenerator*)mGeneratorLoader.create(node, extraData);
    if (psg)
    {
        GHGUIPropTextUpdater* textUpdater = new GHGUIPropTextUpdater(*text, psg);
        GHControllerTransition* psgTrans = new GHControllerTransition(mControllerMgr, textUpdater);
        text->getWidgetStates().addTransition(GHWidgetState::WS_ACTIVE, psgTrans);
		text->getWidgetStates().addTransition(GHWidgetState::WS_TRANSITIONIN, psgTrans);
		text->getWidgetStates().addTransition(GHWidgetState::WS_TRANSITIONOUT, psgTrans);
    }

    return textResource;
}

