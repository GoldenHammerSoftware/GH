// Copyright Golden Hammer Software
#include "GHGeometryComparison.h"
#include "GHGeometry.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGeometryRenderable.h"

bool GHGeometryComparison::compareGeometries(GHGeometryRenderable& left, GHGeometryRenderable& right)
{
    if (left.getGeometry().size() != right.getGeometry().size())
    {
        GHDebugMessage::outputString("GEO COMP FAIL: Different number of geos");
        return false;
    }
    for (size_t i = 0; i < left.getGeometry().size(); ++i)
    {
        GHGeometry* srcGeo = left.getGeometry()[i];
        GHGeometry* destGeo = right.getGeometry()[i];
        if (!compareGeo(*srcGeo, *destGeo)) {
            return false;
        }
    }
    return true;
}

bool GHGeometryComparison::compareGeo(GHGeometry& srcGeo, GHGeometry& destGeo)
{
    if (srcGeo.getMaterial() != destGeo.getMaterial())
    {
        GHDebugMessage::outputString("GEO COMP ALERT: Materials are different");
    }
    
    if (!compareStreams(*srcGeo.getVB()->get(), *destGeo.getVB()->get())) {
        return false;
    }

    GHVBBlitterIndex* srcIndex = (GHVBBlitterIndex*)(srcGeo.getVB()->get()->getBlitter()->get());
    GHVBBlitterIndex* destIndex = (GHVBBlitterIndex*)(destGeo.getVB()->get()->getBlitter()->get());
    if (!compareIB(*srcIndex, *destIndex)) {
        return false;
    }

    return true;
}

bool GHGeometryComparison::compareIB(GHVBBlitterIndex& srcIndex, GHVBBlitterIndex& destIndex)
{
    if (srcIndex.getNumIndices() != destIndex.getNumIndices())
    {
        GHDebugMessage::outputString("GEO COMP FAIL: numIndices are different");
    }
    const unsigned short* srcIB = srcIndex.lockReadBuffer();
    const unsigned short* destIB = destIndex.lockReadBuffer();
	for (size_t ibIdx = 0; ibIdx < srcIndex.getNumIndices(); ++ibIdx)
    {
        if (srcIB[ibIdx] != destIB[ibIdx]) {
            GHDebugMessage::outputString("GEO COMP FAIL: ib val is different %d %d", srcIB[ibIdx], destIB[ibIdx]);
            srcIndex.unlockReadBuffer();
            destIndex.unlockReadBuffer();
            return false;
        }
    }
    srcIndex.unlockReadBuffer();
    destIndex.unlockReadBuffer();
    return true;
}

bool GHGeometryComparison::compareStreams(GHVertexBuffer& srcVB, GHVertexBuffer& destVB)
{
    const std::vector<GHVertexStreamPtr*>& srcStreams = srcVB.getStreams();
    const std::vector<GHVertexStreamPtr*>& destStreams = destVB.getStreams();
    if (srcStreams.size() != destStreams.size()) {
        GHDebugMessage::outputString("GEO COMP FAIL: different number of streams");
        return false;
    }
	for (size_t streamIdx = 0; streamIdx < srcStreams.size(); ++streamIdx)
    {
        if (srcStreams[streamIdx]->get()->getNumVerts() != destStreams[streamIdx]->get()->getNumVerts())
        {
            GHDebugMessage::outputString("GEO COMP FAIL: mismatched vert count");
            return false;
        }
        const GHVertexStreamFormat& srcFormat = srcStreams[streamIdx]->get()->getFormat();
        const GHVertexStreamFormat& destFormat = destStreams[streamIdx]->get()->getFormat();
        const std::vector<GHVertexStreamFormat::ComponentEntry>& srcComps = srcFormat.getComponents();
        const std::vector<GHVertexStreamFormat::ComponentEntry>& destComps = destFormat.getComponents();
        if (srcComps.size() != destComps.size()) {
            GHDebugMessage::outputString("GEO COMP FAIL: different stream component count");
            return false;
        }
        for (int compIdx = 0; compIdx != srcComps.size(); ++compIdx) {
            if (srcComps[compIdx].mComponent != destComps[compIdx].mComponent) {
                GHDebugMessage::outputString("GEO COMP FAIL: mismatched vert comp");
                return false;
            }
            if (srcComps[compIdx].mCount != destComps[compIdx].mCount) {
                GHDebugMessage::outputString("GEO COMP FAIL: mismatched vert comp count");
                return false;
            }
            if (srcComps[compIdx].mType != destComps[compIdx].mType) {
                GHDebugMessage::outputString("GEO COMP FAIL: mismatched vert comp type");
                return false;
            }
            if (srcComps[compIdx].mOffset != destComps[compIdx].mOffset) {
                GHDebugMessage::outputString("GEO COMP FAIL: mismatched vert comp offset");
                return false;
            }
        }
        
        const float* srcBuf = srcStreams[streamIdx]->get()->lockReadBuffer();
        const float* destBuf = destStreams[streamIdx]->get()->lockReadBuffer();
		for (size_t vertIdx = 0; vertIdx < srcFormat.getVertexSize()*srcStreams[streamIdx]->get()->getNumVerts(); ++vertIdx)
        {
            if (srcBuf[vertIdx] != destBuf[vertIdx]) {
                GHDebugMessage::outputString("GEO COMP FAIL: vert data mismatch");
                srcStreams[streamIdx]->get()->unlockReadBuffer();
                destStreams[streamIdx]->get()->unlockReadBuffer();
                return false;
            }
        }
        srcStreams[streamIdx]->get()->unlockReadBuffer();
        destStreams[streamIdx]->get()->unlockReadBuffer();
    }
    
    return true;
}
