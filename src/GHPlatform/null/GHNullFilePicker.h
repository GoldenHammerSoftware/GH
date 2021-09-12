// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHFilePicker.h"
#include "GHUtils/GHPropertyContainer.h"

class GHNullFilePicker : public GHFilePicker
{
private:
	virtual void pickFile(PickedCallback& callback, const std::vector<const char*> exts) const
	{
		PickedCallback::Result result(0);
		callback.handleFilePicked(result);
	}
	virtual void pickMultipleFiles(PickedCallback& callback, const std::vector<const char*> exts) const
	{
		PickedCallback::Result result(0);
		callback.handleFilePicked(result);
	}
};
