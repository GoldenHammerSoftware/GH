// Copyright Golden Hammer Software
#pragma once

#include <vector>

class GHMD3LoaderConfig;
class GHModel;
class GHTransformAnimController;
class GHStringIdFactory;
struct GHMD3ModelHeader;
class GHTransformNode;
class GHTimeVal;

class GHMD3TagLoader
{
public:
    GHMD3TagLoader(const GHMD3LoaderConfig& config,
                   const GHStringIdFactory& hashTable,
                   const GHTimeVal& time);
    
    void loadTags(const char* bufferStart, GHMD3ModelHeader& md3Header, 
                  GHModel& model, 
                  std::vector<GHTransformAnimController*>& tagAnims,
                  GHTransformAnimController*& centerTag) const;
    
    void centerTags(GHTransformAnimController* centerTag, 
                    std::vector<GHTransformAnimController*>& tagAnims,
					GHMD3ModelHeader& md3Header) const;
    
private:
    void createEmptyTagAnims(GHMD3ModelHeader& md3Header, 
                             std::vector<GHTransformAnimController*>& tagAnims) const;
    const char* loadTag(const char* buffer, GHTransformNode& retval) const;

private:
    const GHMD3LoaderConfig& mConfig;
    const GHStringIdFactory& mIdFactory;
    const GHTimeVal& mTime;
};
