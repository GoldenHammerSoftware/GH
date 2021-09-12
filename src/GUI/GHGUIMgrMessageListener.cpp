// Copyright Golden Hammer Software
#include "GHGUIMgrMessageListener.h"
#include "GHUtils/GHEventMgr.h"
#include "GHGUIMgr.h"
#include "GHMessageTypes.h"
#include "GHUtils/GHMessage.h"
#include "GHGUIProperties.h"
#include "GHUtils/GHResourceFactory.h"
#include "GHString/GHStringIdFactory.h"
#include "GHRenderProperties.h"

GHGUIMgrMessageListener::GHGUIMgrMessageListener(GHGUIMgr& guiMgr, GHEventMgr& eventMgr,
                                                 GHResourceFactory& resourceFactory,
                                                 GHStringIdFactory& hashTable)
: mGUIMgr(guiMgr)
, mEventMgr(eventMgr)
, mResourceFactory(resourceFactory)
, mIdFactory(hashTable)
{
    mEventMgr.registerListener(GHMessageTypes::WINDOWRESIZE, *this);
    mEventMgr.registerListener(GHGUIProperties::GUIPOP, *this);
    mEventMgr.registerListener(GHGUIProperties::GUIPUSH, *this);
    mEventMgr.registerListener(GHGUIProperties::GUIBACK, *this);
    mEventMgr.registerListener(GHGUIProperties::M_TOGGLEGUIDRAW, *this);
    mEventMgr.registerListener(GHRenderProperties::M_HMDACTIVATED, *this);
    mEventMgr.registerListener(GHRenderProperties::M_HMDDEACTIVATED, *this);
}

GHGUIMgrMessageListener::~GHGUIMgrMessageListener(void)
{
    mEventMgr.unregisterListener(GHGUIProperties::GUIPUSH, *this);
    mEventMgr.unregisterListener(GHGUIProperties::GUIPOP, *this);
    mEventMgr.unregisterListener(GHMessageTypes::WINDOWRESIZE, *this);
    mEventMgr.unregisterListener(GHGUIProperties::GUIBACK, *this);
    mEventMgr.unregisterListener(GHGUIProperties::M_TOGGLEGUIDRAW, *this);
    mEventMgr.unregisterListener(GHRenderProperties::M_HMDACTIVATED, *this);
    mEventMgr.unregisterListener(GHRenderProperties::M_HMDDEACTIVATED, *this);
}

void GHGUIMgrMessageListener::handleMessage(const GHMessage& message)
{
    if (message.getType() == GHMessageTypes::WINDOWRESIZE)
    {
        mGUIMgr.updateScreenSize();
    }
    else if (message.getType() == GHGUIProperties::GUIPUSH)
    {
        GHGUIWidgetResource* widget = getWidgetResource(message);
        if (widget) {
			float priority = message.getPayload().getProperty(GHGUIProperties::GP_PRIORITY);
			size_t category = message.getPayload().getProperty(GHGUIProperties::GP_GUICATEGORY);
            GHGUITransitionDesc* tDesc = (GHGUITransitionDesc*)message.getPayload().getProperty(GHGUIProperties::GP_TRANSITIONDESC);
            if (!tDesc) mGUIMgr.pushWidget(widget, GHGUITransitionDesc(), priority, category);
            else mGUIMgr.pushWidget(widget, *tDesc, priority, category);
        }
    }
    else if (message.getType() == GHGUIProperties::GUIPOP)
    {
        handleGUIPopMessage(message);
    }
    else if (message.getType() == GHGUIProperties::GUIBACK)
    {
        mGUIMgr.pushHistory();
    }
    else if (message.getType() == GHGUIProperties::M_TOGGLEGUIDRAW)
    {
        mGUIMgr.getRenderable().toggleEnabled();
    }
    else if (message.getType() == GHRenderProperties::M_HMDACTIVATED
            || message.getType() == GHRenderProperties::M_HMDDEACTIVATED)
    {
        mGUIMgr.updateScreenSize();
    }
}

GHGUIWidgetResource* GHGUIMgrMessageListener::getWidgetResource(const GHMessage& message)
{
    GHGUIWidgetResource* ret = (GHGUIWidgetResource*)message.getPayload().getProperty(GHGUIProperties::GP_WIDGETRESOURCEPTR);
    if (!ret)
    {
        const char* filename = message.getPayload().getProperty(GHGUIProperties::GP_WIDGETFILENAME);
        if (filename)
        {
            ret = (GHGUIWidgetResource*)mResourceFactory.getCacheResource(filename);
        }
    }
    return ret;
}

void GHGUIMgrMessageListener::handleGUIPopMessage(const GHMessage& message)
{
    bool addToStack = true;
    const GHProperty& addToStackProp = message.getPayload().getProperty(GHGUIProperties::GP_ADDTOSTACK);
    if (addToStackProp.isValid()) addToStack = addToStackProp;
    
	size_t category = message.getPayload().getProperty(GHGUIProperties::GP_GUICATEGORY);

    GHGUITransitionDesc* tDesc = (GHGUITransitionDesc*)message.getPayload().getProperty(GHGUIProperties::GP_TRANSITIONDESC);
    bool deleteTDesc = false;
    if (!tDesc) {
        tDesc = new GHGUITransitionDesc;
        deleteTDesc = true;
    }

    const char* filename = message.getPayload().getProperty(GHGUIProperties::GP_WIDGETFILENAME);
    if (filename) {
        GHIdentifier id = mIdFactory.generateHash(filename);
        mGUIMgr.popWidget(id, *tDesc, category, addToStack);
    }
    else {
        GHGUIWidgetResource* ret = (GHGUIWidgetResource*)message.getPayload().getProperty(GHGUIProperties::GP_WIDGETRESOURCEPTR);
        if (ret) {
            mGUIMgr.popWidget(ret, *tDesc, category, addToStack);
        }
    }
    if (deleteTDesc) {
        delete tDesc;
    }
}
