// Copyright Golden Hammer Software
#pragma once

class GHXMLNode;

class GHXMLNodeDeepCopier
{
public:
    static GHXMLNode* copy(const GHXMLNode& original);
};
