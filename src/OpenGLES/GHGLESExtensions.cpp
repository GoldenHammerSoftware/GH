#include "GHGLESExtensions.h"
#include "GHGLESInclude.h"
#include "string.h"
#include "GHGLESErrorReporter.h"

bool GHGLESExtensions::checkExtension(const char* ext)
{
    const char* allExtensions = (const char*)glGetString(GL_EXTENSIONS);
    const char* extStr = strstr(allExtensions, ext);
    return (extStr != 0);
}

bool GHGLESExtensions::checkAtLeastES3(void)
{
    // glGetIntegerv(GL_MAJOR_VERSION) was added in gles 3.
    // Therefore we can check for error and know that we are on <3.
    // glGetString(GL_VERSION) gives inconsistent result format.
    // Clear out the error code first to make doubly sure.
    GHGLESErrorReporter::checkAndReportError();
    int major = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    // check to see if our test worked.
    GLenum glErrorCode = glGetError();
    if (glErrorCode == GL_NO_ERROR)
    {
        if (major >= 3)
        {
            return true;
        }
    }
    return false;
}

