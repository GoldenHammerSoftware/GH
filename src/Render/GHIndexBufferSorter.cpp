// Copyright 2010 Golden Hammer Software
#include "GHIndexBufferSorter.h"
#include "GHVBBlitter.h"
#include "GHVertexStream.h"
#include "GHPlatform/GHDebugMessage.h"
#include <algorithm>

namespace {
    class Sorter
	{
	public:
		bool operator()(const std::pair<unsigned int, float>& first, const std::pair<unsigned int, float>& second) const
		{
			if (first.second != second.second) {
				return first.second < second.second;
			}
			return first.first < second.first;
		}
	};
}

void GHIndexBufferSorter::sortIB(GHVBBlitterIndex& ib, GHVertexStream& posStream, const GHPoint3& targetPoint)
{
	// first verify the pos stream has a pos component.
	const GHVertexStreamFormat::ComponentEntry* posComp = posStream.getFormat().getComponentEntry(GHVertexComponentShaderID::SI_POS);
	if (!posComp) {
		GHDebugMessage::outputString("failed to find pos comp for IB sort");
		return;
	}

	unsigned int numIndices = ib.getNumIndices();
	unsigned int numTris = numIndices / 3;
	std::vector< std::pair<unsigned int, float> > tris;
	tris.resize(numTris);

	// go through and calculate the distance for each triangle.
	const unsigned short* ibBuf = ib.lockReadBuffer();
	const float* posBuf = posStream.lockReadBuffer();
	for (int i = 0; i < numTris; ++i) 
	{
		float dist = 0;
		for (int v = 0; v < 3; ++v)
		{
			unsigned short vertId = *ibBuf;
			ibBuf++;

			const float* vertStart = posBuf + posStream.getFormat().getVertexSize() * vertId;
			const float* posStart = vertStart + posComp->mOffset;
			GHPoint3 pos(*posStart, *(posStart + 1), *(posStart + 2));
			pos -= targetPoint;
			dist += pos.length();
		}
		tris[i].first = i;
		tris[i].second = dist;
	}
	ib.unlockReadBuffer();

	// sort the tris front to back.
	std::sort(tris.begin(), tris.end(), Sorter());

	// create a new buffer in the new order.
	unsigned short* rBuf = ib.lockWriteBuffer();
	unsigned short* oldBuf = new unsigned short[ib.getNumIndices()];
	memcpy(oldBuf, rBuf, ib.getNumIndices()*sizeof(unsigned short));
	for (int i = 0; i < numTris; ++i)
	{
		unsigned short* triStart = oldBuf + tris[i].first * 3;
		memcpy(rBuf, triStart, 3 * sizeof(unsigned short));
		rBuf += 3;
	}
	ib.unlockWriteBuffer();
	delete[] oldBuf;
	posStream.unlockReadBuffer();
}
