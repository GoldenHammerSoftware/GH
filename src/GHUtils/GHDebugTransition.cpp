// Copyright Golden Hammer Software
#include "GHDebugTransition.h"
#include "GHPlatform/GHDebugMessage.h"

GHDebugTransition::GHDebugTransition(const char* identifier)
{
	mIdentifier.setConstChars(identifier ? identifier : "", GHString::CHT_COPY);
}

void GHDebugTransition::activate(void)
{
	GHDebugMessage::outputString("Activating debug transition %s", mIdentifier.getChars());
}

void GHDebugTransition::deactivate(void)
{
	GHDebugMessage::outputString("Deactivating debug transition %s", mIdentifier.getChars());
}
