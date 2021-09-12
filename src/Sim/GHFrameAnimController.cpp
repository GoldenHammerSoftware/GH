// Copyright Golden Hammer Software
#include "GHFrameAnimController.h"
#include "GHFrameAnimSet.h"
#include "GHPlatform/GHDebugMessage.h"

GHFrameAnimController::GHFrameAnimController(void)
: mAnimSet(0)
{
}

GHFrameAnimController::~GHFrameAnimController(void)
{
    GHRefCounted::changePointer((GHRefCounted*&)mAnimSet, 0);
}

void GHFrameAnimController::setAnim(const GHIdentifier& id, float startTime)
{
    if (!mAnimSet) {
        GHDebugMessage::outputString("setAnim called with no anim set");
        mAnimBlends.resize(0);
        return;
    }
    GHFrameAnimSequence* newSeq = mAnimSet->getSequence(id);
    if (newSeq) {
        mAnimBlends.resize(1);
        mAnimBlends[0].mSequence = newSeq;
        mAnimBlends[0].mStartTime = startTime;
        mAnimBlends[0].mBlendPct = 1;
    } else {
        // this is ok, we might pass a bogus anim to disable animations.
        //GHDebugMessage::outputString("setAnim called with sequence not found");
        mAnimBlends.resize(0);
    }
}

void GHFrameAnimController::setAnimBlend(const std::vector<AnimBlend>& blends)
{
    if (!mAnimSet) {
        GHDebugMessage::outputString("setAnimBlend called with no anim set");
        return;
    }
    mAnimBlends.resize(0);
    float totPct = 0;
	for (size_t i = 0; i < blends.size(); ++i)
    {
        GHFrameAnimSequence* newSeq = mAnimSet->getSequence(blends[i].mSequenceId);
        if (newSeq) {
            GHFrameAnimBlend blend;
            blend.mSequence = newSeq;
            blend.mStartTime = blends[i].mStartTime;
            blend.mBlendPct = blends[i].mBlendPct;
            mAnimBlends.push_back(blend);
            totPct += blends[i].mBlendPct;
        } else {
            GHDebugMessage::outputString("setAnim called with sequence not found");
        }
    }
    // normalize the blend pct.
    if (totPct != 0 && totPct != 1.0)
    {
        float mult = 1.0f / totPct;
		for (size_t i = 0; i < mAnimBlends.size(); ++i) {
            mAnimBlends[i].mBlendPct *= mult;
        }
    }
}

void GHFrameAnimController::setAnimSet(GHFrameAnimSet* set)
{
    GHRefCounted::changePointer((GHRefCounted*&)mAnimSet, set);
    if (mAnimSet) {
        if (mAnimSet->getSequences().size()) {
            mAnimBlends.resize(1);
            mAnimBlends[0].mSequence = &mAnimSet->getSequences()[0];
            mAnimBlends[0].mStartTime = 0;
            mAnimBlends[0].mBlendPct = 1;
        }
    }
}
