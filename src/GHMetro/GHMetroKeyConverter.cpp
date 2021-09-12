#include "GHMetroKeyConverter.h"
#include <cctype>

GHKeyDef::Enum GHMetroKeyConverter::convertKey(Windows::System::VirtualKey key) 
{
	switch (key)
	{
	case Windows::System::VirtualKey::Up:
		return GHKeyDef::KEY_UPARROW;
	case Windows::System::VirtualKey::Down:
		return GHKeyDef::KEY_DOWNARROW;
	case Windows::System::VirtualKey::Left:
		return GHKeyDef::KEY_LEFTARROW;
	case Windows::System::VirtualKey::Right:
		return GHKeyDef::KEY_RIGHTARROW;
	case Windows::System::VirtualKey::Enter:
		return GHKeyDef::KEY_ENTER;
	case Windows::System::VirtualKey::Shift:
		return GHKeyDef::KEY_SHIFT;
	case Windows::System::VirtualKey::Escape:
		return GHKeyDef::KEY_ESC;
	case Windows::System::VirtualKey::LeftButton:
		return GHKeyDef::KEY_MOUSE1;
	case Windows::System::VirtualKey::RightButton:
		return GHKeyDef::KEY_MOUSE2;
	case Windows::System::VirtualKey::MiddleButton:
		return GHKeyDef::KEY_MOUSE3;
	default:
		/*
		Platform::String^ keyStr = key.ToString();
		Windows::System::VirtualKey::
		char c = (char)(keyStr->Data()[0]);
		if (keyStr->Equals(L"Windows.System.VirtualKey")) {
		int brkpt = 1;
		}
		GHDebugMessage::outputString("key to str %ws", keyStr->Data());
		return (GHKeyDef::Enum)tolower(c);
		*/

		// some windows keys are really odd values?
		if ((int)key == 223) {
			return (GHKeyDef::Enum)((int)'`');
		}
		else if ((int)key == 186) {
			return (GHKeyDef::Enum)((int)';');
		}
		else if ((int)key == 192) {
			return (GHKeyDef::Enum)((int)'\`');
		}
		else if ((int)key == 222) {
			return (GHKeyDef::Enum)((int)'\'');
		}
		else if ((int)key == 188) {
			return (GHKeyDef::Enum)((int)',');
		}
		else if ((int)key == 190) {
			return (GHKeyDef::Enum)((int)'.');
		}
		else if ((int)key == 191) {
			return (GHKeyDef::Enum)((int)'/');
		}
		else if ((int)key == 219) {
			return (GHKeyDef::Enum)((int)'[');
		}
		else if ((int)key == 221) {
			return (GHKeyDef::Enum)((int)']');
		}
		else if ((int)key == 222) {
			return (GHKeyDef::Enum)((int)'\\');
		}
		else if ((int)key == 189) {
			return (GHKeyDef::Enum)((int)'-');
		}
		else if ((int)key == 187) {
			return (GHKeyDef::Enum)((int)'=');
		}
		else if ((int)key == 46) {
			return GHKeyDef::KEY_DELETE;
		}
		else if ((int)key == 45) {
			return GHKeyDef::KEY_INS;
		}
		//GHDebugMessage::outputString("%d %c", key, key);
		return (GHKeyDef::Enum)tolower((int)key);
	};
}

