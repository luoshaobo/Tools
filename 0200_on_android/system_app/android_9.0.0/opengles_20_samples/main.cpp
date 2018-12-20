#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/inotify.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utils/misc.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gl.h>
#include <GLES/glext.h>

#include <ui/PixelFormat.h>
#include <ui/Rect.h>
#include <ui/Region.h>
#include <ui/DisplayInfo.h>

#include <gui/ISurfaceComposer.h>
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>

#include "log.h"

using namespace android;

extern "C" {
    struct ExternalElgContent
    {
       GLint       width;
       GLint       height;
       EGLDisplay  eglDisplay;
       EGLContext  eglContext;
       EGLSurface  eglSurface;
    } gExternalElgContent;
}

extern "C" {   
    void sample_app_main__Chapter_2__Hello_Triangle();
    void sample_app_main__Chapter_9__Simple_Texture2D();
}

static void DrawFrames()
{
    //sample_app_main__Chapter_2__Hello_Triangle();
    sample_app_main__Chapter_9__Simple_Texture2D();
}

int main(int argc, char *argv[])
{
    sp<IBinder> dtoken(SurfaceComposerClient::getBuiltInDisplay(ISurfaceComposer::eDisplayIdMain));
    DisplayInfo dinfo;
    status_t status = SurfaceComposerClient::getDisplayInfo(dtoken, &dinfo);
    if (status)
        return -1;

    // create the native surface
    sp<SurfaceComposerClient> surfaceComposerClient = new SurfaceComposerClient;
    sp<SurfaceControl> control = surfaceComposerClient->createSurface(String8("BootAnimation"),
            dinfo.w, dinfo.h, PIXEL_FORMAT_RGBA_8888);

    SurfaceComposerClient::Transaction t;
    t.setLayer(control, 0x40000004).apply();

    sp<Surface> nativieSurface = control->getSurface();

    // initialize opengl and egl
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_RED_SIZE,   8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE,  8,
            EGL_ALPHA_SIZE, 8,
            //EGL_DEPTH_SIZE, 8,
            //EGL_STENCIL_SIZE, 8,
            EGL_NONE
    };
    EGLint w, h;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;
    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);
    surface = eglCreateWindowSurface(display, config, nativieSurface.get(), NULL);
    context = eglCreateContext(display, config, NULL, contextAttribs);
    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
        return NO_INIT;
    
    gExternalElgContent.width = w;
    gExternalElgContent.height = h;
    gExternalElgContent.eglDisplay = display;
    gExternalElgContent.eglContext = context;
    gExternalElgContent.eglSurface = surface;
    
    DrawFrames();
    
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(display, context);
    eglDestroySurface(display, surface);
    eglTerminate(display);
    eglReleaseThread();
    
    return 0;
}
