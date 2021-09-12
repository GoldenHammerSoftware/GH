// Copyright Golden Hammer Software
#include "GHXMLNode.h"
#include "GHString/GHNumberListParser.h"
#include <string.h>
#include "GHPlatform/GHDebugMessage.h"
#include "GHString/GHStringTokenizer.h"
#include <cassert>
#include <float.h>

GHXMLNode::GHXMLNode(GHRefCountedArrayType<char>* srcBuff)
: mSrcBuff(srcBuff)
{
    if (mSrcBuff) {
        mSrcBuff->acquire();
    }
}

GHXMLNode::~GHXMLNode(void)
{
    NodeList::const_iterator childIter;
    for (childIter = mChildren.begin(); childIter != mChildren.end(); ++childIter)
    {
        delete *childIter;
    }
    if (mSrcBuff) {
        mSrcBuff->release();
    }
}

void GHXMLNode::setName(const char* name, GHString::CharHandlingType charOwnership)
{
    mName.setConstChars(name, charOwnership);
}

void GHXMLNode::setValue(const char* val, GHString::CharHandlingType charOwnership)
{
    mValue.setConstChars(val, charOwnership);
}

const char* GHXMLNode::getAttribute(const char* name) const
{
    AttributeMap::const_iterator findIter = mAttributes.find(name);
    if (findIter == mAttributes.end())
    {
        return 0;
    }
    return findIter->second;
}

void GHXMLNode::setAttribute(const char* name, GHString::CharHandlingType nameOwnership,
                             const char* val, GHString::CharHandlingType valOwnership)
{    
    std::pair<GHString,GHString> attrInsert;
    attrInsert.first.setConstChars(name, nameOwnership);
    
    AttributeMap::iterator iter = mAttributes.find(attrInsert.first);
    if (iter != mAttributes.end())
    {
        iter->second.setConstChars(val, valOwnership);
        return;
    }
    
    attrInsert.second.setConstChars(val, valOwnership);
    mAttributes.insert(attrInsert);
}

void GHXMLNode::deleteAttribute(const char* name)
{
    GHString key;
    key.setConstChars(name, GHString::CHT_REFERENCE);
    AttributeMap::iterator iter = mAttributes.find(key);
    if (iter != mAttributes.end())
    {
        mAttributes.erase(iter);
    }
}

const GHXMLNode* GHXMLNode::getChild(const char* name, bool recurse) const
{
    if (mName == name) {
        return this;
    }
    
    for (size_t i = 0; i < mChildren.size(); ++i)
    {
        if (recurse)
        {
            const GHXMLNode* ret = mChildren[i]->getChild(name, recurse);
            if (ret) return ret;
        }
        else
        {
            const GHXMLNode* currChild = mChildren[i];
            if (currChild->getName() == name) {
                return currChild;
            }
        }
    }
    return 0;
}

GHXMLNode* GHXMLNode::getChild(const char* name, bool recurse)
{
    if (mName == name) {
        return this;
    }
    
    for (size_t i = 0; i < mChildren.size(); ++i)
    {
        if (recurse)
        {
            GHXMLNode* ret = mChildren[i]->getChild(name, recurse);
            if (ret) return ret;
        }
        else
        {
            GHXMLNode* currChild = mChildren[i];
            if (currChild->getName() == name) {
                return currChild;
            }
        }
    }
    return 0;
}

bool GHXMLNode::readAttrFloat(const char* name, float& ret) const
{
    const char* attr = getAttribute(name);
	if (!attr) return false;
	ret = (float)::atof(attr);
	return true;
}

bool GHXMLNode::readAttrBool(const char* name, bool& ret) const
{
    const char* attr = getAttribute(name);
	if (!attr) return false;
	size_t len = ::strlen(attr);
	if (len == 4 && !::strcmp(attr, "true")) {
		ret = true;
	}
	else ret = false;
	return true;
}

bool GHXMLNode::readAttrInt(const char* name, int& ret) const
{
    const char* attr = getAttribute(name);
	if (!attr) return false;
	ret = ::atoi(attr);
	return true;
}

bool GHXMLNode::readAttrUInt(const char* name, unsigned int& ret) const
{
    const char* attr = getAttribute(name);
	if (!attr) return false;
	//sufficiently large values were not being properly converted by atoi or atol
	// even though they were within the bounds of unsigned int
	ret = (unsigned int)::atoll(attr);
	return true;
}

bool GHXMLNode::readAttrUShort(const char* name, unsigned short& ret) const
{
    const char* attr = getAttribute(name);
	if (!attr) return false;
	ret = ::atoi(attr);
	return true;
}

bool GHXMLNode::readAttrULong(const char* name, unsigned long& ret) const
{
	const char* attr = getAttribute(name);
	if (!attr) { return false; }
	ret = (unsigned long)::atoll(attr);
	return true;
}

bool GHXMLNode::readAttrIntList(const char* name, std::vector<int>& ret) const
{
	const char* attr = getAttribute(name);
	if (!attr) return false;
	GHNumberListParser::parseIntVector(attr, ret);
	return true;
}

bool GHXMLNode::readAttrIntArr(const char* name, int* ret, unsigned int count) const
{
    const char* attr = getAttribute(name);
    if (!attr) return false;
    
	GHNumberListParser::parseIntList(attr, ret, count);
	return true;
}

bool GHXMLNode::readAttrFloatList(const char* name, std::vector<float>& ret) const
{
	const char* attr = getAttribute(name);
	if (!attr) return false;
	GHNumberListParser::parseFloatVector(attr, ret);
	return true;
}

bool GHXMLNode::readAttrFloatArr(const char* name, float* ret, unsigned int count) const
{
    const char* attr = getAttribute(name);
    if (!attr) return false;
    
	GHNumberListParser::parseFloatList(attr, ret, count);
	return true;
}

bool GHXMLNode::readAttrEnum(const char* name, int& ret, const GHIdentifierMap<int>& enumMap) const
{
    const char* attr = getAttribute(name);
    if (!attr) return false;
    
    const int* enumPtr = enumMap.find(attr);
    if (!enumPtr) {
        GHDebugMessage::outputString("Failed to find enum %s", attr);
        return false;
    }
    ret = *enumPtr;
    return true;
}

bool GHXMLNode::readAttrIdentifier(const char* name, GHIdentifier& ret, const GHStringIdFactory& idFactory) const
{
    const char* attr = getAttribute(name);
    if (!attr) return false;
    ret = idFactory.generateHash(attr);
    return true;
}

bool GHXMLNode::readAttrStrList(const char* name, std::vector<GHString>& ret) const
{
	const char* attr = getAttribute(name);
	if (!attr) return false;
    GHStringTokenizer strTok;
    const char* token;
    int tokenLen;
	while (attr && *attr != '\0')
	{
        if (strTok.popToken(attr, ' ', token, tokenLen, attr))
        {
            char* buf = new char[tokenLen+1];
            strncpy(buf, token, tokenLen);
            buf[tokenLen] = '\0';
            ret.push_back(GHString(buf, GHString::CHT_CLAIM));
        }
	}
    return true;
}

bool GHXMLNode::writeFloatAttr(const char* name, GHString::CharHandlingType nameOwnership, const float* floats, unsigned int count, unsigned int precision)
{
    //Potential todo: also support other parameters to output, eg scientific notation 

    //total value (assuming default precision of 6): 47 
    const unsigned int maxSingleFloatChars = 1                // sign character for negative floats
                                            + FLT_MAX_10_EXP  // maximum exponent of float value in base 10, since we are printing in base 10 (38)
                                            + 1               // decimal point character
                                            + precision       // number of characters after the decimal point
                                            + 1;              // space character between floats

    const unsigned int totalStrlen = maxSingleFloatChars * count + 1; //+1 for null char
    char* value = new char[totalStrlen];

    char* entryStart = value;
    unsigned int remainingBuffer = totalStrlen;
    for (unsigned int i = 0; i < count; ++i)
    {
        int numCharsWritten = snprintf(entryStart, remainingBuffer, "%.*f ", precision, floats[i]);
        assert(numCharsWritten >= 0 && (unsigned)numCharsWritten <= maxSingleFloatChars);

        if (numCharsWritten < 0
            || (unsigned)numCharsWritten >= remainingBuffer) {
            delete[] value;
            return false;
        }

        entryStart += numCharsWritten;
        remainingBuffer -= (unsigned)numCharsWritten;
    }

    setAttribute(name, nameOwnership, value, GHString::CHT_CLAIM);
    return true;
}


GHXMLNode* GHXMLNode::clone(void) const
{
	GHXMLNode* ret = new GHXMLNode(mSrcBuff);

	ret->mName = mName;
	ret->mValue = mValue;
	ret->mAttributes = mAttributes;

	size_t numChildren = mChildren.size();
	ret->mChildren.resize(numChildren);
	for (size_t i = 0; i < numChildren; ++i)
	{
		const GHXMLNode* myChild = mChildren[i];
		GHXMLNode* clonedChild = myChild->clone();
		ret->mChildren[i] = clonedChild;
	}

	return ret;
}

