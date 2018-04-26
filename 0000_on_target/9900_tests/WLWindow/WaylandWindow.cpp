
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <iostream>

#define LINUX
#define EGL_API_FB

#include <wayland-egl.h>
#include <EGL/egl.h>
//#include <GL/gl.h>
#include <GLES2/gl2.h>
#include "ivi-application-client-protocol.h"
#include "WaylandWindow.h"

static EGLDisplay egl_display;
static EGLContext egl_context;
static EGLSurface egl_surface;

typedef enum
{
    LOG_DEFAULT =    	  -1,   /**< Default log level */
    LOG_OFF          =      0x00,   /**< Log level off */
    LOG_FATAL      =   	  0x01, 	/**< fatal system error */
    LOG_ERROR     =      0x02, 	/**< error with impact to correct functionality */
    LOG_WARN      =      0x03, 	/**< warning, correct behaviour could not be ensured */
    LOG_INFO        =      0x04, 	/**< informational */
    LOG_DEBUG     =      0x05, 	/**< debug  */
    LOG_VERBOSE  =      0x06 	/**< highest grade of information */
} LogLevelType;

inline static void writeLog(LogLevelType log_level_type, const str_t* pFunction, uint32_t lineNum, const char *format, ...)
{
    bool needToWirte = true;
    if (needToWirte)
    {
        va_list args;
        char buffer[512];
        va_start(args, format);
        vsnprintf(buffer, 512, format, args);
        va_end(args);
        printf("%s\n", buffer);
    }
}

#define LOG_PRINT(logLevel, fmt, args...)      writeLog(logLevel, __FUNCTION__ , __LINE__ ,fmt,##args)

#define WL_TOUCH_CLICK_TICKCOUNT								200
#define WL_TOUCH_LONG_PRESS_TICKCOUNT					500
#define WL_TOUCH_LONG_LONG_PRESS_TICKCOUNT		1000


static int set_cloexec_or_close(int fd)
{
    long flags;
    if (fd == -1)
    {
        return -1;
    }

    flags = fcntl(fd, F_GETFD);
    if (flags == -1)
    {
        close(fd);
        return -1;
    }

    if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1)
    {
        close(fd);
        return -1;
    }
    return fd;
}

static int create_tmpfile_cloexec(char *tmpname)
{
    int fd = 0;

#ifdef HAVE_MKOSTEMP
    fd = mkostemp(tmpname, O_CLOEXEC);
    if (fd >= 0)
        unlink(tmpname);
#else
    fd = mkstemp(tmpname);
    if (fd >= 0)
    {
        fd = set_cloexec_or_close(fd);
        unlink(tmpname);
    }
#endif

    return fd;
}

static int os_create_anonymous_file(off_t size)
{
    static const char template1[] = "/weston-shared-XXXXXX";
    const char *path;
    char *name;
    int fd;
    int ret;

    path = getenv("XDG_RUNTIME_DIR");
    if (!path)
    {
        errno = ENOENT;
        return -1;
    }

    name = (char *) malloc(strlen(path) + sizeof(template1));
    if (!name)
    {
        return -1;
    }

    strcpy(name, path);
    strcat(name, template1);

    fd = create_tmpfile_cloexec(name);

    free(name);

    if (fd < 0)
    {
        return -1;
    }

#ifdef HAVE_POSIX_FALLOCATE
    ret = posix_fallocate(fd, 0, size);
    if (ret != 0)
    {
        close(fd);
        errno = ret;
        return -1;
    }
#else
    ret = ftruncate(fd, size);
    if (ret < 0)
    {
        close(fd);
        return -1;
    }
#endif
    return fd;
}

static void draw_window ()
{
    LOG_PRINT(LOG_INFO, "====draw_window=111==");
    LOG_PRINT(LOG_INFO, "====draw_window=11==");
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    LOG_PRINT(LOG_INFO, "====draw_window=22==");
    eglSwapBuffers(egl_display, egl_surface);
    LOG_PRINT(LOG_INFO, "====draw_window=33==");
}

//static void buffer_release(void *data, struct wl_buffer *buffer)
//{
//	struct WLBuffer *mybuf = (struct WLBuffer *) data;
//	mybuf->busy = 0;
//}

//static const struct wl_buffer_listener buffer_listener = {
//	buffer_release
//};

static void paintpixels(void *image, int width, int height)
{
    uint32_t *pixel = (uint32_t*)image;
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            *pixel++ = 0xFF00FF00;
        }
    }
}

const struct wl_shm_listener WaylandWindow::m_wlShmListener = {
    WaylandWindow::shm_format
};

/// wayland listeners.
const struct ivi_surface_listener WaylandWindow::m_wlIviSurfaceListener = {
    NULL,
    WaylandWindow::handle_ivi_surface_configure,
};

const struct wl_registry_listener WaylandWindow::m_wlRegistryListener = {
    WaylandWindow::registry_handle_global,
    WaylandWindow::registry_handle_global_remove
};

// Callback to retrieve changes in the seat capabilities.
const struct wl_seat_listener WaylandWindow::m_wlSeatListener = {
    &WaylandWindow::seatHandleCapabilities
};

// The touch listener callbacks, called from wayland.
const struct wl_pointer_listener WaylandWindow::m_wlPointerListener = {
    &WaylandWindow::pointer_handle_enter,
    &WaylandWindow::pointer_handle_leave,
    &WaylandWindow::pointer_handle_motion,
    &WaylandWindow::pointer_handle_button,
    &WaylandWindow::pointer_handle_axis,
};


// The touch listener callbacks, called from wayland.
const struct wl_touch_listener WaylandWindow::m_wlTouchListener = {
    &WaylandWindow::touch_handle_down,
    &WaylandWindow::touch_handle_up,
    &WaylandWindow::touch_handle_motion,
    &WaylandWindow::touch_handle_frame,
    &WaylandWindow::touch_handle_cancel,
};

static void shellSurfaceHandlePing(void *data, struct wl_shell_surface *shell_surface,
        uint32_t serial)
{
//	printf("[WL]shellSurfaceHandlePing\n");
    wl_shell_surface_pong(shell_surface, serial);
}

static void shellSurfaceHandleConfigure(void *data, struct wl_shell_surface *shell_surface,
         uint32_t edges, int32_t width, int32_t height)
{
//	printf("[WL]shellSurfaceHandleConfigure\n");
}

static void shellSurfaceHandlePopupDone(void *data, struct wl_shell_surface *shell_surface)
{
//	printf("[WL]shellSurfaceHandlePopupDone\n");
}

const struct wl_shell_surface_listener m_wlShellSurfaceListener = {
    shellSurfaceHandlePing,
    shellSurfaceHandleConfigure,
    shellSurfaceHandlePopupDone
};

WaylandWindow* WaylandWindow::m_pWLWndInstance = NULL;

WaylandWindow::WaylandWindow()
    : m_wlData(0)
    , m_wlWindow(0)
    , m_surfaceID(66)
    , m_Width(800)
    , m_Height(480)
    , m_inputService(0)
{
    m_curTouchPoint.id = 0;
    m_curTouchPoint.serial = 0;
    m_curTouchPoint.time = 0;
    m_curTouchPoint.x = 0;
    m_curTouchPoint.y = 0;
}

WaylandWindow::~WaylandWindow()
{
    destroy();
}

WaylandWindow* WaylandWindow::getInstance()
{
    if(m_pWLWndInstance == NULL)
    {
        m_pWLWndInstance = new WaylandWindow();
    }
    return m_pWLWndInstance;
}

bool_t WaylandWindow::create()
{
    bool_t bRet = true;
    m_wlData = createDisplay();
    m_wlWindow = createWindow(m_wlData, m_Width, m_Height);
    if (!m_wlWindow)
    {
        LOG_PRINT(LOG_ERROR, "Fail to create wayland window\n");
        bRet = false;
    }
    else
    {
        wl_surface_damage(m_wlWindow->surface, 0, 0,  m_wlWindow->width, m_wlWindow->height);
        //drawBkgnd(m_wlWindow);

        // Create wayland thread
        //pthread_create(&m_handlerThread, NULL,  handleSurfaceThread, (void *)m_wlData);
        LOG_PRINT(LOG_INFO, "Leaving handleSurfaceThread\n");
        handleSurfaceThread(m_wlData);
    }
    return bRet;
}

bool_t WaylandWindow::create(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    bool_t bRet = true;
    m_wlData = createDisplay();
    m_wlWindow = createWindow(m_wlData, width, height);
    if (!m_wlWindow)
    {
        LOG_PRINT(LOG_ERROR, "Fail to create wayland window\n");
        bRet = false;
    }
    else
    {
        m_Width = width;
        m_Height = height;
        wl_surface_damage(m_wlWindow->surface, x, y,  width, height);
        drawBkgnd(m_wlWindow);

        // Create wayland thread
        pthread_create(&m_handlerThread, NULL,  handleSurfaceThread, (void *)m_wlData);
        LOG_PRINT(LOG_INFO, "Leaving handleSurfaceThread\n");
    }
    return bRet;
}

void WaylandWindow::destroy()
{
    destroyWindow();
    destroyDisplay();
}

WLData* WaylandWindow::createDisplay(void)
{
    struct WLData* pWLData;
    pWLData = new  WLData();
    pWLData->display = wl_display_connect(NULL);

    pWLData->formats = 0;
    pWLData->registry = wl_display_get_registry(pWLData->display);
    pWLData->pData = this;
    wl_registry_add_listener(pWLData->registry,
                             &m_wlRegistryListener, pWLData);
    wl_display_dispatch(pWLData->display);
    wl_display_roundtrip(pWLData->display);
    if (pWLData->shm == NULL)
    {
        LOG_PRINT(LOG_ERROR, "No wl_shm global\n");
        delete pWLData;
        pWLData = NULL;
    }
    else
    {
        wl_display_roundtrip(pWLData->display);
        if (!(pWLData->formats & (1 << WL_SHM_FORMAT_ARGB8888)))
        {
            LOG_PRINT(LOG_ERROR, "WL_SHM_FORMAT_XRGB32 not available\n");
        }
    }
    return pWLData;
}


WLWindow* WaylandWindow::createWindow(WLData *pWLData, int width, int height)
{
    struct WLWindow *pWindow;

    pWindow = new WLWindow();
    if (!pWindow)
    {
        LOG_PRINT(LOG_ERROR, "Failed to create WLWindow");
        return NULL;
    }

    pWindow->callback = NULL;
    pWindow->wlData = pWLData;
    pWindow->width = width;
    pWindow->height = height;
    pWindow->surface = wl_compositor_create_surface(pWLData->compositor);

    eglBindAPI(EGL_OPENGL_ES_API);
    EGLint attributes[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_NONE};

//    EGLint attributes[] = {
//        EGL_SURFACE_TYPE,
//        EGL_WINDOW_BIT,
//        EGL_RED_SIZE, 1,
//        EGL_GREEN_SIZE, 1,
//        EGL_BLUE_SIZE, 1,
//        EGL_ALPHA_SIZE, 1,
//        EGL_RENDERABLE_TYPE,
//        EGL_OPENGL_ES2_BIT,
//        EGL_NONE
//    };

    EGLConfig config;
    EGLint num_config;
    unsigned int iRet = 0;
    egl_display = eglGetDisplay(pWLData->display);
    iRet = eglInitialize(egl_display, NULL, NULL);
    printf("============1==============%d=\n", iRet);

    eglChooseConfig(egl_display, attributes, &config, 1, &num_config);
    printf("============1=============CONFIG=%d=\n", num_config);
    egl_context = eglCreateContext (egl_display, config, EGL_NO_CONTEXT, NULL);
    printf("============2===============\n");
    if (pWLData->wlShell)
    {
        /// Gets the shell surface for the newly created wayland surface.
        pWindow->wlShellSurface = wl_shell_get_shell_surface(pWLData->wlShell, pWindow->surface);
    }

    if (pWindow->wlShellSurface) {
        wl_shell_surface_add_listener(pWindow->wlShellSurface, &m_wlShellSurfaceListener, pWindow);
        wl_shell_surface_set_toplevel(pWindow->wlShellSurface);
        //wl_shell_surface_set_fullscreen(mainWindow.shell_surface, WL_SHELL_SURFACE_FULLSCREEN_METHOD_FILL, 10, mainWindow.output);
    }

    printf("============3===============\n");
    pWindow->eglWindow = wl_egl_window_create(pWindow->surface, width, height);
    egl_surface = eglCreateWindowSurface(egl_display, config, pWindow->eglWindow, NULL);
    if (pWLData->wlShell)
    {
        LOG_PRINT(LOG_ERROR, "Success to create egl window.");
        wl_shell_surface_set_title((wl_shell_surface*)pWindow->wlShellSurface, "keyboard-egl");
    }
    printf("============4===============\n");
    unsigned int iRet2 = 0;
    iRet2 = eglMakeCurrent (egl_display, egl_surface, egl_surface, egl_context);
    printf("============6==============%d=\n", iRet2);

    return pWindow;
}

void WaylandWindow::onMousePress(const MousePoint& mp)
{
}

void WaylandWindow::onMouseMove(const MousePoint& mp)
{
}

void WaylandWindow::onMouseRelease(const MousePoint& mp)
{
}

void WaylandWindow::onMouseWheel()
{
}

void WaylandWindow::onTouchDown(const TouchPoint& tp)
{
    if(m_inputService)
    {
        m_curTouchPoint = tp;
        uint32_t x = 0;
        uint32_t y = 0;
    }
}

void WaylandWindow::onTouchMove(const TouchPoint& tp)
{
    if(m_inputService)
    {
        uint32_t x = 0;
        uint32_t y = 0;
    }
}

void WaylandWindow::onTouchUp(const TouchPoint& tp)
{
    uint32_t x = 0;
    uint32_t y = 0;
    //m_touchConverter.convert(m_curTouchPoint.x, m_curTouchPoint.y, x, y);
    if(m_inputService)
    {
//		uint32_t timeOffset = tp.time - m_curTouchPoint.time;
//      m_inputService->sendTouchActionUp(x, y);
//		if( timeOffset < WL_TOUCH_LONG_PRESS_TICKCOUNT )
//		{
//			m_inputService->sendSingleClick(x, y);
//			printf("=========sendSingleClick: x, y : %d - %d || %d - %d===time : %d=====",m_curTouchPoint.x, m_curTouchPoint.y, x, y, tp.time);
//		}
//		else if(timeOffset > WL_TOUCH_LONG_PRESS_TICKCOUNT && timeOffset < WL_TOUCH_LONG_LONG_PRESS_TICKCOUNT)
//		{
//			m_inputService->sendLongPress(x, y);
//			printf("=========sendLongPress: x, y : %d - %d || %d - %d===time : %d=====",m_curTouchPoint.x, m_curTouchPoint.y, x, y, tp.time);
//		}
//		else
//		{
//			m_inputService->sendTouchActionUp(x, y);
//			printf("=========sendTouchActionUp: x, y : %d - %d || %d - %d===time : %d=====",m_curTouchPoint.x, m_curTouchPoint.y, x, y, tp.time);
//		}
    }
}

void WaylandWindow::destroyDisplay()
{
    if(m_wlData != NULL)
    {
        if (m_wlData->shm)
        {
            wl_shm_destroy(m_wlData->shm);
        }
        if (m_wlData->compositor)
        {
            wl_compositor_destroy(m_wlData->compositor);
        }
        wl_registry_destroy(m_wlData->registry);
        wl_display_flush(m_wlData->display);
        wl_display_disconnect(m_wlData->display);
        delete m_wlData;
        m_wlData = NULL;
    }
}

void WaylandWindow::destroyWindow()
{
    if(m_wlWindow != NULL)
    {
        if (m_wlWindow->callback)
        {
            wl_callback_destroy(m_wlWindow->callback);
        }
        if (m_wlWindow->buffers[0].buffer)
        {
            wl_buffer_destroy(m_wlWindow->buffers[0].buffer);
        }
        if (m_wlWindow->buffers[1].buffer)
        {
            wl_buffer_destroy(m_wlWindow->buffers[1].buffer);
        }
        wl_surface_destroy(m_wlWindow->surface);
        delete m_wlWindow;
        m_wlWindow = NULL;
    }
}

void* WaylandWindow::getWaylandDisplay()
{
    return m_wlData->display;
}

void* WaylandWindow::getWaylandSurface()
{
    return m_wlWindow->surface;
}

uint32_t WaylandWindow::getWidth() const
{
    return m_Width;
}

uint32_t WaylandWindow::getHeight() const
{
    return m_Height;
}

struct WLBuffer* WaylandWindow::createBuffer(struct WLWindow *pWindow)
{
    struct WLBuffer* pBuffer;
    int ret = 0;

    pBuffer = &pWindow->buffers[0];
    if (!pBuffer->buffer)
    {
        ret = create_shm_buffer(pWindow->wlData, pBuffer, pWindow->width, pWindow->height, WL_SHM_FORMAT_ARGB8888);
        if (ret < 0)
        {
            return NULL;
        }
        memset(pBuffer->shm_data, 0xff,  pWindow->width * pWindow->height * 4);
    }
    return pBuffer;
}


void WaylandWindow::registry_handle_global(void *data, struct wl_registry *registry,
                                           uint32_t id, const char *interface, uint32_t version)
{
    struct WLData *pData = (struct WLData *) data;

    if (strcmp(interface, "wl_compositor") == 0)
    {
        pData->compositor = (struct wl_compositor *)wl_registry_bind(registry, id, &wl_compositor_interface, 1);
    }
    else if (strcmp(interface, "xdg_shell") == 0)
    {
    }
    else if (strcmp(interface, "_wl_fullscreen_shell") == 0)
    {
    }
    else if (strcmp(interface, "wl_shm") == 0)
    {
        pData->shm = (wl_shm *) wl_registry_bind(registry,  id, &wl_shm_interface, 1);
        wl_shm_add_listener(pData->shm, &WaylandWindow::m_wlShmListener, pData);
    }
    else if (strcmp(interface, "ivi_application") == 0)
    {
        pData->ivi_application = (ivi_application *)wl_registry_bind(registry, id, &ivi_application_interface, 1);
    }
    else if (strcmp(interface, "wl_shell") == 0)
    {
        printf("[WL]registryHandleGlobal wl_shell\n");
        pData->wlShell = wl_registry_bind(registry, id, &wl_shell_interface, 1);
    }
    else if (strcmp(interface, "wl_seat") == 0)
    {
        pData->seat = (struct wl_seat *)wl_registry_bind(registry, id, &wl_seat_interface, 1);
        if(pData->seat)
        {
            wl_seat_add_listener(pData->seat, &m_wlSeatListener, pData);
        }
    }
}

void WaylandWindow::registry_handle_global_remove(void *data, struct wl_registry *registry, uint32_t name)
{
}


void WaylandWindow::drawBkgnd(void *data)
{
    struct WLWindow *window = (struct WLWindow *) data;
    struct WLBuffer *buffer;

    buffer = createBuffer(window);
    if (!buffer)
    {
        LOG_PRINT(LOG_ERROR, "Failed to create the first buffer.\n" );
        abort();
    }
    paintpixels(buffer->shm_data, window->width, window->height);
    wl_surface_attach(window->surface, buffer->buffer, 0, 0);
    wl_surface_damage(window->surface, 0, 0, window->width, window->height);

    wl_surface_commit(window->surface);
    buffer->busy = 1;
}

void WaylandWindow::handle_ivi_surface_configure(void *data, struct ivi_surface *ivi_surface,
                                                 int32_t width, int32_t height)
{
}

void* WaylandWindow::handleSurfaceThread(void* params)
{
    int ret = 0;
    LOG_PRINT(LOG_INFO, "Surface handler thread started");
    struct WLData* wlData = (struct WLData *)params;
    while (ret != -1)
    {
        ret = wl_display_dispatch_pending(wlData->display);
        draw_window();
    }
    return NULL;
}

void WaylandWindow::seatHandleCapabilities(void *data, struct wl_seat *seat, uint32_t caps)
{
    (void)data;
    (void)seat;
    (void)caps;

    WLData* wlData = (WLData*)data;
    if (caps & WL_SEAT_CAPABILITY_POINTER)
    {
        LOG_PRINT(LOG_INFO, "Register the mouse listener");
        if (!wlData->pointer)
        {
            wlData->pointer = (struct wl_pointer *)wl_seat_get_pointer(seat);
            wl_pointer_add_listener(wlData->pointer, &m_wlPointerListener, wlData);
        }
    }
    if (caps & WL_SEAT_CAPABILITY_KEYBOARD)
    {
    }
    if (caps & WL_SEAT_CAPABILITY_TOUCH)
    {
        LOG_PRINT(LOG_INFO, "Register the touch listener");
        if (!wlData->touch)
        {
            wlData->touch = (struct wl_touch *)wl_seat_get_touch(seat);
            wl_touch_add_listener(wlData->touch,  &m_wlTouchListener, wlData);
        }
    }
}

void WaylandWindow::pointer_handle_enter(void *data, struct wl_pointer *pointer, uint32_t serial,
                                         struct wl_surface *surface, wl_fixed_t sx, wl_fixed_t sy)
{
    LOG_PRINT(LOG_DEBUG, "Pointer enter the window.");
}

void WaylandWindow::pointer_handle_leave(void *data, struct wl_pointer *pointer,
                                         uint32_t serial, struct wl_surface *surface)
{
    LOG_PRINT(LOG_DEBUG, "Pointer leave the window.");
}

void WaylandWindow::pointer_handle_motion(void *data, struct wl_pointer *pointer,
                                          uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
}

void WaylandWindow::pointer_handle_button(void *data, struct wl_pointer *wl_pointer, uint32_t serial,
                                          uint32_t time, uint32_t button, uint32_t state)
{
    LOG_PRINT(LOG_DEBUG, "handle mouse button operation.");
}

void WaylandWindow::pointer_handle_axis(void *data, struct wl_pointer *wl_pointer,
                                        uint32_t time, uint32_t axis, wl_fixed_t value)
{
    LOG_PRINT(LOG_DEBUG, "handle mouse wheel operation.");
}

void WaylandWindow::touch_handle_down(void *data, struct wl_touch *wl_touch, uint32_t serial, uint32_t time, struct wl_surface *surface,
                                      int32_t id, wl_fixed_t x_w, wl_fixed_t y_w)
{
    WLData* pWLData = (WLData*)data;
    TouchPoint tp;
    tp.id = id;
    tp.x  = wl_fixed_to_int(x_w);
    tp.y  = wl_fixed_to_int(y_w);
    tp.serial = serial;
    tp.time = time;
    if( pWLData->pData )
    {
        WaylandWindow* pWindow = (WaylandWindow*)pWLData->pData;
        pWindow->onTouchDown(tp);
    }
}

void WaylandWindow::touch_handle_up(void *data, struct wl_touch *wl_touch,
                                    uint32_t serial, uint32_t time, int32_t id)
{
    WLData* pWLData = (WLData*)data;
    TouchPoint tp;
    tp.id = id;
    tp.x  = 0;
    tp.y  = 0;
    tp.serial = serial;
    tp.time = time;
    if( pWLData->pData )
    {
        WaylandWindow* pWindow = (WaylandWindow*)pWLData->pData;
        pWindow->onTouchUp(tp);
    }
}

void WaylandWindow::touch_handle_motion(void *data, struct wl_touch *wl_touch,
                                        uint32_t time, int32_t id, wl_fixed_t x_w, wl_fixed_t y_w)
{
    WLData* pWLData = (WLData*)data;
    TouchPoint tp;
    tp.id = id;
    tp.x  = wl_fixed_to_int(x_w);
    tp.y  = wl_fixed_to_int(y_w);
    tp.serial = 0;
    tp.time = time;
    if( pWLData->pData )
    {
        WaylandWindow* pWindow = (WaylandWindow*)pWLData->pData;
        pWindow->onTouchMove(tp);
    }
}

void WaylandWindow::touch_handle_frame(void *data, struct wl_touch *wl_touch)
{
}

void WaylandWindow::touch_handle_cancel(void *data, struct wl_touch *wl_touch)
{
}

void WaylandWindow::setWaylandLayerID(uint32_t layerID)
{
    m_surfaceID = layerID;
}

void WaylandWindow::setInputService(void* inputService)
{
    m_inputService = inputService;
}

int WaylandWindow::create_shm_buffer(struct WLData *wlData, struct WLBuffer *buffer,
                             int width, int height, uint32_t format)
{
    struct wl_shm_pool *pool;
    int fd, size, stride;
    void *data;

    stride = width * 4;
    size = stride * height;

    fd = os_create_anonymous_file(size);
    if (fd < 0)
    {
        LOG_PRINT(LOG_ERROR, "creating a buffer file for %d B failed: %m\n",size);
        return -1;
    }

    data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED)
    {
        LOG_PRINT(LOG_ERROR, "mmap failed: %m\n");
        close(fd);
        return -1;
    }
    pool = wl_shm_create_pool(wlData->shm, fd, size);
    buffer->buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride, format);
    //wl_buffer_add_listener(buffer->buffer, &buffer_listener, buffer);
    wl_shm_pool_destroy(pool);
    close(fd);
    buffer->shm_data = data;

    return 0;
}

void WaylandWindow::shm_format(void *data, struct wl_shm *wl_shm, uint32_t format)
{
    struct WLData* wlData = (struct WLData *) data;
    wlData->formats |= (1 << format);
}


void WaylandWindow::initEGL()
{
}

