// Copyright Golden Hammer Software
#include "GHFrameAnimSetLoader.h"
#include "GHXMLNode.h"
#include "GHFrameAnimSet.h"
#include "GHString/GHStringIdFactory.h"

GHFrameAnimSetLoader::GHFrameAnimSetLoader(const GHStringIdFactory& hashTable)
: mIdFactory(hashTable)
{
}

void* GHFrameAnimSetLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHFrameAnimSet* ret = new GHFrameAnimSet;
    
    const std::vector<GHXMLNode*> children = node.getChildren();
    for (size_t i = 0; i < children.size(); ++i)
    {
        GHFrameAnimSequence seq;
        children[i]->readAttrUShort("start", seq.mFirstFrame);
        unsigned short numFrames = 0;
        children[i]->readAttrUShort("numFrames", numFrames);
        seq.mEndFrame = seq.mFirstFrame + numFrames;
        children[i]->readAttrInt("loopFrames", seq.mLoopFrames);
        children[i]->readAttrFloat("fps", seq.mFramesPerSecond);
        const char* nameStr = children[i]->getAttribute("name");
        seq.mId = mIdFactory.generateHash(nameStr);
        
        ret->addSequence(seq);
    }

    return ret;
}
