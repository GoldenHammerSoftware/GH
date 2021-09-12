// Copyright Golden Hammer Software
#include "GHTransformAnimHelper.h"
#include "GHMath/GHQuaternion.h"
#include "GHMath/GHTransform.h"
#include "GHFrameAnimSequence.h"
#include "GHPlatform/GHTimeVal.h"
#include "GHTransformAnimData.h"
#include "GHPlatform/GHDebugMessage.h"

bool GHTransformAnimHelper::blendTransform(std::vector<GHFrameAnimBlend>& blends, const GHTimeVal& time,
                                           const GHTransformAnimData& animData, GHPoint16& ret) const
{
    if (!blends.size()) return false;
    if (!blends[0].mSequence) return false;

    GHPoint16 totMat;
    GHTransform::makeIdentity(totMat);
    float totPct = 0;
    for (size_t i = 0; i < blends.size(); ++i) {
        if (blends[i].mBlendPct == 0) {
            continue;
        }
        
        unsigned short lowFrame = 0;
        unsigned short highFrame = 0;
        float pctLow = 1.0f;
        blends[i].mSequence->findBorderFrames(blends[i].mStartTime, time.getTime(), lowFrame, highFrame, pctLow);
		//GHDebugMessage::outputString("Blending %d with %d at %f", lowFrame, highFrame, pctLow);
        
        const GHPoint16* lowMatrix = animData.getFrame(lowFrame);
        const GHPoint16* highMatrix = animData.getFrame(highFrame);
        if (!lowMatrix || !highMatrix) return false;
        GHPoint16 matrixRes;
        blendAnimFrames(*lowMatrix, *highMatrix, pctLow, matrixRes);
        
        if (totPct == 0) {
            totMat = matrixRes;
        }
        else {
            float currPct = totPct + blends[i].mBlendPct;
            float pctMult = 1.0f / currPct;
            float pctTotMat = totPct * pctMult;
            blendAnimFrames(totMat, matrixRes, pctTotMat, totMat);
        }
        totPct += blends[i].mBlendPct;
    }
    
    ret.setCoords(totMat.getArr());
    return true;
}

void GHTransformAnimHelper::blendAnimFrames(const GHPoint16& lowMatrix, const GHPoint16& highMatrix, float pctLow, GHPoint16& matrixRes) const
{
    GHPoint4 lowQuat, highQuat, retQuat;
    GHPoint3 pos;
    for (int i = 0; i < 3; ++i) {
        pos[i] = lowMatrix[12+i] + (1.0f-pctLow)*(highMatrix[12+i]-lowMatrix[12+i]);
    }
    GHQuaternion::convertTransformToQuaternion(lowMatrix, lowQuat);
    GHQuaternion::convertTransformToQuaternion(highMatrix, highQuat);
    GHQuaternion::slerp(lowQuat, highQuat, 1.0f-pctLow, retQuat);
    GHQuaternion::convertQuaternionToTransform(retQuat, matrixRes);
    
    for (int i = 0; i < 3; ++i) {
        matrixRes[12+i] = pos[i];
    }
}
