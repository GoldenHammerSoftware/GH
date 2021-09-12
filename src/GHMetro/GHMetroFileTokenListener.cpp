// Copyright 2010 Golden Hammer Software
#include "GHMetroFileTokenListener.h"
#include "GHBaseIdentifiers.h"
#include "GHUtils/GHEventMgr.h"
#include "GHUtils/GHMessage.h"
#include <wrl.h>
#include "GHUtils/GHUtilsIdentifiers.h"

GHMetroFileTokenListener::GHMetroFileTokenListener(GHEventMgr& eventMgr)
: mEventMgr(eventMgr)
{
	mEventMgr.registerListener(GHBaseIdentifiers::M_RELEASEFILETOKEN, *this);
}

GHMetroFileTokenListener::~GHMetroFileTokenListener(void)
{
	mEventMgr.unregisterListener(GHBaseIdentifiers::M_RELEASEFILETOKEN, *this);
}

void GHMetroFileTokenListener::handleMessage(const GHMessage& message)
{
	char* tokenStr = (char*)message.getPayload().getProperty(GHUtilsIdentifiers::VAL);
	if (!tokenStr)
	{
		return;
	}

	size_t tokenLen = strlen(tokenStr);
	wchar_t* tokenBuffer = new wchar_t[tokenLen + 1];
	memset(tokenBuffer, 0, (tokenLen + 1)*sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, tokenStr, -1, tokenBuffer, tokenLen);
	Platform::String^ filekeyStr = ref new Platform::String(tokenBuffer);

	Windows::Storage::AccessCache::StorageApplicationPermissions::FutureAccessList->Remove(filekeyStr);
}
