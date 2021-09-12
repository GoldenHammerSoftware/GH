// Copyright Golden Hammer Software
#include "GHGPUVertexAnimController.h"
#include "GHVBAnimData.h"
#include "GHPlatform/GHTimeVal.h"
#include "GHGeometryRenderable.h"
#include "GHGeometry.h"
#include "GHVertexStreamOverride.h"
#include "GHPlatform/GHDebugMessage.h"

GHGPUVertexAnimController::GHGPUVertexAnimController(unsigned int lowStreamTarget, unsigned int highStreamTarget,
                                                     unsigned int sourceStreamId, const GHIdentifier& interpPctProp,
                                                     const GHTimeVal& time)
: mLowTarget(lowStreamTarget)
, mHighTarget(highStreamTarget)
, mSourceStreamId(sourceStreamId)
, mInterpPctProp(interpPctProp)
, mTargetProperties(0)
, mTime(time)
{
    // we push the second pos into diffuse and the second normal into specular
    GHVertexStreamFormat* repFormat = new GHVertexStreamFormat();
    repFormat->addComponent(GHVertexComponentShaderID::SI_DIFFUSE, GHVertexComponentType::CT_FLOAT, 3, 0);
//    repFormat->addComponent(GHVertexComponentShaderID::SI_SPECULAR, GHVertexComponentType::CT_UBYTE, 2, 0);
//    repFormat->setVertexSize(4);
    repFormat->addComponent(GHVertexComponentShaderID::SI_SPECULAR, GHVertexComponentType::CT_FLOAT, 3, 0);
    repFormat->setVertexSize(6);
    GHVertexStreamOverride* repOverride = new GHVertexStreamOverride(new GHVertexStreamFormatPtr(repFormat));
    mReplacementStream = new GHVertexStreamPtr(repOverride);
    mReplacementStream->acquire();
}

GHGPUVertexAnimController::~GHGPUVertexAnimController(void)
{
    GHRefCounted::changePointer((GHRefCounted*&)mReplacementStream, 0);
}

void GHGPUVertexAnimController::setTarget(GHGeometry* target)
{
    GHFrameAnimControllerVB::setTarget(target);
    if (!target) return;
    mTargetProperties = &target->getProperties();
}

void GHGPUVertexAnimController::update(void)
{
    if (!mAnimData) return;
	if (!mTarget) return;
	if (!mFrameChecker.checkIsNewFrame(mTime)) return;
    if (!mAnimBlends.size() || !mAnimBlends[0].mSequence) return;
    if (mAnimBlends.size() > 1) {
        GHDebugMessage::outputString("Vertex Anim blend not supported.");
    }
    
	unsigned short lowFrame = 0;
	unsigned short highFrame = 0;
	float pctLow = 1.0f;
	mAnimBlends[0].mSequence->findBorderFrames(mAnimBlends[0].mStartTime, mTime.getTime(), lowFrame, highFrame, pctLow);

    GHVertexBufferPtr* lowBuffer = mAnimData->getFrame(lowFrame);
    GHVertexBufferPtr* highBuffer = mAnimData->getFrame(highFrame);
    if (!lowBuffer || !highBuffer) return;
    
    GHVertexStreamPtr* lowStream = lowBuffer->get()->getStream(mSourceStreamId);
    GHVertexStreamPtr* highStream = highBuffer->get()->getStream(mSourceStreamId);
    if (!lowStream || !highStream) return;
    
    mTarget->getVB()->get()->replaceStream(mLowTarget, lowStream);
    
    GHVertexStreamOverride* repOverride = (GHVertexStreamOverride*)mReplacementStream->get();
    repOverride->setSourceStream(highStream);
    mTarget->getVB()->get()->replaceStream(mHighTarget, mReplacementStream);
    
    mTargetProperties->setProperty(mInterpPctProp, 1.0f-pctLow);
}

GHAnimController* GHGPUVertexAnimController::clone(void) const
{
    GHGPUVertexAnimController* ret = new GHGPUVertexAnimController(mLowTarget, mHighTarget, mSourceStreamId, mInterpPctProp, mTime);
    ret->setAnimData(mAnimData);
    ret->setAnimSet(mAnimSet);

    return ret;
}
