#pragma once

class GHXMLNode;

//Simple utility for debug printing an xml node and its children to the debug output.
namespace GHXMLDebugPrint
{
	void print(const GHXMLNode& node);
}
