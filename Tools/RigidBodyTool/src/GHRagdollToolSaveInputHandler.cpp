// Copyright 2010 Golden Hammer Software
#include "GHRagdollToolSaveInputHandler.h"
#include "GHInputState.h"
#include "GHRagdollGenerator.h"
#include "GHKeyDef.h"
#include "GHPropertyContainer.h"
#include "GHRagdollToolProperties.h"
#include "GHXMLNode.h"
#include "GHXMLSerializer.h"

GHRagdollToolSaveInputHandler::GHRagdollToolSaveInputHandler(const GHXMLSerializer& xmlSerializer,
                                                             const GHInputState& inputState,
                                                             GHRagdollGenerator& ragdollGenerator,
                                                             GHPropertyContainer& props)
: mSerializer(xmlSerializer)
, mInputState(inputState)
, mRagdollGenerator(ragdollGenerator)
, mProps(props)
{
    
}

void GHRagdollToolSaveInputHandler::onActivate(void)
{
    
}

void GHRagdollToolSaveInputHandler::onDeactivate(void)
{
    
}

void GHRagdollToolSaveInputHandler::update(void)
{
    if (!mInputState.getKeyState(0, GHKeyDef::KEY_CONTROL)) {
        return;
    }
    
    if (mInputState.checkKeyChange(0, 's', true))
    {
        const GHXMLNode* xmlNode = mRagdollGenerator.getXMLNode();
        if (mSerializer.writeXMLFile("ragdolltooloutput.xml", *xmlNode))
        {
            mProps.setProperty(GHRagdollToolProperties::COMMANDRESULT, "saved file");
        }
        else
        {
            mProps.setProperty(GHRagdollToolProperties::COMMANDRESULT, "failed to save file");
        }
    }
}
