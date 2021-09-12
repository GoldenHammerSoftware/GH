// Copyright Golden Hammer Software
#include "GHFrameAnimSetXMLSerializer.h"
#include "GHXMLNode.h"
#include "GHFrameAnimSet.h"
#include <stdio.h>

void GHFrameAnimSetXMLSerializer::saveAnimSet(const GHFrameAnimSet& animSet, GHXMLNode& topNode)
{
    GHXMLNode* setNode = new GHXMLNode(0);
    setNode->setName("animSet", GHString::CHT_REFERENCE);
    topNode.addChild(setNode);
    
    const std::vector<GHFrameAnimSequence>& seqs = animSet.getSequences();
    char buf[512];
	for (size_t i = 0; i < seqs.size(); ++i)
    {
        GHXMLNode* seqNode = new GHXMLNode(0);
        seqNode->setName("Seq", GHString::CHT_REFERENCE);
        seqNode->setAttribute("name", GHString::CHT_REFERENCE, seqs[i].mId.getString(), GHString::CHT_REFERENCE);
        sprintf(buf, "%d", seqs[i].mFirstFrame);
        seqNode->setAttribute("start", GHString::CHT_REFERENCE, buf, GHString::CHT_COPY);
        sprintf(buf, "%d", seqs[i].mEndFrame-seqs[i].mFirstFrame);
        seqNode->setAttribute("numFrames", GHString::CHT_REFERENCE, buf, GHString::CHT_COPY);
        sprintf(buf, "%d", seqs[i].mLoopFrames);
        seqNode->setAttribute("loopFrames", GHString::CHT_REFERENCE, buf, GHString::CHT_COPY);
        sprintf(buf, "%f", seqs[i].mFramesPerSecond);
        seqNode->setAttribute("fps", GHString::CHT_REFERENCE, buf, GHString::CHT_COPY);
        setNode->addChild(seqNode);
    }
}
