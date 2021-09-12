// Copyright Golden Hammer Software
#pragma once

#include "GHModelLoader.h"
#include "GHMath/GHRect.h"
#include "stdint.h"
#include "GHMD3LoaderAnimModifierGPU.h"
#include <map>
#include "GHString/GHString.h"

class GHFileOpener;
class GHFile;
class GHModel;
class GHMD3LoaderConfig;
struct GHMD3ModelHeader;
struct GHMD3Frame;
struct GHMD3SurfaceDescription;
class GHTransformAnimController;
class GHGeometryRenderable;
class GHVBFactory;
class GHVertexBuffer;
class GHVBBlitterIndex;
class GHTimeVal;
class GHTransformNode;

/* example
 <md3model file=gothax_lower.md3 meshTop=ModelTop>
    <transformNode id=Legs rotation="-0.5 0 0.5" translate="0 0.28 0" scale=.85/>
 </md3model> 
*/
class GHMD3ModelLoader : public GHModelLoader
{
public:
    GHMD3ModelLoader(const GHXMLObjFactory& objFactory,
                     const GHStringIdFactory& hashTable,
                     const GHFileOpener& fileOpener,
                     GHVBFactory& vbFactory, 
                     const GHTimeVal& time);
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    void loadMD3Config(const GHXMLNode& node, GHMD3LoaderConfig& config) const;
    void loadMD3(GHModel& model, const GHMD3LoaderConfig& config, const char* buffer, GHTransformNode& meshTransform) const;
	
    // load in the header info.
	// returns a pointer to where the buffer ended up.
	const char* loadHeader(const char* buffer, GHMD3ModelHeader& retval) const;
	
    void loadFrames(const char* bufferStart, GHMD3ModelHeader& md3Header) const;
	// load in the info for a frame.
	// returns a pointer to where the buffer ended up.
	const char* loadFrame(const char* buffer, GHMD3Frame& retval) const;
    
	void loadSurfaces(const char* bufferStart, 
                      GHMD3ModelHeader& md3Header, GHModel& model,
					  GHTransformAnimController* centerTag, 
                      const GHMD3LoaderConfig& config,
                      GHTransformNode& meshTransform) const;
	// load in a surface.
	// returns a pointer to where the buffer ended up.
	const char* loadSurface(const char* buffer, 
                            GHModel& retval, GHGeometryRenderable& renderable,
                            GHRect<float,3>& bounds,
							GHTransformAnimController* centerTag,
                            const GHMD3LoaderConfig& config,
                            GHTransformNode& meshTransform) const;
    // load the list of shaders for a surface.
	void loadSurfaceShaders(const char* bufferStart, 
                            const GHMD3SurfaceDescription& surfaceDesc,
                            std::map<long,GHString>& ret) const;
    void loadSurfaceSourceInfo(const char* bufferStart, const GHMD3SurfaceDescription& surfaceDesc,
                               GHPoint3*& tempVerts, GHPoint2*& encodedNormals, uint32_t*& connectivity, 
                               float*& texCoords, GHRect<float,3>& bounds) const;
    const char* loadSurfaceFrameSourceInfo(const char* bufferStart, 
										   const GHMD3SurfaceDescription& surfaceDesc,
										   GHPoint3* tempVerts, GHPoint2* encodedNormals, 
										   GHRect<float,3>& bounds, const GHPoint3* centerTagPos,
                                           const GHMD3LoaderConfig& config) const;
    
    void fillSurfaceFrameVB(GHVertexBuffer& vb, 
                            const GHMD3SurfaceDescription& surfaceDesc, 
                            const GHPoint3* tempVerts, const GHPoint2* encodedNormals, 
                            const float* texCoords) const;
	void fillSurfaceFrameIB(GHVBBlitterIndex& ib, const GHMD3SurfaceDescription& surfaceDesc, 
                            const uint32_t* connectivity) const;
    GHFrameAnimControllerVB* createSurfaceAnimController(const GHMD3SurfaceDescription& surfaceDesc) const;

private:
    const GHFileOpener& mFileOpener;
    GHMD3LoaderAnimModifierGPU mAnimModifier;
    GHVBFactory& mVBFactory;
    const GHTimeVal& mTime;
};
