// Copyright 2010 Golden Hammer Software
#include "GHBBGLESContext.h"
#include <stdlib.h>
#include "GHDebugMessage.h"

static int nbuffers = 2;

static void egl_perror(const char *msg) {
    static const char *errmsg[] = {
        "function succeeded",
        "EGL is not initialized, or could not be initialized, for the specified display",
        "cannot access a requested resource",
        "failed to allocate resources for the requested operation",
        "an unrecognized attribute or attribute value was passed in an attribute list",
        "an EGLConfig argument does not name a valid EGLConfig",
        "an EGLContext argument does not name a valid EGLContext",
        "the current surface of the calling thread is no longer valid",
        "an EGLDisplay argument does not name a valid EGLDisplay",
        "arguments are inconsistent",
        "an EGLNativePixmapType argument does not refer to a valid native pixmap",
        "an EGLNativeWindowType argument does not refer to a valid native window",
        "one or more argument values are invalid",
        "an EGLSurface argument does not name a valid surface configured for rendering",
        "a power management event has occurred",
        "unknown error code"
    };

    int message_index = eglGetError() - EGL_SUCCESS;

    if (message_index < 0 || message_index > 14)
        message_index = 15;

    printf("%s: %s\n", msg, errmsg[message_index]);
}

GHBBGLESContext::GHBBGLESContext(screen_context_t& ctx)
: screen_ctx(ctx)
, egl_disp(EGL_NO_DISPLAY)
, egl_ctx(EGL_NO_CONTEXT)
, egl_surf(EGL_NO_SURFACE)
, screen_win(NULL)
{
    EGLint interval = 1;
    int rc, num_configs;
    int format = SCREEN_FORMAT_RGBX8888;
    int usage = SCREEN_USAGE_OPENGL_ES2 | SCREEN_USAGE_ROTATION;
    int idle_mode = SCREEN_IDLE_MODE_KEEP_AWAKE;

    EGLint attrib_list[]= { EGL_RED_SIZE,        8,
                            EGL_GREEN_SIZE,      8,
                            EGL_BLUE_SIZE,       8,
                            EGL_DEPTH_SIZE,		 16,
                            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
                            EGL_RENDERABLE_TYPE, 0,
                            EGL_NONE};

    attrib_list[11] = EGL_OPENGL_ES2_BIT;
    EGLint attributes[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };

    //Simple egl initialization
    egl_disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (egl_disp == EGL_NO_DISPLAY) {
    	egl_perror("eglGetDisplay");
        return;
    }

    rc = eglInitialize(egl_disp, NULL, NULL);
    if (rc != EGL_TRUE) {
        egl_perror("eglInitialize");
        return;
    }

    rc = eglBindAPI(EGL_OPENGL_ES_API);

    if (rc != EGL_TRUE) {
        egl_perror("eglBindApi");
        return;
    }

    if(!eglChooseConfig(egl_disp, attrib_list, &egl_conf, 1, &num_configs)) {
        return;
    }

    egl_ctx = eglCreateContext(egl_disp, egl_conf, EGL_NO_CONTEXT, attributes);

    if (egl_ctx == EGL_NO_CONTEXT) {
        egl_perror("eglCreateContext");
        return;
    }

    rc = screen_create_window(&screen_win, screen_ctx);
    if (rc) {
        perror("screen_create_window");
        return;
    }

    rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_FORMAT, &format);
    if (rc) {
        perror("screen_set_window_property_iv(SCREEN_PROPERTY_FORMAT)");
        return;
    }

    rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_IDLE_MODE, &idle_mode);
    if (rc) {
    	perror("screen_set_window_property_iv(SCREEN_PROPERTY_IDLE_MODE)");
    	return;
    }

    rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_USAGE, &usage);
    if (rc) {
        perror("screen_set_window_property_iv(SCREEN_PROPERTY_USAGE)");
        return;
    }

    rc = screen_get_window_property_pv(screen_win, SCREEN_PROPERTY_DISPLAY, (void **)&screen_disp);
    if (rc) {
        perror("screen_get_window_property_pv");
        return;
    }

    screen_display_mode_t screen_mode;
    rc = screen_get_display_property_pv(screen_disp, SCREEN_PROPERTY_MODE, (void**)&screen_mode);
    if (rc) {
        perror("screen_get_display_property_pv");
        return;
    }

    int screen_resolution[2];
    rc = screen_get_display_property_iv(screen_disp, SCREEN_PROPERTY_SIZE, screen_resolution);
    if (rc) {
        perror("screen_get_display_property_iv");
        return;
    }

    int angle = atoi(getenv("ORIENTATION"));

	int size[2];
	rc = screen_get_window_property_iv(screen_win, SCREEN_PROPERTY_BUFFER_SIZE, size);
	if (rc) {
	    perror("screen_get_window_property_iv");
	    return;
	}

	buffer_size[0] = size[0];
	buffer_size[1] = size[1];

	if ((angle == 0) || (angle == 180)) {
	    if (((screen_mode.width > screen_mode.height) && (size[0] < size[1])) ||
	        ((screen_mode.width < screen_mode.height) && (size[0] > size[1]))) {
	            buffer_size[1] = size[0];
	            buffer_size[0] = size[1];
	    }
	} else if ((angle == 90) || (angle == 270)){
	    if (((screen_mode.width > screen_mode.height) && (size[0] > size[1])) ||
	        ((screen_mode.width < screen_mode.height && size[0] < size[1]))) {
	            buffer_size[1] = size[0];
	            buffer_size[0] = size[1];
	    }
	} else {
	     fprintf(stderr, "Navigator returned an unexpected orientation angle.\n");
	     return;
	}

	rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_BUFFER_SIZE, buffer_size);
	if (rc) {
	    perror("screen_set_window_property_iv");
	    return;
	}

	rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_ROTATION, &angle);
	if (rc) {
	    perror("screen_set_window_property_iv");
	    return;
	}

    rc = screen_create_window_buffers(screen_win, nbuffers);
    if (rc) {
        perror("screen_create_window_buffers");
        return;
    }

    egl_surf = eglCreateWindowSurface(egl_disp, egl_conf, screen_win, NULL);
    if (egl_surf == EGL_NO_SURFACE) {
        egl_perror("eglCreateWindowSurface");
        return;
    }

    rc = eglMakeCurrent(egl_disp, egl_surf, egl_surf, egl_ctx);
    if (rc != EGL_TRUE) {
        egl_perror("eglMakeCurrent");
        return;
    }

    rc = eglSwapInterval(egl_disp, interval);
    if (rc != EGL_TRUE) {
        egl_perror("eglSwapInterval");
        return;
    }
}

GHBBGLESContext::~GHBBGLESContext(void)
{
    //Typical EGL cleanup
    if (egl_disp != EGL_NO_DISPLAY) {
        eglMakeCurrent(egl_disp, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (egl_surf != EGL_NO_SURFACE) {
            eglDestroySurface(egl_disp, egl_surf);
            egl_surf = EGL_NO_SURFACE;
        }

        if (egl_ctx != EGL_NO_CONTEXT) {
            eglDestroyContext(egl_disp, egl_ctx);
            egl_ctx = EGL_NO_CONTEXT;
        }
        if (screen_win != NULL) {
            screen_destroy_window(screen_win);
            screen_win = NULL;
        }
        EGLBoolean terminateResult = eglTerminate(egl_disp);
        if (terminateResult == EGL_FALSE) {
        	GHDebugMessage::outputString("eglTerminate failed");
        }
        egl_disp = EGL_NO_DISPLAY;
    }
    EGLBoolean releaseResult = eglReleaseThread();
    if (releaseResult != EGL_TRUE) {
    	GHDebugMessage::outputString("eglReleaseThread failed");
    }
}

void GHBBGLESContext::beginFrame(void)
{
	glViewport(0, 0, buffer_size[0], buffer_size[1]);
}

void GHBBGLESContext::endFrame(void)
{
    int rc = eglSwapBuffers(egl_disp, egl_surf);
    if (rc != EGL_TRUE) {
        egl_perror("eglSwapBuffers");
    }
}

void GHBBGLESContext::handleScreenRotation(int angle)
{
    int rc, rotation, skip = 1, temp;;
    EGLint interval = 1;
    int size[2];

    if ((angle != 0) && (angle != 90) && (angle != 180) && (angle != 270)) {
        fprintf(stderr, "Invalid angle\n");
        return;
    }

    rc = screen_get_window_property_iv(screen_win, SCREEN_PROPERTY_ROTATION, &rotation);
    if (rc) {
        perror("screen_set_window_property_iv");
        return;
    }

    rc = screen_get_window_property_iv(screen_win, SCREEN_PROPERTY_BUFFER_SIZE, size);
    if (rc) {
        perror("screen_set_window_property_iv");
        return;
    }

    switch (angle - rotation) {
        case -270:
        case -90:
        case 90:
        case 270:
            temp = size[0];
            size[0] = size[1];
            size[1] = temp;
            skip = 0;
            break;
    }
	buffer_size[0] = size[0];
	buffer_size[1] = size[1];

    if (!skip) {
        rc = eglMakeCurrent(egl_disp, NULL, NULL, NULL);
        if (rc != EGL_TRUE) {
        	egl_perror("eglMakeCurrent");
            return;
        }

        rc = eglDestroySurface(egl_disp, egl_surf);
        if (rc != EGL_TRUE) {
        	egl_perror("eglMakeCurrent");
            return;
        }

        rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_SOURCE_SIZE, size);
        if (rc) {
            perror("screen_set_window_property_iv");
            return;
        }

        rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_BUFFER_SIZE, size);
        if (rc) {
            perror("screen_set_window_property_iv");
            return;
        }
        egl_surf = eglCreateWindowSurface(egl_disp, egl_conf, screen_win, NULL);
        if (egl_surf == EGL_NO_SURFACE) {
        	egl_perror("eglCreateWindowSurface");
            return;
        }

        rc = eglMakeCurrent(egl_disp, egl_surf, egl_surf, egl_ctx);
        if (rc != EGL_TRUE) {
        	egl_perror("eglMakeCurrent");
            return;
        }

        rc = eglSwapInterval(egl_disp, interval);
        if (rc != EGL_TRUE) {
        	egl_perror("eglSwapInterval");
            return;
        }
    }

    rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_ROTATION, &angle);
    if (rc) {
        perror("screen_set_window_property_iv");
    }
}
