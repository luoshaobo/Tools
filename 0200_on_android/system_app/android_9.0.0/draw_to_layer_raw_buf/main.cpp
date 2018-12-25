#define LOG_TAG "codec"
#include <utils/Log.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <system/window.h>
#include <gui/ISurfaceComposer.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/Surface.h>
#include <ui/DisplayInfo.h>
#include <ui/GraphicBufferMapper.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/ALooper.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/foundation/AString.h>

#define UNUSED(v) (&v);

#undef CHECK_EQ
#define CHECK_EQ(a1,a2) if ((a1) != (a2)) { abort(); }

using namespace android;

struct ColorRGBA {
    unsigned char r, g, b, a;
};

void RenderFrame(sp<ANativeWindow> nativeWindow, int32_t width, int32_t height, unsigned char colorHint)
{
    ANativeWindowBuffer *buf;
    int err;
    if ((err = native_window_dequeue_buffer_and_wait(nativeWindow.get(), &buf)) != 0) {
        ALOGW("Surface::dequeueBuffer returned error %d", err);
        return;
    }

    GraphicBufferMapper &mapper = GraphicBufferMapper::get();

    Rect bounds(width, height);

    void *dst = 0;
    CHECK_EQ(0, mapper.lock(buf->handle, GRALLOC_USAGE_SW_WRITE_OFTEN, bounds, &dst));
    
    for (int32_t i = 0; i < height; i++) {
        for (int32_t j = 0; j < width; j++) {
            ColorRGBA *pPixel = (ColorRGBA *)((unsigned char *)dst + width * 4 * i + 4 * j);
            pPixel->r = colorHint;
            pPixel->g = 0;
            pPixel->b = 0;
            pPixel->a = 200;
        }
    }
                
    CHECK_EQ(0, mapper.unlock(buf->handle));

    if ((err = nativeWindow->queueBuffer(nativeWindow.get(), buf, -1)) != 0) {
        ALOGW("Surface::queueBuffer returned error %d", err);
    }
    buf = NULL;
}

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);
    
    //////////////////////////////////////////////////////////////////////////////////////
    // 1. Create a window instance.
    //
    sp<SurfaceComposerClient> surfaceComposerClient = new SurfaceComposerClient;
    CHECK_EQ(surfaceComposerClient->initCheck(), (status_t)OK);

    sp<IBinder> display(SurfaceComposerClient::getBuiltInDisplay(ISurfaceComposer::eDisplayIdMain));
    DisplayInfo displayInfo;
    SurfaceComposerClient::getDisplayInfo(display, &displayInfo);
    ssize_t displayWidth = displayInfo.w;
    ssize_t displayHeight = displayInfo.h;

    sp<SurfaceControl> surfaceControl = surfaceComposerClient->createSurface(
            String8("A Surface"),
            displayWidth,
            displayHeight,
            PIXEL_FORMAT_RGB_565,
            0);
    CHECK(surfaceControl != NULL);
    CHECK(surfaceControl->isValid());
    
    SurfaceComposerClient::Transaction{}
         .setLayer(surfaceControl, INT_MAX)
         .show(surfaceControl)
         .apply();

    sp<Surface> surface = surfaceControl->getSurface();
    CHECK(surface != NULL);
    
    //////////////////////////////////////////////////////////////////////////////////////
    // 2. Initialize the window.
    //
    int nativeWindowApi = NATIVE_WINDOW_API_MEDIA;                                        // ?
    CHECK_EQ(native_window_api_connect(surface.get(), nativeWindowApi), (status_t)OK);
    
    int halFormat;
    size_t bufWidth, bufHeight;

    halFormat = HAL_PIXEL_FORMAT_RGBA_8888;
    bufWidth = displayWidth;
    bufHeight = displayHeight;
    
    sp<ANativeWindow> nativeWindow(surface.get());
#ifdef EXYNOS4_ENHANCEMENTS
    CHECK_EQ(0,
            native_window_set_usage(
            nativeWindow.get(),
            GRALLOC_USAGE_SW_READ_NEVER | GRALLOC_USAGE_SW_WRITE_OFTEN
            | GRALLOC_USAGE_HW_TEXTURE | GRALLOC_USAGE_EXTERNAL_DISP
            | GRALLOC_USAGE_HW_FIMC1 | GRALLOC_USAGE_HWC_HWOVERLAY));
#else
    CHECK_EQ(0,
            native_window_set_usage(
            nativeWindow.get(),
            GRALLOC_USAGE_SW_READ_NEVER | GRALLOC_USAGE_SW_WRITE_OFTEN
            | GRALLOC_USAGE_HW_TEXTURE | GRALLOC_USAGE_EXTERNAL_DISP));
#endif

    CHECK_EQ(0,
            native_window_set_scaling_mode(
            nativeWindow.get(),
            NATIVE_WINDOW_SCALING_MODE_SCALE_TO_WINDOW));

    CHECK_EQ(0, native_window_set_buffers_dimensions(
                nativeWindow.get(),
                bufWidth,
                bufHeight));
                
    CHECK_EQ(0, native_window_set_buffers_format(
                nativeWindow.get(),
                halFormat));

    uint32_t transform = 0;
    if (transform) {
        CHECK_EQ(0, native_window_set_buffers_transform(nativeWindow.get(), transform));
    }
    
    //////////////////////////////////////////////////////////////////////////////////////
    // 3. Render each frame.
    //
    unsigned char colorHint = 0;    
    for (;;) {
        
        RenderFrame(nativeWindow, bufWidth, bufHeight, colorHint);
        usleep(1000 * 100);
        
        colorHint += 3;
    }
    
    //////////////////////////////////////////////////////////////////////////////////////
    // 4. Finalize the window.
    //    
    CHECK_EQ(native_window_api_disconnect(nativeWindow.get(), nativeWindowApi), (status_t)OK);

    return 0;
}
