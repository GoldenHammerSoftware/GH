#pragma once

#include "GHKeyDef.h"

// util for converting windows key codes to ghkeydef
class GHMetroKeyConverter
{
public:
	static GHKeyDef::Enum convertKey(Windows::System::VirtualKey key);
};