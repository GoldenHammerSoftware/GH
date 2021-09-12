// Copyright 2010 Golden Hammer Software
#include "GHBBPNGLoader.h"
#include "png.h"
#include <stdlib.h>
#include <stdio.h>
#include "GHGLESTexture.h"
#include "GHFileOpener.h"
#include "GHFileC.h"
#include "GHGLESTextureMgr.h"
#include <GLES2/gl2.h>

/* Finds the next power of 2 */
static inline int
nextp2(int x)
{
    int val = 1;
    while(val < x) val <<= 1;
    return val;
}

GHBBPNGLoader::GHBBPNGLoader(GHFileOpener& fileOpener, GHGLESTextureMgr& textureMgr)
: mTextureMgr(textureMgr)
, mFileOpener(fileOpener)
{
}

GHResource* GHBBPNGLoader::loadFile(const char* filename, GHPropertyContainer* extraData)
{
    int i;
    GLuint format;
    //header for testing if it is a png
    png_byte header[8];

    GHFileC* file = (GHFileC*)mFileOpener.openFile(filename, GHFile::FT_BINARY, GHFile::FM_READONLY);
    if (!file) {
    	return 0;
    }
    FILE* fp = file->getFILE();
    if (!fp) {
        return 0;
    }

    //read the header
    fread(header, 1, 8, fp);

    //test if png
    int is_png = !png_sig_cmp(header, 0, 8);
    if (!is_png) {
        delete file;
        return 0;
    }

    //create png struct
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        delete file;
        return 0;
    }

    //create png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
        delete file;
        return 0;
    }

    //create png info struct
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        delete file;
        return 0;
    }

    //setup error handling (required without using custom error handlers above)
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        delete file;
        return 0;
    }

    //init png reading
    png_init_io(png_ptr, fp);

    //let libpng know you already read the first 8 bytes
    png_set_sig_bytes(png_ptr, 8);

    // read all the info up to the image data
    png_read_info(png_ptr, info_ptr);

    //variables to pass to get info
    int bit_depth, color_type;
    png_uint_32 image_width, image_height;

    // get info about png
    png_get_IHDR(png_ptr, info_ptr, &image_width, &image_height, &bit_depth, &color_type, NULL, NULL, NULL);

    int colorDepth = 4;
    switch (color_type)
    {
        case PNG_COLOR_TYPE_RGBA:
            format = GL_RGBA;
            break;
        case PNG_COLOR_TYPE_RGB:
            format = GL_RGB;
            colorDepth = 3;
            break;
        default:
            fprintf(stderr,"Unsupported PNG color type (%d) for texture: %s", (int)color_type, filename);
            fclose(fp);
            png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
            return NULL;
    }

    // Update the png info struct.
    png_read_update_info(png_ptr, info_ptr);

    // Row size in bytes.
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    // Allocate the image_data as a big block, to be given to opengl
    png_byte *image_data = new png_byte[rowbytes*image_height];

    if (!image_data) {
        //clean up memory and close stuff
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        delete file;
        return 0;
    }

    //row_pointers is for pointing to image_data for reading the png with libpng
    png_bytep *row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * image_height);
    if (!row_pointers) {
        //clean up memory and close stuff
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        delete [] image_data;
        delete file;
        return 0;
    }

    // set the individual row_pointers to point at the correct offsets of image_data
    for (i = 0; i < image_height; i++) {
        row_pointers[i] = image_data + i * rowbytes;
    }

    //read the png into image_data through row_pointers
    png_read_image(png_ptr, row_pointers);

    int tex_width, tex_height;

    tex_width = nextp2(image_width);
    tex_height = nextp2(image_height);

    // premultiply the alpha
    if (color_type == PNG_COLOR_TYPE_RGBA)
    {
    	int numPixels = image_width * image_height;
    	int xIdx = 0;
    	int aIdx = 3;
    	for (int i = 0; i < numPixels; ++i) {
    		float alphaVal = ((float)image_data[aIdx]/255.0f);
    		image_data[xIdx] *= alphaVal;
    		image_data[xIdx+1] *= alphaVal;
    		image_data[xIdx+2] *= alphaVal;
    		xIdx += 4;
    		aIdx += 4;
    	}
    }

	// todo: support not generating mipmaps
    bool generateMips = true;
	GHGLESTexture* ret = new GHGLESTexture(mTextureMgr, image_data, tex_width, tex_height, colorDepth, generateMips);

    //clean up memory and close stuff
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    free(row_pointers);
    delete file;

    return ret;
}
