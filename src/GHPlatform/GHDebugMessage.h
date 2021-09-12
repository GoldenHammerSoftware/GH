// Copyright Golden Hammer Software
#pragma once

class GHOutputPipe;

// this class is a singleton to make it easy to pop a debugging string in anywhere.
class GHDebugMessage
{
public:
	static void init(GHOutputPipe& output);
	// Output a string to some debugging console.
	static void outputString(const char* str, ...);
	// Allows us to exceed the internal debug print buffer size.
	static void outputPreformattedString(const char* str);
	static void shutdown(void) { sOutput = 0; }
    
protected:
	// A referenced output sender owned elsewhere.
	static GHOutputPipe* sOutput;
};
