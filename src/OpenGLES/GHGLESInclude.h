// Copyright 2010 Golden Hammer Software
#pragma once

#ifndef BLACKBERRY
	#ifndef ANDROID
        // likely ios.
		#define GH_ES_VERSION 3

		#import <OpenGLES/ES3/gl.h>
		#import <OpenGLES/ES3/glext.h>
	#else
		#define GH_ES_VERSION 3

		#include <GLES3/gl3.h>
		#include <GLES3/gl3ext.h>
	#endif
#else
    // blackberry
	#define GH_ES_VERSION 2

	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#endif

#ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#endif
#ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif

#ifndef GL_MAJOR_VERSION
#define GL_MAJOR_VERSION 0x821B
#endif
#ifndef GL_MINOR_VERSION
#define GL_MINOR_VERSION 0x821C
#endif

#include "GHGLTypes.h"
