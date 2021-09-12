// Copyright Golden Hammer Software
#include "GHXMLSerializer.h"
#include "GHPlatform/GHFile.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHString/GHStringTokenizer.h"
#include "GHString/GHWhitespaceUtil.h"
#include "GHUtils/GHProfiler.h"
#include "GHPlatform/GHXorFile.h"
#include <string.h>

GHXMLSerializer::GHXMLSerializer(const GHFileOpener& fileOpener)
: mFileOpener(fileOpener)
{
}

bool GHXMLSerializer::writeXMLFile(const char* fileName, const GHXMLNode& node) const
{
	GHFile* file = mFileOpener.openFile(fileName, GHFile::FT_TEXT, GHFile::FM_WRITEONLY);
	if (!file) {
		return false;
	}
	writeXMLNode(*file, node, 0);
	delete file;
	return true;
}


bool GHXMLSerializer::writeEncryptedFile(const char* fileName, const GHXMLNode& node, unsigned char key) const
{
    GHFile* encryptedFile = mFileOpener.openFile(fileName, GHFile::FT_TEXT, GHFile::FM_WRITEONLY);
	if (!encryptedFile) {
		return false;
	}
    GHXorFile file(*encryptedFile, key);
	writeXMLNode(file, node, -1);
	file.closeFile();
	return true;
}

void GHXMLSerializer::writeXMLNode(GHFile& file, const GHXMLNode& node, int depth) const
{
    for (int i = 0; i < depth; ++i)
    {
        file.writeBuffer("\t", 1);
    }
    
	file.writeBuffer("<",1);
	file.writeBuffer(node.getName().getChars(), ::strlen(node.getName().getChars()));
	file.writeBuffer(" ",1);
    
	const GHXMLNode::AttributeMap& attributes = node.getAttributes();
	GHXMLNode::AttributeMap::const_iterator attrIter;
	for (attrIter = attributes.begin(); attrIter != attributes.end(); ++attrIter)
	{
		writeXMLAttribute(file, attrIter->first.getChars(), 
                          attrIter->second.getChars());
	}
	
	if (node.getChildren().size())
	{
		file.writeBuffer(">\n",2);
	
		const GHXMLNode::NodeList& children = node.getChildren();
		GHXMLNode::NodeList::const_iterator childIter;
		for (childIter = children.begin(); childIter != children.end(); ++childIter)
		{
			writeXMLNode(file, *(*childIter), depth < 0 ? -1 : depth+1);
		}
	
		for (int i = 0; i < depth; ++i)
		{
			file.writeBuffer("\t", 1);
		}
    
		file.writeBuffer("</",2);
		file.writeBuffer(node.getName().getChars(), ::strlen(node.getName().getChars()));
		file.writeBuffer(">\n",2);
	}
	else
	{
		file.writeBuffer("/>\n", 3);
	}
}

void GHXMLSerializer::writeXMLAttribute(GHFile& file, const char* key, const char* value) const
{
	if (!key || !value) return;
	file.writeBuffer(key, ::strlen(key));
	file.writeBuffer("=\"",2);
	file.writeBuffer(value, ::strlen(value));
	file.writeBuffer("\" ",2);
}

GHXMLNode* GHXMLSerializer::loadXMLFile(const char* fileName) const
{
    GHPROFILESCOPE("loadXMLFile", GHString::CHT_REFERENCE)

    GHPROFILEBEGIN("openXMLfile")
    //GHPROFILEBEGINARGS(fileName, GHString::CHT_COPY)
	GHFile* file = mFileOpener.openFile(fileName, GHFile::FT_TEXT, GHFile::FM_READONLY);
    //GHPROFILEEND(fileName)
    GHPROFILEEND("openXMLfile")
	if (!file) {
		GHDebugMessage::outputString("Failed to load XML file %s", fileName);
		return 0;
	}
    GHXMLNode* retVal = loadFromFile(*file);

	delete file;
	return retVal;
}

GHXMLNode* GHXMLSerializer::loadXMLFromString(const char* string) const
{
	GHXMLNode* retVal = 0;
	char* dummy;
	if (!loadXMLNode(string, retVal, dummy, 0))
	{
		return nullptr;
	}
	return retVal;
}

GHXMLNode* GHXMLSerializer::loadEncryptedFile(const char* fileName, unsigned char key) const
{
	GHFile* encryptedFile = mFileOpener.openFile(fileName, GHFile::FT_TEXT, GHFile::FM_READONLY);
	if (!encryptedFile) {
		GHDebugMessage::outputString("Failed to load eXML file %s", fileName);
		return 0;
	}
    GHXorFile file(*encryptedFile, key);
    GHXMLNode* retVal = loadFromFile(file);
	delete encryptedFile;
	return retVal;
}

GHXMLNode* GHXMLSerializer::loadFromFile(GHFile& file) const
{
	if (!file.readIntoBuffer()) {
		GHDebugMessage::outputString("Can't read XML file into buffer");
		return 0;
	}
	char* text = 0;
	size_t textSize = 0;
	if (!file.getFileBuffer(text, textSize)) {
		return 0;
	}
	file.releaseFileBuffer();
    
	GHXMLNode* retVal = 0;
	char* dummy;
	loadXMLNode(text, retVal, dummy, 0);
    
	delete text;
    return retVal;
}

bool GHXMLSerializer::loadXMLNode(const char* start, GHXMLNode*& node, char*& nextStart, const char* end) const
{
	char* firstBracket = 0;
	char* attributesEnd = 0;
	char* nodeEnd = 0;
	if (!findImportantNodeMarkers(start, firstBracket, attributesEnd, nodeEnd, nextStart)) {
		GHDebugMessage::outputString("Unable to load xml node: odd format.\n%s\n", start);
		return false;
	}
    
	if (nodeEnd && firstBracket >= nodeEnd) {
		return false;
	}
    
	if (*(firstBracket+1) == '\0') {
		GHDebugMessage::outputString("Unable to load xml node: unexpected EOF.\n");
		return false;
	}
    
	if (*(firstBracket+1) == '?' ||
        *(firstBracket+1) == '!') 
    {
        // we are in a header or comment node, skip it.
        return loadXMLNode(nextStart, node, nextStart, end);
    }
    
	node = new GHXMLNode;
    
	char* attributesStart = 0;
    // readNodeName could set attributesEnd to \0, so grab the char after it first.
	char* nextChild = attributesEnd+1;
	readNodeName(firstBracket+1, attributesEnd, attributesStart, *node);
    
	if (attributesStart) {
		readAttributes(attributesStart, attributesEnd, *node);
	}
	
    // support for <include file=blah.xml/>
    // if we hit a node named include, replace that node with the one in the specified file.
    if (node->getName() == "include")
    {
        GHPROFILESCOPE("xmlInclude", GHString::CHT_REFERENCE)
        const char* fileAttr = node->getAttribute("file");
        if (!fileAttr) {
            GHDebugMessage::outputString("xml include with no file specified");
        }
        else {
            GHXMLNode* includeNode = loadXMLFile(fileAttr);
            if (!includeNode) {
                GHDebugMessage::outputString("could not load xml include %s", fileAttr);
            }
            else {
                delete node;
                node = includeNode;
            }
        }
    }
        
	char* nextChildStart = 0;
	while (nextChild < nodeEnd && nextChild) {
		GHXMLNode* child = 0;
		if (loadXMLNode(nextChild, child, nextChildStart, nodeEnd)) {
			node->addChild(child);
			nextChild = nextChildStart;
		}
		else {
			readValue(nextChild+1, nodeEnd, *node);
			break;
		}
	}
	return true;
}

bool GHXMLSerializer::findImportantNodeMarkers(const char* start, char*& firstBracket, 
											char*& attributesEnd, char*& nodeEnd, 
											char*& nextStart) const
{
	if (!findNextOpenBracket(start, firstBracket)) {
		GHDebugMessage::outputString("Invalid xml format: can't find open bracket. %s\n", start);
		return false;
	}
    
    if (*(firstBracket+1) == '?') {
        nodeEnd = strstr(firstBracket+1, "?>");
        if (!nodeEnd) {
            GHDebugMessage::outputString("Invalid xml, no ?> found for <?. %s\n", start);
            return false;
        }
        nextStart = nodeEnd+2;
        return true;
    }
    if (*(firstBracket+1) == '!') {
        if (*(firstBracket+2) == '-' && *(firstBracket+3) == '-') 
        {
            nodeEnd = strstr(firstBracket+2, "-->");
            if (!nodeEnd) {
                GHDebugMessage::outputString("Invalid xml, no --> for <!--");
                return false;
            }
            nextStart = nodeEnd + 3;
            return true;
        }
    }
    
	if (!findNextCloseBracket(firstBracket, attributesEnd)) {
		GHDebugMessage::outputString("Invalid xml format: can't find close bracket.\n");
		return false;
	}
    
	if (!findMatchingEndMarker(firstBracket, (const char*&)nodeEnd)) {
		GHDebugMessage::outputString("Invalid xml format: odd end marker?.\n");
		return false;
	}
    
	if (*nodeEnd == '/') {
		if (*(nodeEnd+1) == '\0' || *(nodeEnd+2) == '\0') {
			nextStart = 0;
		}
		else {
			nextStart = nodeEnd+2;
		}
	}
	else {
		if (!findNextCloseBracket(nodeEnd, nextStart)) {
			GHDebugMessage::outputString("Invalid xml format: odd end marker?.\n");
			return false;
		}
		if (*(nextStart+1) == '\0') {
			nextStart = 0;
		}
		else {
			nextStart++;
		}
	}
    
	return true;
}

static void setNodeNameWithEndStripped(char* name, GHXMLNode& node)
{
}

void GHXMLSerializer::readNodeName(char* firstBracket, const char* attributesEnd,
                                   char*& attrStart, GHXMLNode& node) const
{
	char* name = 0;
	GHStringTokenizer strTok;
	if (strTok.popToken(firstBracket, GHStringTokenizer::kWhiteTxtMarkers, name, attrStart, attributesEnd)) 
    {
        char* endMarker = strchr(name, '/');
        if (endMarker) {
            *endMarker = '\0';
        }
        node.setName(name, GHString::CHT_COPY);
	}
	else 
    {
		// no attributes present, only a name.
		char* nameEnd = 0;
        if (*(attributesEnd-1) == '/') {
            nameEnd = (char*)(attributesEnd-1);
        }
        else {
            nameEnd = (char*)attributesEnd;
        }

        nameEnd[0] = '\0';
		node.setName(firstBracket, GHString::CHT_COPY);
		attrStart = 0;
	}
}

void GHXMLSerializer::readAttributes(char* curr, char* attributesEnd, GHXMLNode& node) const
{
	char* next = 0;
	char* token = 0;
	GHStringTokenizer strTok;
    
	while (strTok.popToken(curr, GHStringTokenizer::kWhiteTxtMarkers, token, next, attributesEnd)) 
    {
        if (next >= attributesEnd)
        {
            // if the node is all one one line at the end of the file with no newline
            // then <test blah=0/> will look like attribute blah = "0/>"
            if (*(attributesEnd-1) == '/') {
                *(attributesEnd-1) = '\0';
            }
        }
        
		parseAttribute(token, node);
		curr = next;
		if (next >= attributesEnd) {
			break;
		}
	}
	if (curr < attributesEnd) 
    {
		// node could look like <test blah1=9/>
		//  or <test blah1=0><parm /></>
		bool removedSlash = false;
		if (*(attributesEnd-1) != '/') {
			*attributesEnd = '\0';
		}
		else {
			*(attributesEnd-1) = '\0';
			removedSlash = true;
		}
		parseAttribute(curr, node);
		if (removedSlash) {
			*(attributesEnd-1) = '/';
		}
		else {
			*(attributesEnd) = '>';
		}
	}
}

void GHXMLSerializer::readValue(const char* start, char* end, GHXMLNode& node) const
{
	if(start >= end)
		return;
	start = GHWhitespaceUtil::skipWhitespace(start, 1, (int)(end - start));
	if(start >= end)
		return;
	end = GHWhitespaceUtil::skipWhitespace(end, -1, (int)(end - start));
	if(end > start)
    {
        end[0] = '\0';
		node.setValue(start, GHString::CHT_COPY);
    }
}

void GHXMLSerializer::parseAttribute(char* token, GHXMLNode& node) const
{
	char* attrName = 0;
	char* attrVal = 0;
	GHStringTokenizer strTok;
	if (strTok.popToken(token, '=', attrName, attrVal)) 
    {
		if (attrVal[0] == '"') 
        {
            attrVal = attrVal+1;
            attrVal[::strlen(attrVal)-1] = '\0';
		}

        node.setAttribute(attrName, GHString::CHT_COPY, attrVal, GHString::CHT_COPY);
	}
}

bool GHXMLSerializer::findNextMatch(const char* start, char*& next, char match) const
{
    bool inQuotes = false;
    next = (char*)start;
    while (next && *next != '\0' && (inQuotes || *next != match))
    {
        if (*next == '"') {
            inQuotes = !inQuotes;
        }
        ++next;
    }
	if (!next) return false;
	return true;
}

bool GHXMLSerializer::findNextOpenBracket(const char* start, char*& next) const
{
    return findNextMatch(start, next, '<');
}

bool GHXMLSerializer::findNextCloseBracket(const char* start, char*& next) const
{
    return findNextMatch(start, next, '>');
}

bool GHXMLSerializer::findEndMarker(const char* start, char*& next) const
{
	char* nextStandardClose = (char*)::strstr(start, "/>");
	char* nextAltClose = (char*)::strstr(start, "</");
	if (!nextStandardClose && !nextAltClose) return false;
    
	if (!nextStandardClose || nextAltClose < nextStandardClose) {
		char* nextBracket = 0;
		if (findNextCloseBracket(nextAltClose, nextBracket)) {
			next = nextAltClose;
			return true;
		}
	}
    
	next = nextStandardClose;
	if (!next) return false;
	return true;
}

bool GHXMLSerializer::findMatchingEndMarker(const char* startMarker, const char*& endMarker) const
{
	//GHDebugMessage::outputString("fMEMarker - %s", startMarker);
    if (!strncmp(startMarker, "<!--", 4))
    {
        const char* commentEnd = strstr(startMarker, "-->");
        if (commentEnd)
        {
            endMarker = commentEnd+2;
            return true;
        }
    }
    
	while (true) {
		char* nextEndMarker = 0;
		if (!findEndMarker(startMarker, nextEndMarker)) {
			return false;
		}
		char* nextOpen = 0;
		if (!findNextOpenBracket(startMarker+1, nextOpen)) {
			endMarker = nextEndMarker;
			return true;
		}
		else {
			if (!nextOpen || nextOpen >= nextEndMarker) {
				endMarker = nextEndMarker;
				return true;
			}
		}
        
		// skip nesting.
		if (!findMatchingEndMarker(nextOpen, startMarker)) {
			return false;
		}
		startMarker++;
	}
}
