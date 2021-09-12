#include "GHGLESCubemap.h"
#include "GHGLESTextureMgr.h"
#include "GHGLESErrorReporter.h"

GHGLESCubemap::GHGLESCubemap(GHGLESTextureMgr& textureMgr, GHGLESStateCache& stateCache,
	std::vector<GHGLESTexture*>&& faceTextures,
	bool autoGenerateMips,
	const GHTextureDataLoader* dataLoader, const char* sourceName)
	: GHGLESTexture(textureMgr, stateCache, nullptr, autoGenerateMips, dataLoader, sourceName)
	, mFaceTextures(std::move(faceTextures))
{
	mBindType = GL_TEXTURE_CUBE_MAP; //<-- this is all we need to do to make GHGLESTexture::bind work
	assert(mFaceTextures.size() == 6);
	for (int i = 0; i < mFaceTextures.size(); ++i)
	{
		mFaceTextures[i]->acquire();
	}

	createOGLTexture();
}

GHGLESCubemap::~GHGLESCubemap(void) 
{
	for (int i = 0; i < mFaceTextures.size(); ++i)
	{
		mFaceTextures[i]->release();
	}
}

bool GHGLESCubemap::lockSurface(void** ret, unsigned int& pitch)
{
	assert(false && "GHGLESCubemap::lockSurface not implemented");
	*ret = 0;
	pitch = 0;
	return false;
}

bool GHGLESCubemap::getDimensions(unsigned int& width, unsigned int& height, unsigned int& depth)
{
	if (!mFaceTextures.size()) {
		return false;
	}

	unsigned int firstWidth = 0, firstHeight = 0, firstDepth = 0;
	if (!mFaceTextures[0]->getDimensions(firstWidth, firstHeight, firstDepth)) {
		return false;
	}

	//just double check that all faces have matching dimensions in debug mode
	assert(subsequentDimensionsMatch(firstWidth, firstHeight, firstDepth));

	width = firstWidth;
	height = firstHeight;
	depth = firstDepth;
	return true;
}

bool GHGLESCubemap::subsequentDimensionsMatch(unsigned int firstWidth, unsigned int firstHeight, unsigned int firstDepth)
{
	unsigned int subsequentWidth = 0, subsequentHeight = 0, subsequentDepth = 0;
	size_t numFaceTextures = mFaceTextures.size();
	for (size_t i = 1; i < numFaceTextures; ++i)
	{
		if (!mFaceTextures[i]->getDimensions(subsequentWidth, subsequentHeight, subsequentDepth)) {
			return false;
		}

		if (subsequentWidth != firstWidth) {
			return false;
		}
		
		if (subsequentHeight != firstHeight) {
			return false;
		}
		
		if (subsequentDepth != firstDepth) {
			return false;
		}
	}

	return true;
}

void GHGLESCubemap::deleteSourceData(void)
{
	size_t numFaces = mFaceTextures.size();
	for (size_t i = 0; i < numFaces; ++i)
	{
		mFaceTextures[i]->deleteSourceData();
	}
}

void GHGLESCubemap::createOGLTexture(void)
{
	int numFaces = (int)mFaceTextures.size();
	if (numFaces != 6) { 
		return;
	}

	mTextureId = mTextureMgr.getUnusedHandle();
	bind();

	for (int face = 0; face < numFaces; ++face)
	{
		GHGLESTexture* tex = mFaceTextures[face];
		tex->uploadToOGL(0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face);
	}
	
	//todo: support uploading explicit mips?
	if (mAutoGenerateMipmaps)
	{
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}
	GHGLESErrorReporter::checkAndReportError("GHGLESCubemap::CreateOGLTexture glGenerateMipmap");

}
