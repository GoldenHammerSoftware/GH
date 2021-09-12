// Copyright Golden Hammer Software
#include "GHMemoryTexture.h"
#include <assert.h>

GHMemoryTexture::GHMemoryTexture(unsigned int width, unsigned int height, unsigned int depth)
:	mpTexels( 0 )
,	mWidth( width )
,	mHeight( height )
,	mDepth( depth )
,	mLocked( false )
{
	assert( mWidth > 0 );
	assert( mHeight > 0 );
	assert( mDepth > 0 );
	mpTexels = new unsigned char[ mWidth * mHeight * mDepth ];
}

GHMemoryTexture::GHMemoryTexture(unsigned char* pTexels, unsigned int width, unsigned int height, unsigned int depth)
:	mpTexels( pTexels )
,	mWidth( width )
,	mHeight( height )
,	mDepth( depth )
,	mLocked( false )
{
	assert( mWidth > 0 );
	assert( mHeight > 0 );
	assert( mDepth > 0 );
	assert( mpTexels != 0 );
}

GHMemoryTexture::~GHMemoryTexture( void )
{
	delete [] mpTexels;
}

bool GHMemoryTexture::lockSurface(void** ret, unsigned int& pitch)
{
	*ret = mpTexels;
	pitch = mWidth * mDepth;
	return true;
}

bool GHMemoryTexture::unlockSurface(void)
{
	bool wasLocked = mLocked;
	mLocked = false;
	return wasLocked;
}

bool GHMemoryTexture::getDimensions(unsigned int& width, unsigned int& height, unsigned int& depth)
{
	width = mWidth;
	height = mHeight;
	depth = mDepth;
	return true;
}

