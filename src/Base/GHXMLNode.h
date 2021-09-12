// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHString.h"
#include <vector>
#include <map>
#include "GHString/GHIdentifierMap.h"
#include "GHPlatform/GHRefCounted.h"

class GHStringIdFactory;

class GHXMLNode
{
public:
    typedef std::map<GHString, GHString> AttributeMap;
    typedef std::vector<GHXMLNode*> NodeList;
    
public:
    GHXMLNode(GHRefCountedArrayType<char>* srcBuff=0);
    ~GHXMLNode(void);
    
    const GHString& getName(void) const { return mName; }
    void setName(const char* name, GHString::CharHandlingType charOwnership);
    
    const char* getValue(void) const { return mValue; }
    void setValue(const char* val, GHString::CharHandlingType charOwnership);
    
    const char* getAttribute(const char* name) const;
    void setAttribute(const char* name, GHString::CharHandlingType nameOwnership,
                      const char* val, GHString::CharHandlingType valOwnership);
    void deleteAttribute(const char* name);
    
    void addChild(GHXMLNode* node) { mChildren.push_back(node); }
    
    const AttributeMap& getAttributes(void) const { return mAttributes; }
    const NodeList& getChildren(void) const { return mChildren; }
    NodeList& getChildren(void) { return mChildren; }
	const GHXMLNode* getChild(const char* name, bool recurse) const;
    GHXMLNode* getChild(const char* name, bool recurse);

	bool readAttrFloat(const char* name, float& ret) const;
	bool readAttrBool(const char* name, bool& ret) const;
	bool readAttrInt(const char* name, int& ret) const;
	bool readAttrUInt(const char* name, unsigned int& ret) const;
    bool readAttrUShort(const char* name, unsigned short& ret) const;
	bool readAttrULong(const char* name, unsigned long& ret) const;
    bool readAttrIntList(const char* name, std::vector<int>& ret) const;
    bool readAttrIntArr(const char* name, int* ret, unsigned int count) const;
	bool readAttrFloatList(const char* name, std::vector<float>& ret) const;
    bool readAttrFloatArr(const char* name, float* ret, unsigned int count) const;
    bool readAttrEnum(const char* name, int& ret, const GHIdentifierMap<int>& enumMap) const;
    bool readAttrIdentifier(const char* name, GHIdentifier& ret, const GHStringIdFactory& idFactory) const;
    bool readAttrStrList(const char* name, std::vector<GHString>& ret) const;

    
    bool writeFloatAttr(const char* name, GHString::CharHandlingType nameOwnership, const float* floats, unsigned int count, unsigned int precision=6);

	GHXMLNode* clone(void) const;

private:
    GHString mName;
    GHString mValue;
    // child nodes.
	NodeList mChildren;
    // key/value pairs for attributes.
    AttributeMap mAttributes;
    GHRefCountedArrayType<char>* mSrcBuff;
};

