// Copyright Golden Hammer Software
#include "GHPropertyStringGenerator.h"
#include "GHString/GHStringTokenizer.h"
#include "GHUtils/GHPropertyContainer.h"

namespace GHPropertyString
{
	char* copyStr(char* dest, const char* src, unsigned int &remainingBufferSize)
	{
		const char* srcEnd = (const char*)memchr(src, '\0', remainingBufferSize);

		unsigned int charsToCopy = srcEnd ? (unsigned int)(srcEnd - src) : remainingBufferSize;
		memcpy(dest, src, charsToCopy);
		dest[remainingBufferSize - 1] = '\0';

		remainingBufferSize -= charsToCopy;
		return dest + charsToCopy;
	}
}

GHPropertyStringGenerator::GHPropertyStringGenerator(const GHPropertyContainer& propertyContainer,
                                                     unsigned int outputBufferSize)
: mPropertyContainer(propertyContainer)
, mBufferSize(outputBufferSize)
{
    mOutputBuffer = new char[outputBufferSize];
    mScratchBuffer = new char[outputBufferSize];
}

GHPropertyStringGenerator::~GHPropertyStringGenerator(void)
{
    delete [] mOutputBuffer;
    delete [] mScratchBuffer;
}

const char* GHPropertyStringGenerator::generateString(void)
{
	::memset(mOutputBuffer, 0, mBufferSize);
	::memset(mScratchBuffer, 0, mBufferSize);
    
    std::vector<GHProperty> propVals;
    for (size_t i = 0; i < mPropertyTypes.size(); ++i)
    {
        const GHProperty& val = mPropertyContainer.getProperty(mPropertyTypes[i]);
        propVals.push_back(val);
    }

	char* outputBuffer = mOutputBuffer;
	unsigned int remainingBufferSize = mBufferSize;
	for (unsigned int i = 0; i < mPrefixInsertionStrings.size(); ++i) 
    {
        mScratchBuffer[0] = 0;
		outputBuffer = GHPropertyString::copyStr(outputBuffer, mPrefixInsertionStrings[i], remainingBufferSize);
		if (i < mOrderedPropertyTypes.size()) 
        {
			const char tag = mPropertyInsertionTags[i];
			
			// bleh, don't know how to find a way around the cast, so we need to explicitly support types.
			if (tag == 'd') {
                float fval = (float)propVals[mOrderedPropertyTypes[i]];
                int ival = (int)propVals[mOrderedPropertyTypes[i]];
                fval++; ival++;
				snprintf(mScratchBuffer, mBufferSize, "%d", (int)propVals[mOrderedPropertyTypes[i]]);
			}
			else if (tag == 's') {
				GHProperty& propVal = propVals[mOrderedPropertyTypes[i]];
				const char* propStr = propVal;
				if (propStr) {
                    ::strncpy(mScratchBuffer, propStr, mBufferSize);
				}
			}
			else if (tag == 'f') {
                snprintf(mScratchBuffer, mBufferSize, "%.2f", (float)propVals[mOrderedPropertyTypes[i]]);
			}
			else if (tag == 't') {
				float floatSeconds = (float)propVals[mOrderedPropertyTypes[i]];
				int floorSeconds = (int)floatSeconds;
				float ms = (floatSeconds - (float)floorSeconds)*100.0f;
				int minutes = floorSeconds / 60;
				int seconds = floorSeconds-(minutes*60);
				if (minutes > 99) {
                    ::strncpy(mScratchBuffer, "LONGTIME", mBufferSize);
				}
				else {
					snprintf(mScratchBuffer, mBufferSize, "%02d:%02d:%02d", minutes, seconds, (int)ms);
				}
			}
			else if (tag == 'b') {
				bool val = (bool)propVals[mOrderedPropertyTypes[i]];
				snprintf(mScratchBuffer, mBufferSize, val ? "true" : "false");
			}
		}
		outputBuffer = GHPropertyString::copyStr(outputBuffer, mScratchBuffer, remainingBufferSize);
	} 
    return mOutputBuffer;
}

void GHPropertyStringGenerator::addMonitoredProperty(const GHIdentifier& propId)
{
    mPropertyTypes.push_back(propId);
}

void GHPropertyStringGenerator::setSourceString(const char* str)
{
    mSourceString.setConstChars(str, GHString::CHT_COPY);
    if (!str) return;
    
    // pop off the initial text before the '%' char
	// todo: make this safer for bad input strings.
	char* sourceChars = mSourceString.getNonConstChars();
    char* currChar = sourceChars;
	GHStringTokenizer strTok;
    const char* endChar = sourceChars + mSourceString.getCharLen();
	while(currChar < endChar)
	{
		char* prefixString = 0;
		strTok.popToken(currChar, '%', prefixString, currChar, endChar);
		mPrefixInsertionStrings.push_back(prefixString);
		// store out what kind of variable comes next.
		if (!currChar || *currChar == '\0') {
			break;
		}
		mPropertyInsertionTags.push_back(*currChar);
		currChar++;
		// and store out what variable to throw into that slot.
		if (*currChar == '[') {
			currChar++;
			const char* constCurr = currChar;
			mOrderedPropertyTypes.push_back(strTok.popInt(constCurr, ']', constCurr));
			currChar += (constCurr - currChar);
		}
		else {
			char tempChar = *(currChar+1);
			*(currChar+1) = '\0';
			mOrderedPropertyTypes.push_back(::atoi(currChar));
			*(currChar+1) = tempChar;
			currChar++;
		}
	} 
}
