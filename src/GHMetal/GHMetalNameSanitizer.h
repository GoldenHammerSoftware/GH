#pragma once

namespace GHMetalNameSanitizer
{
	void sanitizeName(const char* inName, char* outName)
	{
		const char* dotPos = strstr(inName, ".");
		assert(dotPos);
		if (dotPos)
		{
			snprintf(outName, 2 + dotPos - inName, "n%s", inName);
		}
		else
		{
			sprintf(outName, "n%s", inName);
		}

		size_t nameLen = strlen(outName);
		for (size_t i = 0; i < nameLen; ++i)
		{
			if (outName[i] == '-')
			{
				outName[i] = '_';
			}
		}
	}
};
