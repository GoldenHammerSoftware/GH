// Copyright Golden Hammer Software
#include "GHOptionsSerializer.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHUtils/GHEventMgr.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHXMLSerializer.h"
#include "GHXMLNode.h"
#include "GHXMLObjFactory.h"
#include "GHPropertyXMLSaver.h"
#include "GHUtils/GHMessage.h"
#include "GHXMLObjLoader.h"

GHOptionsSerializer::GHOptionsSerializer(GHPropertyContainer& props, GHEventMgr& eventMgr,
                                   const GHXMLSerializer& xmlSerializer,
                                   const GHXMLObjFactory& objFactory,
								   const GHIdentifier& saveMessageId,
								   const GHIdentifier& resetMessageId,
								   const char* saveFilename)
: mProps(props)
, mEventMgr(eventMgr)
, mOptionsListener(*this)
, mXMLSerializer(xmlSerializer)
, mObjFactory(objFactory)
, mSaveMessageId(saveMessageId)
, mResetMessageId(resetMessageId)
, mSaveFilename(saveFilename, GHString::CHT_COPY)
{
    mEventMgr.registerListener(mSaveMessageId, mOptionsListener);
	mEventMgr.registerListener(mResetMessageId, mOptionsListener);
}

GHOptionsSerializer::~GHOptionsSerializer(void)
{
    mEventMgr.unregisterListener(mSaveMessageId, mOptionsListener);
	mEventMgr.unregisterListener(mResetMessageId, mOptionsListener);

	for (size_t i = 0; i < mOptions.size(); ++i)
	{
		delete mOptions[i].mPropSaver;
	}
}

void GHOptionsSerializer::setDefaults(bool initialSet)
{
	for (unsigned int i = 0; i < mOptions.size(); ++i)
	{
		if (initialSet || mOptions[i].mCanBeReset) {
			mProps.setProperty(mOptions[i].mId, mOptions[i].mDefault);
		}
	}
}

void GHOptionsSerializer::saveOptions(void)
{
    GHXMLNode topNode;
    topNode.setName("properties", GHString::CHT_REFERENCE);

	for (unsigned int i = 0; i < mOptions.size(); ++i)
	{
		if (mOptions[i].mPropSaver)
		{
			GHPropertyXMLSaver::saveProp(topNode, mProps, mOptions[i].mId, mOptions[i].mSaveName.getChars(), mOptions[i].mPropTypeName.getChars(), *mOptions[i].mPropSaver);
		}
		else
		{
			// it is safe to save bools etc as floats.
			GHPropertyXMLSaver::saveFloatProp(topNode, mProps, mOptions[i].mId, mOptions[i].mSaveName.getChars());
		}
	}

    mXMLSerializer.writeXMLFile(mSaveFilename.getChars(), topNode);
}

void GHOptionsSerializer::loadOptions(void)
{
    GHPropertyContainer* loadedProps = (GHPropertyContainer*)mObjFactory.load(mSaveFilename.getChars());
    if (!loadedProps) {
        saveOptions();
        return;   
    }

	for (unsigned int i = 0; i < mOptions.size(); ++i)
	{
		loadProperty(loadedProps, mOptions[i].mId);
	}

    delete loadedProps;
}

void GHOptionsSerializer::loadProperty(const GHPropertyContainer* loadedProps, const GHIdentifier& index)
{
    const GHProperty& prop = loadedProps->getProperty(index);
    if (prop.isValid())
    {
        mProps.setProperty(index, prop);
    }
}

void GHOptionsSerializer::addOption(const GHIdentifier& id, const GHProperty& defaultProp, const GHString& saveName, bool canBeReset, const GHString* propTypeName, GHXMLObjLoader* propSaver)
{
	Entry newEntry;
	newEntry.mId = id;
	newEntry.mDefault = defaultProp;
	newEntry.mSaveName = saveName;
	newEntry.mCanBeReset = canBeReset;
	newEntry.mPropSaver = propSaver;
	if (propTypeName)
	{
		newEntry.mPropTypeName = *propTypeName;
	}
	mOptions.push_back(newEntry);
}

GHOptionsSerializer::OptionsListener::OptionsListener(GHOptionsSerializer& options)
: mOptions(options)
{
}

void GHOptionsSerializer::OptionsListener::handleMessage(const GHMessage& message)
{
	if (message.getType() == mOptions.mSaveMessageId) {
		mOptions.saveOptions();
	}
	else if (message.getType() == mOptions.mResetMessageId) {
		mOptions.setDefaults(false);
	}
}
