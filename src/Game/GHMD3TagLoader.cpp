// Copyright Golden Hammer Software
#include "GHMD3TagLoader.h"
#include "GHMD3Structs.h"
#include "GHTransformAnimController.h"
#include "GHTransformAnimData.h"
#include "GHString/GHStringIdFactory.h"
#include "GHTransformNode.h"
#include "GHMD3LoaderConfig.h"
#include "GHFrameAnimSet.h"
#include "GHModel.h"
#include <string.h>
#include "GHModelTransformAnimCloner.h"

const float sQ3Scale = 20;

GHMD3TagLoader::GHMD3TagLoader(const GHMD3LoaderConfig& config,
                               const GHStringIdFactory& hashTable,
                               const GHTimeVal& time)
: mConfig(config)
, mIdFactory(hashTable)
, mTime(time)
{
}

void GHMD3TagLoader::loadTags(const char* bufferStart, GHMD3ModelHeader& md3Header, 
                              GHModel& model, 
                              std::vector<GHTransformAnimController*>& tagAnims,
                              GHTransformAnimController*& centerTag) const
{
    if (!md3Header.numtags) return;
	centerTag = 0;
	createEmptyTagAnims(md3Header, tagAnims);
	
	GHTransformNode dummyTag;
    const char* buffer = bufferStart + md3Header.tagsoffset;
	const char* centerTagName = mConfig.mCenterTagName;
    GHIdentifier centerTagId = mIdFactory.generateHash(centerTagName);
    
	for (int frame = 0; frame < md3Header.numframes; ++frame) {
		for (int i = 0; i < md3Header.numtags; ++i) {
			GHTransformNode* tag = &dummyTag;
			if (frame == 0) { tag = new GHTransformNode; }
			
			buffer = loadTag(buffer, *tag);
			if (frame == 0) {
				// this is the first time we've seen the tag,
				//  so use the returned node to store the actual value.
				model.getSkeleton()->addChild(tag);
				if (tagAnims.size()) {
					tagAnims[i]->setTarget(tag);
					model.addTransformAnimController(tagAnims[i], new GHModelTransformAnimCloner(*tagAnims[i]));
					
					if (centerTagName) {
						if (centerTagId == tag->getId()) {
							centerTag = tagAnims[i];
						}
					}
				}
			}
			// store out the frame position for this tag.
			if (tagAnims.size()) {
				tagAnims[i]->getAnimData()->addFrame(tag->getLocalTransform().getMatrix());
			}
		}
	}
}

void GHMD3TagLoader::createEmptyTagAnims(GHMD3ModelHeader& md3Header, 
                                         std::vector<GHTransformAnimController*>& tagAnims) const
{
    if (md3Header.numframes < 2) 
    {
        return;
    }

    // add a dumb anim sequence to the tags that just loops through all.
    GHFrameAnimSet* animSet = new GHFrameAnimSet;
    GHFrameAnimSequence seq;
    seq.mFirstFrame = 0;
    seq.mEndFrame = (unsigned short)(md3Header.numframes-1);
    seq.mLoopFrames = seq.mEndFrame - seq.mFirstFrame;
    seq.mFramesPerSecond = 15;
    seq.mId = mIdFactory.generateHash("default");
    animSet->addSequence(seq);
               
    // read in the tags.
	// the set of tags is duplicated for every frame of animation.
	// create an animation controller for each tag.
    tagAnims.resize(md3Header.numtags);
    for (int i = 0; i < md3Header.numtags; ++i) {
        // create an anim controller for this tag.
        tagAnims[i] = new GHTransformAnimController(mTime);
        // make an animation data struct to hold the pos anim data.
        GHTransformAnimData* animData = new GHTransformAnimData(md3Header.numframes);
        tagAnims[i]->setAnimData(animData);

        tagAnims[i]->setAnimSet(animSet);
        tagAnims[i]->setAnim(seq.mId, 0);
    }
}

const char* GHMD3TagLoader::loadTag(const char* buffer, GHTransformNode& retval) const
{
	char name[64];
	::memset(name, 0, 64);
	::strncpy(name, buffer, 64);
	retval.setId(mIdFactory.generateHash(name));
	buffer += 64;
	
	GHTransform& localTrans = retval.getLocalTransform();
	GHPoint3 translate;
	
	float x,y,z;
	::memcpy(&x, buffer, sizeof(float));
	buffer += sizeof(float);
	::memcpy(&y, buffer, sizeof(float));
	buffer += sizeof(float);
	::memcpy(&z, buffer, sizeof(float));
	buffer += sizeof(float);
	translate.setCoords(x,y,z);
	
	float posDiv = sQ3Scale;
	if (mConfig.mLoadingScale) posDiv = mConfig.mLoadingScale;
	
	for (int i = 0; i < 3; ++i) {
		translate[i] /= posDiv;
	}
	localTrans.setTranslate(translate);
	
	//#define IGNORE_ROTATION_ON_TAGS
#ifdef IGNORE_ROTATION_ON_TAGS
	buffer += 9*4;
#else
	GHPoint16& matrix = localTrans.getMatrix();
	::memcpy(&matrix.getArr()[0], buffer, 12);
	buffer += 12;
	::memcpy(&matrix.getArr()[4], buffer, 12);
	buffer += 12;
	::memcpy(&matrix.getArr()[8], buffer, 12);
	buffer += 12;
#endif
	
	return buffer;
}

void GHMD3TagLoader::centerTags(GHTransformAnimController* centerTag, 
                                std::vector<GHTransformAnimController*>& tagAnims,
                                GHMD3ModelHeader& md3Header) const
{
	if (!centerTag) return;
	GHPoint3 centerPos;
	for (int frame = 0; frame < md3Header.numframes; ++frame) 
	{
		GHPoint16* centerFrame = centerTag->getAnimData()->getFrame(frame);
		centerPos[0] = (*centerFrame)[12];
        centerPos[1] = (*centerFrame)[13];
        centerPos[2] = (*centerFrame)[14];
		
		for (int i = 0; i < md3Header.numtags; ++i)
		{
			GHTransformAnimController* currTag = tagAnims[i];
			if (currTag == centerTag) continue;
			GHPoint16* tagFrame = currTag->getAnimData()->getFrame(frame);
			if (!tagFrame) continue;
			tagFrame[12] -= centerPos[0];
			tagFrame[13] -= centerPos[1];
			tagFrame[14] -= centerPos[2];
		}
	}    
}

