// Copyright 2010 Golden Hammer Software
#include "GHBBJPGLoader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "GHResourceFactory.h"
#include "GHFileC.h"
#include "GHFileOpener.h"
#include "GHGLESTexture.h"
#include <jpeglib.h>

GHBBJPGLoader::GHBBJPGLoader(GHFileOpener& fileOpener, GHResourceFactory& resourceCache,
		GHGLESTextureMgr& textureMgr)
: mFileOpener(fileOpener)
, mResourceCache(resourceCache)
, mTextureMgr(textureMgr)
{
}

GHResource* GHBBJPGLoader::redirectPVRToJPG(const char* filename)
{
	const char* ext = ::strstr(filename, ".pvr4");
	if (ext == 0) {
		return 0;
	}

	char newName[128];
	::strcpy(newName, filename);
	char* ext2 = ::strstr(newName, ".");
	*(ext2+1) = 'j'; *(ext2+2) = 'p'; *(ext2+3) = 'g'; *(ext2+4) = '\0';
	GHResource* ret = mResourceCache.getUniqueResource(newName);
	if (!ret)
	{
		*(ext2+1) = 'p'; *(ext2+2) = 'n'; *(ext2+3) = 'g'; *(ext2+4) = '\0';
		ret = mResourceCache.getUniqueResource(newName);
	}
	return ret;
}

GHResource* GHBBJPGLoader::loadFile(const char* filename, GHPropertyContainer* extraData)
{
	GHResource* redirect = redirectPVRToJPG(filename);
	if (redirect) {
		return redirect;
	}

	unsigned char *image;

    GHFileC* file = (GHFileC*)mFileOpener.openFile(filename, GHFile::FT_BINARY, GHFile::FM_READONLY);
    if (!file) {
    	return 0;
    }
    FILE* fd = file->getFILE();
    if (!fd) {
        return 0;
    }

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];
	unsigned long location = 0;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, fd);
	jpeg_read_header(&cinfo, 0);

	cinfo.scale_num = 1;
	cinfo.scale_denom = 1;
	jpeg_start_decompress(&cinfo);

	int width = cinfo.output_width;
	int height = cinfo.output_height;
	int depth = cinfo.num_components; //should always be 3
	image = new unsigned char[width*height*depth];
	row_pointer[0] = new unsigned char[width*depth];

	GHGLESTexture* ret = 0;
	if (image && row_pointer[0])
	{
		/* read one scan line at a time */
		int rowLen = width*depth;
		while( cinfo.output_scanline < cinfo.output_height )
		{
			jpeg_read_scanlines( &cinfo, row_pointer, 1 );
			for (int i=0; i < rowLen; i++)
			{
				image[location++] = row_pointer[0][i];
			}
		}

		// todo: support not generating mipmaps
        bool generateMips = true;
		ret = new GHGLESTexture(mTextureMgr, image, width, height, depth, generateMips);
		ret->deleteSourceData();
	}

	delete [] row_pointer[0];
	delete file;
	// we would delete image but it has been passed to the yrgtexture.
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	return ret;
}
