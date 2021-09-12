// Copyright Golden Hammer Software
#include "GHMD3ModelLoader.h"
#include "GHModel.h"
#include "GHXMLNode.h"
#include "GHXMLObjFactory.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHPlatform/GHFile.h"
#include "GHMD3Structs.h"
#include "GHMD3TagLoader.h"
#include "GHMD3LoaderConfig.h"
#include "GHGeometryRenderable.h"
#include "GHTransformAnimController.h"
#include "GHTransformAnimData.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHFrameAnimControllerVB.h"
#include "GHVBAnimData.h"
#include "GHGeometry.h"
#include "GHVBFactory.h"
#include "GHMath/GHPi.h"
#include "GHFrameAnimSet.h"
#include "GHMath/GHFloat.h"
#include "GHEntityHashes.h"
#include "GHModelFrameAnimVBAnimCloner.h"

const float sQ3Scale = 20;

GHMD3ModelLoader::GHMD3ModelLoader(const GHXMLObjFactory& xmlFactory,
                                   const GHStringIdFactory& hashTable,
                                   const GHFileOpener& fileOpener,
                                   GHVBFactory& vbFactory,
                                   const GHTimeVal& time)
: GHModelLoader(xmlFactory, hashTable)
, mFileOpener(fileOpener)
, mVBFactory(vbFactory)
, mAnimModifier(hashTable.generateHash("interppct"), time)
, mTime(time)
{
}

void* GHMD3ModelLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHModel* ret = new GHModel;
    loadTransformNode(*ret, node, true);

    GHMD3LoaderConfig config;
    loadMD3Config(node, config);
    
    GHTransformNode* meshTransform = ret->getSkeleton();
    assert(meshTransform);
    const char* meshTopName = node.getAttribute("meshTop");
    if (meshTopName) {
        GHIdentifier meshTopId = mIdFactory.generateHash(meshTopName);
        GHTransformNode* testNode = meshTransform->findChild(meshTopId);
        if (testNode) {
            meshTransform = testNode;
        }
    }

    void* prevModel = (void*)extraData.getProperty(GHEntityHashes::MODEL);
    extraData.setProperty(GHEntityHashes::MODEL, ret);
    
    const char* md3fileStr = node.getAttribute("file");
    GHFile* file = mFileOpener.openFile(md3fileStr, GHFile::FT_BINARY, GHFile::FM_READONLY);
    if (file)
    {
        size_t bufferSize = 0;
        char* bufferStart = 0;
        if (file->readIntoBuffer() && file->getFileBuffer(bufferStart, bufferSize)) {
            const char* buffer = bufferStart;
            loadMD3(*ret, config, buffer, *meshTransform);
        }
        delete file;
    }
	else
	{
		GHDebugMessage::outputString("Failed to find md3 file %s", md3fileStr);
	}
    
    loadChildren(*ret, node);
    loadMatReplace(*ret, node);
    loadAnimSet(*ret, node);
    loadPhysics(*ret, node, extraData);
    
    extraData.setProperty(GHEntityHashes::MODEL, prevModel);

    return ret;
}

void GHMD3ModelLoader::loadMD3Config(const GHXMLNode& node, GHMD3LoaderConfig& config) const
{
    const GHXMLNode* configNode = node.getChild("md3LoaderArgs", false);
    if (!configNode) {
        return;
    }
    configNode->readAttrFloat("loadingScale", config.mLoadingScale);
    // todo: mCenterTagName
}

void GHMD3ModelLoader::loadMD3(GHModel& model, const GHMD3LoaderConfig& config, 
                               const char* buffer, GHTransformNode& meshTransform) const
{
  	// load in the header info.
	GHMD3ModelHeader md3Header;
	loadHeader(buffer, md3Header);
    
	loadFrames(buffer, md3Header);
    
	GHMD3TagLoader tagLoader(config, mIdFactory, mTime);
    std::vector<GHTransformAnimController*> tagAnims;
    GHTransformAnimController* centerTag = 0;
	tagLoader.loadTags(buffer, md3Header, model, tagAnims, centerTag);
	loadSurfaces(buffer, md3Header, model, centerTag, config, meshTransform);
	tagLoader.centerTags(centerTag, tagAnims, md3Header);
}

const char* GHMD3ModelLoader::loadHeader(const char* buffer, GHMD3ModelHeader& retval) const
{
	::memcpy(&retval, buffer, sizeof(GHMD3ModelHeader));
	buffer += sizeof(GHMD3ModelHeader);
	return buffer;
}

void GHMD3ModelLoader::loadFrames(const char* bufferStart, GHMD3ModelHeader& md3Header) const
{
	// todo: do something with the frame info instead of throwing it away.
	// read in the frames. this is just bounding box info essentially.
	const char* buffer = bufferStart + md3Header.framesoffset;
	GHMD3Frame frame;
	for (int i = 0; i < md3Header.numframes; ++i) {
		buffer = loadFrame(buffer, frame);
	}
}

const char* GHMD3ModelLoader::loadFrame(const char* buffer, GHMD3Frame& retval) const
{
	retval.minBounds.setCoords( ((float*)buffer)[0], ((float*)buffer)[2], ((float*)buffer)[1] );
	buffer += 3*sizeof(float);
	retval.maxBounds.setCoords( ((float*)buffer)[0], ((float*)buffer)[2], ((float*)buffer)[1] );
	buffer += 3*sizeof(float);
	retval.localOrigin.setCoords( ((float*)buffer)[0], ((float*)buffer)[2], ((float*)buffer)[1] );
	buffer += 3*sizeof(float);
    
	retval.radius = *((float*)buffer);
	buffer += 4;
    
	::strncpy(retval.name, buffer, 16);
	buffer += 16;
    
	return buffer;
}

void GHMD3ModelLoader::loadSurfaces(const char* bufferStart, 
                                    GHMD3ModelHeader& md3Header, GHModel& model,
                                    GHTransformAnimController* centerTag,
                                    const GHMD3LoaderConfig& config,
                                    GHTransformNode& meshTransform) const
{
    if (!md3Header.numsurfaces) return;

    GHGeometryRenderable* renderable = new GHGeometryRenderable();
    model.setRenderable(renderable);
    
	GHRect<float,3> realBoundRect;
	realBoundRect.mMin.setCoords((float)INT_MAX, (float)INT_MAX, (float)INT_MAX);
	realBoundRect.mMax.setCoords((float)-INT_MAX, (float)-INT_MAX, (float)-INT_MAX);
    
	// read in the surfaces.  these are the render primitives.
	const char* buffer = bufferStart + md3Header.surfacesoffset;
	for (int i = 0; i < md3Header.numsurfaces; ++i) 
    {
		buffer = loadSurface(buffer, model, *renderable, realBoundRect, centerTag, config, meshTransform);
	}
    
    // create a culling bounds out of the bound rect and give it to the model.
    GHSphere sphere;
    sphere.fromAABB(realBoundRect);
    GHSphereBounds sphereBounds;
    sphereBounds.setSphere(sphere);
    sphereBounds.setTransform(&meshTransform);
    model.addBounds(sphereBounds);
}

const char* GHMD3ModelLoader::loadSurface(const char* buffer, 
                                          GHModel& retval, GHGeometryRenderable& renderable,
                                          GHRect<float,3>& bounds,
                                          GHTransformAnimController* centerTag,
                                          const GHMD3LoaderConfig& config,
                                          GHTransformNode& meshTransform) const
{
    const char* bufferStart = buffer;

    GHGeometry* geom = new GHGeometry;
    geom->setTransform(&meshTransform);
    renderable.addGeometry(geom);
    
	GHMD3SurfaceDescription surfaceDesc;
	::memcpy(&surfaceDesc, buffer, sizeof(GHMD3SurfaceDescription));
	//buffer += sizeof(GHMD3SurfaceDescription);
    
	mAnimModifier.modifyRootGeom(*geom, surfaceDesc);
    //GHDebugMessage::outputString("MD3 Surface: %s", surfaceDesc.surfacename);
    geom->setId(mIdFactory.generateHash(surfaceDesc.surfacename));
    
    //std::map<long,GHString> shaderMap;
	//loadSurfaceShaders(bufferStart, surfaceDesc, shaderMap);
    
	GHPoint3* tempVerts = 0;
	GHPoint2* encodedNormals = 0;
	uint32_t* connectivity = 0;
	float* texCoords = 0;
	loadSurfaceSourceInfo(bufferStart, surfaceDesc, tempVerts, encodedNormals, connectivity, 
                          texCoords, bounds);
    
    GHVBDescription* vbDesc = mAnimModifier.createVBDescription(surfaceDesc);
    
	// create the main vb, the one that actually gets drawn.
	// if there's only one frame, then this is all that gets created.
	GHVertexBuffer* vb = mVBFactory.createVB(*vbDesc);
	if (!vb) {
		GHDebugMessage::outputString("Unable to create vb for md3.\n");
        delete vbDesc;
		return 0;
	}
    
	geom->setVB(new GHVertexBufferPtr(vb));
    
	GHPoint3 centerTagBuf(0,0,0);
	GHPoint3* centerTagPos = 0;
	if (centerTag) {
		GHPoint16* tagFrame = centerTag->getAnimData()->getFrame(0);
		if (tagFrame) {
			centerTagPos = &centerTagBuf;
			centerTagBuf.setCoords(tagFrame->getCoords()+12);
		}
	}
	// grab the main buffer information.
	const char* vertbuffer = bufferStart + surfaceDesc.vertexoffset;
	/*vertbuffer = */ loadSurfaceFrameSourceInfo(vertbuffer, surfaceDesc, tempVerts, encodedNormals,
                                            bounds, centerTagPos, config);		
	fillSurfaceFrameVB(*vb, surfaceDesc, tempVerts, encodedNormals, texCoords);
    
    if (vb->getBlitter() && vb->getBlitter()->get()->getType() == GHVBBlitter::BT_INDEX)
    {
		fillSurfaceFrameIB(*((GHVBBlitterIndex*)vb->getBlitter()->get()), surfaceDesc, connectivity);
    }
    
	// grab the anim frame info.
	if (surfaceDesc.numframes > 1) 
    {
        GHVBDescription* animVBDesc = mAnimModifier.createAnimInfoVBDescription(surfaceDesc);
        
		GHFrameAnimControllerVB* animController = createSurfaceAnimController(surfaceDesc);
		if (animController) {
            animController->setTarget(geom);
			retval.addGeometryAnimController(animController, new GHModelFrameAnimVBAnimCloner(*animController));
		}
        
		const char* frameVertbuffer = bufferStart + surfaceDesc.vertexoffset;
		for (int frameid = 0; frameid < surfaceDesc.numframes; ++frameid) 
		{
			if (centerTag) {
				GHPoint16* tagFrame = centerTag->getAnimData()->getFrame(frameid);
				if (tagFrame) {
					centerTagPos = &centerTagBuf;
					centerTagBuf.setCoords(tagFrame->getCoords()+12);
				}
			}
			
			frameVertbuffer = loadSurfaceFrameSourceInfo(frameVertbuffer, surfaceDesc, tempVerts, encodedNormals, bounds, 
                                                    centerTagPos, config);		
            
			GHVertexBuffer* frameVB = mVBFactory.createVB(*animVBDesc);
            //mAnimModifier.createAnimFrameVB(animInfoDesc, mVBFactory);
			if (!frameVB) { 
				continue; 
			}
            
			fillSurfaceFrameVB(*frameVB, surfaceDesc, tempVerts, encodedNormals, texCoords);
			
			animController->getAnimData()->addFrame(new GHVertexBufferPtr(frameVB));
		}
	}
    
	// cleanup.
	delete [] tempVerts;
	delete [] encodedNormals;
	delete [] connectivity;
    delete vbDesc;
    // texCoords just points into the main buffer, so is not deleted here.
    
	return bufferStart + surfaceDesc.end;
}

void GHMD3ModelLoader::loadSurfaceShaders(const char* bufferStart, 
										  const GHMD3SurfaceDescription& surfaceDesc,
                                          std::map<long,GHString>& ret) const
{
    char shaderName[64];
    const char* buffer = bufferStart + surfaceDesc.shadersoffset;
    shaderName[0] = '\0';
    // what does having more than one shader here mean?
    // the format supports it, so it probably makes sense somehow.
    for (int i = 0; i < surfaceDesc.numshaders; ++i) {
        ::strncpy(shaderName, buffer, 64);
        buffer += 64;
        long shaderIndex = *((long*)buffer);
        buffer += 4; 
        GHString nameString(shaderName, GHString::CHT_COPY);
        ret[shaderIndex] = nameString;
    }
}

void GHMD3ModelLoader::loadSurfaceSourceInfo(const char* bufferStart, const GHMD3SurfaceDescription& surfaceDesc,
                                             GHPoint3*& tempVerts, GHPoint2*& encodedNormals, uint32_t*& connectivity, 
                                             float*& texCoords, GHRect<float,3>& bounds) const
{
	tempVerts = new GHPoint3[surfaceDesc.numverts];
	encodedNormals = new GHPoint2[surfaceDesc.numverts];
	connectivity = new uint32_t[surfaceDesc.numtriangles*3];
    
	const char* buffer = bufferStart + surfaceDesc.trianglesoffset;
	::memcpy(connectivity, buffer, 3*surfaceDesc.numtriangles*4);
    
	buffer = bufferStart + surfaceDesc.stoffset;
	texCoords = (float*)buffer;
    
	// verts must be loaded on a per frame basis with loadSurfaceFrameSourceInfo
}

const char* GHMD3ModelLoader::loadSurfaceFrameSourceInfo(const char* buffer, 
                                                         const GHMD3SurfaceDescription& surfaceDesc,
                                                         GHPoint3* tempVerts, GHPoint2* encodedNormals, 
                                                         GHRect<float,3>& bounds, const GHPoint3* centerTagPos,
                                                         const GHMD3LoaderConfig& config) const
{
	// (1/64) to decode the float16, /q3scale to get closer to a scale of 1 unit = 1 meter.
	float md3mult = (1.0f/64.0f);
	if (config.mLoadingScale) md3mult /= config.mLoadingScale;
	else md3mult /= sQ3Scale;
	
	for (int i = 0; i < surfaceDesc.numverts; ++i) {
		short x = *((short*)buffer);
		buffer++; buffer++;
		short y = *((short*)buffer);
		buffer++; buffer++;
		short z = *((short*)buffer);
		buffer++; buffer++;
		
		float fx = (float)x*md3mult;
		float fy = (float)y*md3mult;
		float fz = (float)z*md3mult;
		
		if (centerTagPos) {
			fx -= (*centerTagPos)[0];
			fy -= (*centerTagPos)[1];
			fz -= (*centerTagPos)[2];
		}
        
		tempVerts[i][0] = fx;
		tempVerts[i][1] = fy;
		tempVerts[i][2] = fz;
		
		bounds.checkExpandBounds(tempVerts[i]);
		
		unsigned char n1 = *((unsigned char*)buffer);
		buffer++; 
		unsigned char n2 = *((unsigned char*)buffer);
		buffer++; 
		encodedNormals[i][0] = (float)n1;
		encodedNormals[i][1] = (float)n2;
	}
	return buffer;
}

void GHMD3ModelLoader::fillSurfaceFrameVB(GHVertexBuffer& vb, 
                                          const GHMD3SurfaceDescription& surfaceDesc, 
                                          const GHPoint3* tempVerts, const GHPoint2* encodedNormals, 
                                          const float* texCoords) const
{
    GHVertexStreamPtr* stream = vb.getStream(0);
    if (!stream) {
        GHDebugMessage::outputString("fillSurfaceFrameVB - expected at least one stream.");
        return;
    }
    
	float* vbStart = (float*)(stream->get()->lockWriteBuffer());
	if (!vbStart) {
		GHDebugMessage::outputString("Unable to lock buffer verts.\n");
		return;
	}
    const GHVertexStreamFormat& streamFormat = stream->get()->getFormat();
    const GHVertexStreamFormat::ComponentEntry* posEntry = streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_POS);   
    const GHVertexStreamFormat::ComponentEntry* normEntry = streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_NORMAL);   
    const GHVertexStreamFormat::ComponentEntry* uvEntry = streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_UV1);
    
	float* vertStart = vbStart;
    float* posStart = 0;
	float* normStart = 0;
	float* uvStart = 0;

	for (int i = 0; i < surfaceDesc.numverts; ++i) 
	{
		if (posEntry)
		{
            posStart = vertStart + posEntry->mOffset;
			const GHPoint3& pos = tempVerts[i];
			::memcpy(posStart, pos.getCoords(), 3*sizeof(float));
		}
		
        if (uvEntry)
        {
            uvStart = vertStart + uvEntry->mOffset;
			const float u = texCoords[i*2];
			const float v = texCoords[i*2+1]; 
			*uvStart = u;
			uvStart++;
			*uvStart = v;
		}
        
        if (normEntry) {
            normStart = vertStart + normEntry->mOffset;
            // CT_UBYTE happens to only be used for encoded normals in this context.
            if (normEntry->mType == GHVertexComponentType::CT_UBYTE)
			{
				unsigned char* byteNormStart = (unsigned char*)normStart;
				
				*byteNormStart = (unsigned char)encodedNormals[i][0];
				byteNormStart++;
				*byteNormStart = (unsigned char)encodedNormals[i][1];
			}
			else
			{
				float lat = (float)(encodedNormals[i][0] * (2*GHPI) / 255.0f);
				float lng = (float)(encodedNormals[i][1] * (2*GHPI) / 255.0f);
				float lngSin = ::sin(lng);
				float lngCos = ::cos(lng);
				float latSin = ::sin(lat);
				float latCos = ::cos(lat);

				float normX = lngCos * latSin;
				float normY = lngSin * latSin;
				float normZ = latCos;

                // special case up/down
                // the spherical coordinate system has a singularity for up/down
                if (GHFloat::isEqual(encodedNormals[i][1], 128.0)) {
                    normZ *= -1;
                }
                
				if (normEntry->mType == GHVertexComponentType::CT_BYTE)
				{
					char* byteNormStart = (char*)normStart;
					normX *= 127.0f;
					normY *= 127.0f;
					normZ *= 127.0f;
                    
					*byteNormStart = (char)normX;
					byteNormStart++;
					*byteNormStart = (char)normY;
					byteNormStart++;
					*byteNormStart = (char)normZ;
				}
				else if (normEntry->mType == GHVertexComponentType::CT_SHORT)
				{
					short* shortNormStart = (short*)normStart;
					normX *= 32000;
					normY *= 32000;
					normZ *= 32000;
					
					*shortNormStart = (short)normX;
					shortNormStart++;
					*shortNormStart = (short)normY;
					shortNormStart++;
					*shortNormStart = (short)normZ;
				}
				else
				{
					*normStart = normX;
					normStart++;
					*normStart = normY;
					normStart++;
					*normStart = normZ;
				}
			}
		}
		
		if (i < surfaceDesc.numverts-1) {
			vertStart += streamFormat.getVertexSize();
		}
	}
	stream->get()->unlockWriteBuffer();
}

void GHMD3ModelLoader::fillSurfaceFrameIB(GHVBBlitterIndex& ib, const GHMD3SurfaceDescription& surfaceDesc, 
                                          const uint32_t* connectivity) const
{
    unsigned short* ibuf = (unsigned short*)(ib.lockWriteBuffer());
	if (!ibuf) {
		GHDebugMessage::outputString("Unable to lock buffer index.\n");
		return;
	}
	int numIndex = surfaceDesc.numtriangles*3;
	for (int i = 0; i < numIndex; ++i) {
		*ibuf = (unsigned short)connectivity[i];
		if (i < numIndex-1) {
			ibuf++;
		}
	}
	ib.unlockWriteBuffer();
}

GHFrameAnimControllerVB* GHMD3ModelLoader::createSurfaceAnimController(const GHMD3SurfaceDescription& surfaceDesc) const
{
	if (surfaceDesc.numframes <= 1)
	{
		// not an animated model, so no controller.
		return 0;
	}
    
	GHFrameAnimControllerVB* animController = mAnimModifier.createAnimController();
	GHVBAnimData* animData = new GHVBAnimData;
	animController->setAnimData(animData);
    
    // add a dumb anim sequence to the tags that just loops through all.
    GHFrameAnimSet* animSet = new GHFrameAnimSet;
    GHFrameAnimSequence seq;
    seq.mFirstFrame = 0;
    seq.mEndFrame = (unsigned short)(surfaceDesc.numframes-1);
    seq.mLoopFrames = seq.mEndFrame - seq.mFirstFrame;
    seq.mFramesPerSecond = 15;
    seq.mId = mIdFactory.generateHash("default");
    animSet->addSequence(seq);
    
    animController->setAnimSet(animSet);
    animController->setAnim(seq.mId, 0);
    
	return animController;
}
