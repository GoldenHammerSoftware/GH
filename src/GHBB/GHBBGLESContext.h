// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHGLESContext.h"
#include <EGL/egl.h>
#include <screen/screen.h>
#include <sys/platform.h>
#include <GLES2/gl2.h>

class GHBBGLESContext : public GHGLESContext
{
public:
	GHBBGLESContext(screen_context_t& ctx);
	~GHBBGLESContext(void);

	virtual void beginFrame(void);
	virtual void endFrame(void);

	void getScreenSize(int& w, int& h) {
		w = buffer_size[0]; h = buffer_size[1];
	}

	void handleScreenRotation(int angle);

private:
	EGLDisplay egl_disp;
	EGLSurface egl_surf;
	EGLConfig egl_conf;
	EGLContext egl_ctx;
	screen_context_t& screen_ctx;
	screen_window_t screen_win;
	screen_display_t screen_disp;
	int buffer_size[2];
};
