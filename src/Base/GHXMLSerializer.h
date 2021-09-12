// Copyright Golden Hammer Software
#pragma once

class GHFileOpener;
class GHFile;
class GHXMLNode;

class GHXMLSerializer
{
public:
    GHXMLSerializer(const GHFileOpener& fileOpener);

	// NOTE: expecting one root node per file.
	GHXMLNode* loadXMLFile(const char* fileName) const;
    GHXMLNode* loadEncryptedFile(const char* fileName, unsigned char key) const;
	bool writeXMLFile(const char* fileName, const GHXMLNode& node) const;
	bool writeEncryptedFile(const char* fileName, const GHXMLNode& node, unsigned char key) const;
	GHXMLNode* loadXMLFromString(const char* string) const;
	
private:
    GHXMLNode* loadFromFile(GHFile& file) const;
    
	bool loadXMLNode(const char* start, GHXMLNode*& node, char*& nextStart, const char* end) const;
    
	bool findImportantNodeMarkers(const char* start, char*& firstBracket, 
                                  char*& attributesEnd, char*& nodeEnd, char*& nextStart) const;
	void readNodeName(char* firstBracket, const char* attributesEnd,
                      char*& attrStart, GHXMLNode& node) const;
    
	void readAttributes(char* curr, char* attributesEnd, GHXMLNode& node) const;
	void parseAttribute(char* token, GHXMLNode& node) const;
	
	void readValue(const char* start, char* end, GHXMLNode& node) const;
    
    bool findNextMatch(const char* start, char*& next, char match) const;
	bool findNextOpenBracket(const char* start, char*& next) const;
	bool findNextCloseBracket(const char* start, char*& next) const;
	bool findEndMarker(const char* start, char*& next) const;
	bool findMatchingEndMarker(const char* startMarker, const char*& endMarker) const;
	
	void writeXMLNode(GHFile& file, const GHXMLNode& node, int depth) const;
	void writeXMLAttribute(GHFile& file, const char* key, const char* value) const;    

private:
    const GHFileOpener& mFileOpener;
};
