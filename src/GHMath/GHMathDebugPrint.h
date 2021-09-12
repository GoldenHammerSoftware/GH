#pragma once

#include "GHPoint.h"

#ifndef GHMATHDEBUGPRINT

#define GHMATHDEBUGPRINT(...) if (GHMathDebugPrint::PrintFunc) { (*GHMathDebugPrint::PrintFunc)(__VA_ARGS__); } else { /*printf("Set a GHMathDebugPrint function");*/ }

#endif

namespace GHMathDebugPrint
{
	typedef void (*printf_t) (const char*, ...);
	extern printf_t PrintFunc;

	template <typename T, size_t COUNT>
	void printPoint(const GHPoint<T, COUNT>& point, const char* tag = "", const char* delimiter = " ", const char* end = "")
	{
		// Build the string in place so we can handle the templating.
		const size_t numberSize = 64; // overshoot
		const size_t maxTag = 64;
		const size_t maxEnd = 64;
		const size_t maxDelimiter = 64;
		assert(strlen(tag) < maxTag);
		assert(strlen(delimiter) < maxDelimiter);
		assert(strlen(end) < maxEnd);
		const size_t buffLen = COUNT * (numberSize + maxDelimiter) + maxEnd + maxTag + 1;
		char outputBuf[buffLen];
		memset(outputBuf, 0, buffLen);
	
		if (!delimiter || !*delimiter)
		{
			delimiter = " ";
		}

		sprintf(outputBuf, "%s", tag);
		for (size_t i = 0; i < COUNT; ++i)
		{
			sprintf(outputBuf, "%s%f", outputBuf, point.getCoords()[i]);
			if (i < COUNT - 1)
			{
				sprintf(outputBuf, "%s%s", outputBuf, delimiter);
			}
		}
		sprintf(outputBuf, "%s%s", outputBuf, end);

		//GHMATHDEBUGPRINT("%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s", tag, mMatrix[0], delimiter, mMatrix[1], delimiter, mMatrix[2], delimiter, mMatrix[3], delimiter, mMatrix[4], delimiter, mMatrix[5], delimiter, mMatrix[6], delimiter, mMatrix[7], delimiter, mMatrix[8], delimiter, mMatrix[9], delimiter, mMatrix[10], delimiter, mMatrix[11], delimiter, mMatrix[12], delimiter, mMatrix[13], delimiter, mMatrix[14], delimiter, mMatrix[15], end);
		GHMATHDEBUGPRINT(outputBuf);
	}
	
};

