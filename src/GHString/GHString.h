// Copyright Golden Hammer Software
#pragma once

#include <stdlib.h>

// A convenience and safety wrapper for char*s.
// A GHString can be created with a const char* or a char*.
// The CharHandlingType is used to manage whether we make a copy and whether we delete the memory.
class GHString
{
public:
	enum CharHandlingType {
		// just point to the new chars
		CHT_REFERENCE,
		// make a copy of the chars
		CHT_COPY,
		// point to the new chars, and take responsibility for deletion.
		CHT_CLAIM
	};

	GHString(void);
	GHString(const GHString& rhs);
	GHString(const char* text, CharHandlingType type=CHT_REFERENCE);
	~GHString(void);

	// Set our const chars and handling type.  Non-const chars will be deleted if present.
	void setConstChars(const char* chars, CharHandlingType type);
	// Set our non-const chars and handling type.  Const chars will be set to 0.
	void setChars(char* chars, CharHandlingType type);

	// Become a truncation of chars.  if char length > maxLen then add "..." at the end.
	void truncateChars(const char* chars, size_t maxLen);
    // Become a printf-style formatting of an input string and arguments.
    void format(const char* str, ...);

	const char* getChars(void) const;
    char* getNonConstChars(void);
	
	size_t getCharLen(void) const { if (!mChars && !mConstChars) return 0; return mCharLen; }
    // return true if string is 0 or ""
    bool isEmpty(void) const;

	// return the location after our replacement, or 0 if not found.
	char* replace(const char* oldStr, const char* newStr, char* searchStart);
    bool replaceAll(const char* oldStr, const char* newStr);

    operator const char*(void) const { return getChars(); }
	bool operator < (const GHString& other) const;
	bool operator ==(const GHString& other) const;
	bool operator ==(const char* other) const;
	bool operator !=(const GHString& other) const;
	bool operator !=(const char* other) const;
    
    GHString& operator=(const GHString& rhs);

private:
	void clearNonConstChars(void);
	void transitionToWriteable(void);

private:
	/// A string is expected to have either const chars or chars but not both.
	const char* mConstChars{ 0 };
	char* mChars{ 0 };
	bool mOwnsChars{ false };
	size_t mCharLen{ 0 };
};
