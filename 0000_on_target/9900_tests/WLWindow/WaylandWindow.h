
#ifndef WAYLANDWINDOW_H
#define WAYLANDWINDOW_H

#include <pthread.h>
#include <wayland-client.h>
#include <wayland-egl.h>
#include "ivi-application-client-protocol.h"
#include "ssw_types.h"

struct WLBuffer
{
    struct wl_buffer*	buffer;
    void*				shm_data;
    int32_t				busy;
};

struct WLData
{
    struct wl_display *			display;
    struct wl_registry *		registry;
    struct wl_compositor*       compositor;
    struct wl_shm	*           shm;
    struct wl_shell*            wlShell;
    struct ivi_application*     ivi_application;
    struct wl_seat*				seat;
    struct wl_pointer*			pointer;
    struct wl_list				touchPointList;
    struct wl_touch*			touch;
    unsigned int 				formats;
    void*						pData;
};

struct WLWindow
{
    struct WLData *          wlData;
    struct wl_surface*		 surface;
    struct wl_shell_surface* wlShellSurface;
    struct ivi_surface*		 ivi_surface;
    struct wl_egl_window*    eglWindow;
    struct WLBuffer			 buffers[2];
    struct WLBuffer*		 prev_buffer;
    struct wl_callback*	     callback;
    int32_t					 width;
    int32_t				     height;
};

struct MousePoint
{
    int32_t		id;
    int32_t		x;
    int32_t		y;
};

struct TouchPoint
{
    int32_t		id;
    int32_t		x;
    int32_t		y;
    uint32_t  serial;
    uint32_t  time;
};

class WaylandWindow
{
protected:
    WaylandWindow();

public:
    virtual ~WaylandWindow();

    virtual bool_t create();

    virtual bool_t create(int32_t x, int32_t y, uint32_t width, uint32_t height);

    virtual void destroy();

    virtual void onMousePress(const MousePoint& mp);

    virtual void onMouseMove(const MousePoint& mp);

    virtual void onMouseRelease(const MousePoint& mp);

    virtual void onMouseWheel();

    virtual void onTouchDown(const TouchPoint& tp);

    virtual void onTouchMove(const TouchPoint&  tp);

    virtual void onTouchUp(const TouchPoint& tp);

    virtual void drawBkgnd(void *data);

    virtual void* getWaylandDisplay();

    virtual void* getWaylandSurface();

    virtual uint32_t getWidth() const;

    virtual uint32_t getHeight() const;

    virtual void setWaylandLayerID(uint32_t layerID);

    virtual void setInputService(void* inputService);

    static WaylandWindow* getInstance();

private:
    static void registry_handle_global(void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version);

    static void registry_handle_global_remove(void *data, struct wl_registry *registry, uint32_t name);

    static void handle_ivi_surface_configure(void *data, struct ivi_surface *ivi_surface,  int32_t width, int32_t height);

    static void*  handleSurfaceThread(void *inArg);

    static void registry_global_listener(void *data, struct wl_registry *registry,
                        uint32_t name, const char *interface, uint32_t version);

    static void handle_seat_capabilities(void *data, struct wl_seat *wl_seat,
                                         uint32_t caps);

    static struct WLBuffer* createBuffer(struct WLWindow *pWindow);

    static void seatHandleCapabilities(void *data, struct wl_seat *wl_seat, uint32_t caps);

    static void pointer_handle_enter(void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t sx, wl_fixed_t sy);

    static void pointer_handle_leave(void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface);

    static void pointer_handle_motion(void *data, struct wl_pointer *pointer, uint32_t time, wl_fixed_t sx, wl_fixed_t sy);

    static void pointer_handle_button(void *data, struct wl_pointer *wl_pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);

    static void pointer_handle_axis(void *data, struct wl_pointer *wl_pointer,uint32_t time, uint32_t axis, wl_fixed_t value);

    static void touch_handle_down(void *data, struct wl_touch *wl_touch, uint32_t serial, uint32_t time, struct wl_surface *surface,
                     int32_t id, wl_fixed_t x_w, wl_fixed_t y_w);

    static void touch_handle_up(void *data, struct wl_touch *wl_touch, uint32_t serial, uint32_t time, int32_t id);

    static void touch_handle_motion(void *data, struct wl_touch *wl_touch, uint32_t time, int32_t id, wl_fixed_t x_w, wl_fixed_t y_w);

    static void touch_handle_frame(void *data, struct wl_touch *wl_touch);

    static void touch_handle_cancel(void *data, struct wl_touch *wl_touch);

    static int32_t create_shm_buffer(struct WLData *wlData, struct WLBuffer *buffer, int width, int height, uint32_t format);

    static void shm_format(void *data, struct wl_shm *wl_shm, uint32_t format);

    WLData* createDisplay(void);

    WLWindow* createWindow(WLData *display, int width, int height);


    void destroyDisplay();

    void destroyWindow();

    void initEGL();

private:
    static const struct wl_registry_listener	m_wlRegistryListener;
    static const struct wl_seat_listener			m_wlSeatListener;
    static const struct wl_callback_listener	m_wlFrameListener;
    static const struct ivi_surface_listener	m_wlIviSurfaceListener;
    static const struct wl_touch_listener		m_wlTouchListener;
    static const struct wl_pointer_listener	m_wlPointerListener;
    static const struct wl_shm_listener			m_wlShmListener;

    WLData*				m_wlData;
    WLWindow*		m_wlWindow;
    pthread_t			m_handlerThread;
    static WaylandWindow* m_pWLWndInstance;

    uint32_t			m_surfaceID;
    uint32_t			m_Width;
    uint32_t			m_Height;
    bool					m_bUseIviShell;
    TouchPoint		m_curTouchPoint;

    void*			m_inputService;
};

#endif
