// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHMessageHandler.h"
#include "GHUtils/GHProperty.h"
#include "GHString/GHString.h"
#include "GHString/GHIdentifier.h"
#include <vector>

class GHPropertyContainer;
class GHEventMgr;
class GHXMLSerializer;
class GHXMLObjFactory;
class GHXMLObjLoader;

// class for setting defaults, loading, and saving of options properties
class GHOptionsSerializer
{
private:
    class OptionsListener : public GHMessageHandler
    {
    public:
        OptionsListener(GHOptionsSerializer& options);
        virtual void handleMessage(const GHMessage& message);
        
    private:
        GHOptionsSerializer& mOptions;
    };
    
public:
    GHOptionsSerializer(GHPropertyContainer& props, GHEventMgr& eventMgr,
						const GHXMLSerializer& xmlSerializer,
						const GHXMLObjFactory& objFactory,
						const GHIdentifier& saveMessageId,
						const GHIdentifier& resetMessageId,
						const char* saveFilename);
    virtual ~GHOptionsSerializer(void);
    
    void setDefaults(bool initialSet);
    void saveOptions(void);
    void loadOptions(void);
    
	void addOption(const GHIdentifier& id, const GHProperty& defaultProp, const GHString& saveName, bool canBeReset=true, const GHString* propTypeName=0, GHXMLObjLoader* propSaver=0);

private:
    void loadProperty(const GHPropertyContainer* loadedProps, const GHIdentifier& index);
    
private:
    GHEventMgr& mEventMgr;
    GHPropertyContainer& mProps;
    const GHXMLObjFactory& mObjFactory;
    OptionsListener mOptionsListener;
    const GHXMLSerializer& mXMLSerializer;
	const GHIdentifier& mSaveMessageId;
	const GHIdentifier& mResetMessageId;
	GHString mSaveFilename;

	struct Entry {
		GHIdentifier mId;
		GHProperty mDefault;
		GHString mSaveName;
		GHString mPropTypeName;
		GHXMLObjLoader* mPropSaver;
		bool mCanBeReset;
	};
	std::vector<Entry> mOptions;
};
