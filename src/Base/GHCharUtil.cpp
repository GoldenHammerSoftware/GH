// Copyright Golden Hammer Software
#include "GHCharUtil.h"
#include <cctype>

GHKeyDef::Enum GHCharUtil::applyUpperCase(GHKeyDef::Enum input, bool shiftHeld)
{
	if (!shiftHeld) {
		return input;
	}
	if (input == '\'') {
		return (GHKeyDef::Enum)('~');
	}
	if (input == '1') {
		return (GHKeyDef::Enum)('!');
	}
	if (input == '2') {
		return (GHKeyDef::Enum)('@');
	}
	if (input == '3') {
		return (GHKeyDef::Enum)('#');
	}
	if (input == '4') {
		return (GHKeyDef::Enum)('$');
	}
	if (input == '5') {
		return (GHKeyDef::Enum)('%');
	}
	if (input == '6') {
		return (GHKeyDef::Enum)('^');
	}
	if (input == '7') {
		return (GHKeyDef::Enum)('&');
	}
	if (input == '8') {
		return (GHKeyDef::Enum)('*');
	}
	if (input == '9') {
		return (GHKeyDef::Enum)('(');
	}
	if (input == '0') {
		return (GHKeyDef::Enum)(')');
	}

	if (input == '-') {
		return (GHKeyDef::Enum)('_');
	}
	if (input == '=') {
		return (GHKeyDef::Enum)('+');
	}
	if (input == '[') {
		return (GHKeyDef::Enum)('{');
	}
	if (input == ']') {
		return (GHKeyDef::Enum)('}');
	}
	if (input == '\\') {
		return (GHKeyDef::Enum)('|');
	}
	if (input == ';') {
		return (GHKeyDef::Enum)(':');
	}
	if (input == '\'') {
		return (GHKeyDef::Enum)('\"');
	}

	if (input == ',') {
		return (GHKeyDef::Enum)('<');
	}
	if (input == '.') {
		return (GHKeyDef::Enum)('>');
	}
	if (input == '/') {
		return (GHKeyDef::Enum)('?');
	}

	return (GHKeyDef::Enum)toupper((int)input);
}
