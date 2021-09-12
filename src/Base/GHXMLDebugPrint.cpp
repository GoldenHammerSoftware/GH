#include "GHXMLDebugPrint.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHPlatform/GHFile.h"
#include "GHXMLSerializer.h"
#include "GHPlatform/GHDebugMessage.h"

//Todo: promote this to a public class?
class GHDebugPrintFile : public GHFile
{
public:
	virtual ~GHDebugPrintFile(void)
	{
		closeFile();
	}

	virtual bool closeFile(void)
	{
		mBuffer.push_back('\0');
		char* token = strtok(mBuffer.data(), "\n");
		do
		{
			GHDebugMessage::outputString(token);
		} while ((token = strtok(NULL, "\n")));
		mBuffer.clear();
		return true;
	}

	virtual bool readIntoBuffer(void)
	{
		return false;
	}

	virtual bool getFileBuffer(char*& buffer, size_t& bufferSize)
	{
		buffer = 0;
		bufferSize = 0; 
		return false;
	}

	virtual void releaseFileBuffer(void)
	{

	}

	virtual bool writeBuffer(const void* buffer, size_t bufferSize)
	{
		size_t oldSize = mBuffer.size();
		mBuffer.resize(mBuffer.size() + bufferSize);
		memcpy(mBuffer.data() + oldSize, buffer, bufferSize);
		return true;
	}

private:
	std::vector<char> mBuffer;
};

class GHDebugPrintFileOpener : public GHFileOpener
{
protected:
	virtual GHFile* openPlatformFile(const char* filename, GHFile::FileType fileType, GHFile::FileMode fileMode) const
	{
		if (fileType != GHFile::FT_TEXT)
		{
			GHDebugMessage::outputString("Error: GHDebugPrintFiles only work with GHFile::FT_TEXT");
			return nullptr;
		}

		if (fileMode != GHFile::FM_WRITEONLY)
		{
			GHDebugMessage::outputString("Error: GHDebugPrintFiles only work with GHFile::FM_WRITEONLY");
			return nullptr;
		}

		return new GHDebugPrintFile;
	}
};

void GHXMLDebugPrint::print(const GHXMLNode& node)
{
	GHDebugPrintFileOpener debugPrinter;
	GHXMLSerializer xmlSerializer(debugPrinter);
	xmlSerializer.writeXMLFile("", node);
}
