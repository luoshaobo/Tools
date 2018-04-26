/*
 * Copyright © 2011 Benjamin Franzke
 * Copyright © 2011 Intel Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/epoll.h>
#include <string.h>
#include <stdlib.h>
#include "log.h"
#include <wayland-client.h>

#define ARRAY_LENGTH(a) (sizeof (a) / sizeof (a)[0])

#define TITLE "simple-touch"

struct seat {
	struct touch *touch;
	struct wl_seat *seat;
	struct wl_touch *wl_touch;
};

struct touch {
	struct wl_display *display;
	struct wl_registry *registry;
	struct wl_compositor *compositor;
	struct wl_shell *shell;
	struct wl_shm *shm;
	struct wl_pointer *pointer;
	struct wl_keyboard *keyboard;
	struct wl_surface *surface;
	struct wl_shell_surface *shell_surface;
	struct wl_buffer *buffer;
	int has_argb;
	int width, height;
	void *data;
};

struct touch *touch = NULL;

unsigned char *get_surface_data_buffer()
{
    unsigned char *buf = NULL;
    
    if (touch != NULL) {
        buf =  (unsigned char*)touch->data;
    }
    
    return buf;
}

void surface_update_display(int x, int y, unsigned int width, unsigned int height)
{
    if (touch->surface != NULL && touch->buffer != NULL && touch->display != NULL) {
        wl_surface_attach(touch->surface, touch->buffer, 0, 0);
        wl_surface_damage(touch->surface, x, x, width, height);
        wl_surface_commit(touch->surface);
        wl_display_flush(touch->display);
    }
}

void display_dispatch_input_events()
{
    if (touch != NULL) {
        wl_display_dispatch(touch->display);
    }
}

static int
set_cloexec_or_close(int fd)
{
    long flags;

    if (fd == -1)
        return -1;

    flags = fcntl(fd, F_GETFD);
    if (flags == -1)
        goto err;

    if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1)
        goto err;

    return fd;

err:
    close(fd);
    return -1;
}

static int
create_tmpfile_cloexec(char *tmpname)
{
    int fd;

#ifdef HAVE_MKOSTEMP
    fd = mkostemp(tmpname, O_CLOEXEC);
    if (fd >= 0)
        unlink(tmpname);
#else
    fd = mkstemp(tmpname);
    if (fd >= 0) {
        fd = set_cloexec_or_close(fd);
        unlink(tmpname);
    }
#endif

    return fd;
}

int
os_create_anonymous_file(off_t size)
{
    static const char template1[] = "/weston-shared-XXXXXX";
    const char *path;
    char *name;
    int fd;
    int ret;

    path = getenv("XDG_RUNTIME_DIR");
    if (!path) {
        errno = ENOENT;
        return -1;
    }

    name = (char*)malloc(strlen(path) + sizeof(template1));
    if (!name)
        return -1;

    strcpy(name, path);
    strcat(name, template1);

    fd = create_tmpfile_cloexec(name);

    free(name);

    if (fd < 0)
        return -1;

#ifdef HAVE_POSIX_FALLOCATE
    ret = posix_fallocate(fd, 0, size);
    if (ret != 0) {
        close(fd);
        errno = ret;
        return -1;
    }
#else
    ret = ftruncate(fd, size);
    if (ret < 0) {
        close(fd);
        return -1;
    }
#endif

    return fd;
}

static void
create_shm_buffer(struct touch *touch)
{
    LOG_GEN_PRINTF("\n");
	struct wl_shm_pool *pool;
	int fd, size, stride;

	stride = touch->width * 4;
	size = stride * touch->height;

    LOG_GEN_PRINTF("\n");
	fd = os_create_anonymous_file(size);
	if (fd < 0) {
        LOG_GEN_PRINTF("\n");
		fprintf(stderr, "creating a buffer file for %d B failed: %m\n",
			size);
		exit(1);
	}

    LOG_GEN_PRINTF("\n");
	touch->data =
		mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    LOG_GEN_PRINTF("\n");
	if (touch->data == MAP_FAILED) {
        LOG_GEN_PRINTF("\n");
		fprintf(stderr, "mmap failed: %m\n");
		close(fd);
		exit(1);
	}

    //memset(touch->data, 0x80, size);

    LOG_GEN_PRINTF("\n");
	pool = wl_shm_create_pool(touch->shm, fd, size);
    LOG_GEN_PRINTF("\n");
	touch->buffer =
		wl_shm_pool_create_buffer(pool, 0,
					  touch->width, touch->height, stride,
					  WL_SHM_FORMAT_ARGB8888);
    LOG_GEN_PRINTF("\n");
	wl_shm_pool_destroy(pool);

    LOG_GEN_PRINTF("\n");
	close(fd);
}

static void
shm_format(void *data, struct wl_shm *wl_shm, uint32_t format)
{
    LOG_GEN_PRINTF("\n");
    struct touch *touch =(struct touch*) data;

    if (format == WL_SHM_FORMAT_ARGB8888) {
        LOG_GEN_PRINTF("\n");
		touch->has_argb = 1;
    }
}

struct wl_shm_listener shm_listener = {
	shm_format
};


static void
touch_paint(struct touch *touch, int32_t x, int32_t y, int32_t id)
{
    LOG_GEN_PRINTF("\n");
//	uint32_t *p, c;
//	static const uint32_t colors[] = {
//		0xffff0000,
//		0xffffff00,
//		0xff0000ff,
//		0xffff00ff,
//		0xff00ff00,
//		0xff00ffff,
//	};

//    LOG_GEN_PRINTF("\n");
//	if (id < (int32_t) ARRAY_LENGTH(colors))
//		c = colors[id];
//	else
//		c = 0xffffffff;

//	if (x < 2 || x >= touch->width - 2 ||
//	    y < 2 || y >= touch->height - 2)
//		return;

//	p = (uint32_t *) touch->data + (x - 2) + (y - 2) * touch->width;
//	p[2] = c;
//	p += touch->width;
//	p[1] = c;
//	p[2] = c;
//	p[3] = c;
//	p += touch->width;
//	p[0] = c;
//	p[1] = c;
//	p[2] = c;
//	p[3] = c;
//	p[4] = c;
//	p += touch->width;
//	p[1] = c;
//	p[2] = c;
//	p[3] = c;
//	p += touch->width;
//	p[2] = c;

//    LOG_GEN_PRINTF("\n");
//	wl_surface_attach(touch->surface, touch->buffer, 0, 0);
//    LOG_GEN_PRINTF("\n");
//	wl_surface_damage(touch->surface, x - 2, y - 2, 5, 5);
//	/* todo: We could queue up more damage before committing, if there
//	 * are more input events to handle.
//	 */
//    LOG_GEN_PRINTF("\n");
//	wl_surface_commit(touch->surface);
}

static void
touch_handle_down(void *data, struct wl_touch *wl_touch,
		  uint32_t serial, uint32_t time, struct wl_surface *surface,
		  int32_t id, wl_fixed_t x_w, wl_fixed_t y_w)
{
    LOG_GEN_PRINTF("\n");
    struct touch *touch = (struct touch*)data;
    float x = wl_fixed_to_double(x_w);
    float y = wl_fixed_to_double(y_w);

    touch_paint(touch, x, y, id);
}

static void
touch_handle_up(void *data, struct wl_touch *wl_touch,
		uint32_t serial, uint32_t time, int32_t id)
{
    LOG_GEN_PRINTF("\n");
}

static void
touch_handle_motion(void *data, struct wl_touch *wl_touch,
		    uint32_t time, int32_t id, wl_fixed_t x_w, wl_fixed_t y_w)
{
    LOG_GEN_PRINTF("\n");
    struct touch *touch = (struct touch*)data;
    float x = wl_fixed_to_double(x_w);
    float y = wl_fixed_to_double(y_w);

    touch_paint(touch, x, y, id);
}

static void
touch_handle_frame(void *data, struct wl_touch *wl_touch)
{
    LOG_GEN_PRINTF("\n");
}

static void
touch_handle_cancel(void *data, struct wl_touch *wl_touch)
{
    LOG_GEN_PRINTF("\n");
}

static const struct wl_touch_listener touch_listener = {
	touch_handle_down,
	touch_handle_up,
	touch_handle_motion,
	touch_handle_frame,
	touch_handle_cancel,
};

static void
seat_handle_capabilities(void *data, struct wl_seat *wl_seat,
			 enum wl_seat_capability caps)
{
    LOG_GEN_PRINTF("\n");
    struct seat *seat = (struct seat*)data;
	struct touch *touch = seat->touch;

    LOG_GEN_PRINTF("\n");
	if ((caps & WL_SEAT_CAPABILITY_TOUCH) && !seat->wl_touch) {
        LOG_GEN_PRINTF("attach touch\n");
		seat->wl_touch = wl_seat_get_touch(wl_seat);
		wl_touch_set_user_data(seat->wl_touch, touch);
		wl_touch_add_listener(seat->wl_touch, &touch_listener, touch);
	} else if (!(caps & WL_SEAT_CAPABILITY_TOUCH) && seat->wl_touch) {
        LOG_GEN_PRINTF("detach touch\n");
		wl_touch_destroy(seat->wl_touch);
		seat->wl_touch = NULL;
	}
}

static const struct wl_seat_listener seat_listener = {
    (void (*)(void*, wl_seat*, uint32_t))seat_handle_capabilities,
};

static void
add_seat(struct touch *touch, uint32_t name, uint32_t version)
{
    LOG_GEN_PRINTF("\n");
	struct seat *seat;

    LOG_GEN_PRINTF("\n");
    seat = (struct seat*)malloc(sizeof *seat);
	assert(seat);

	seat->touch = touch;
	seat->wl_touch = NULL;
    LOG_GEN_PRINTF("\n");
    seat->seat = (wl_seat*)wl_registry_bind(touch->registry, name,
                      &wl_seat_interface, 1);
    LOG_GEN_PRINTF("\n");
	wl_seat_add_listener(seat->seat, &seat_listener, seat);
}

static void
handle_ping(void *data, struct wl_shell_surface *shell_surface,
	    uint32_t serial)
{
    LOG_GEN_PRINTF("\n");
	wl_shell_surface_pong(shell_surface, serial);
}

static void
handle_configure(void *data, struct wl_shell_surface *shell_surface,
		 uint32_t edges, int32_t width, int32_t height)
{
    LOG_GEN_PRINTF("\n");
}

static void
handle_popup_done(void *data, struct wl_shell_surface *shell_surface)
{
    LOG_GEN_PRINTF("\n");
}

static const struct wl_shell_surface_listener shell_surface_listener = {
	handle_ping,
	handle_configure,
	handle_popup_done
};

static void
handle_global(void *data, struct wl_registry *registry,
	      uint32_t name, const char *interface, uint32_t version)
{
    LOG_GEN_PRINTF("\n");
    struct touch *touch = (struct touch*)data;

	if (strcmp(interface, "wl_compositor") == 0) {
        LOG_GEN_PRINTF("wl_compositor\n");
		touch->compositor =
            (wl_compositor*)wl_registry_bind(registry, name,
                     &wl_compositor_interface, 1);
	} else if (strcmp(interface, "wl_shell") == 0) {
        LOG_GEN_PRINTF("wl_shell\n");
		touch->shell =
            (wl_shell*)wl_registry_bind(registry, name,
                     &wl_shell_interface, 1);
	} else if (strcmp(interface, "wl_shm") == 0) {
        LOG_GEN_PRINTF("wl_shm\n");
        touch->shm = (wl_shm*)wl_registry_bind(registry, name,
                          &wl_shm_interface, 1);
		wl_shm_add_listener(touch->shm, &shm_listener, touch);
	} else if (strcmp(interface, "wl_seat") == 0) {
        LOG_GEN_PRINTF("wl_seat\n");
		add_seat(touch, name, version);
	}
}

static void
handle_global_remove(void *data, struct wl_registry *registry, uint32_t name)
{
    LOG_GEN_PRINTF("\n");
}

static const struct wl_registry_listener registry_listener = {
	handle_global,
	handle_global_remove
};

struct ColorARGB { unsigned char b, g, r, a; };

void fill_rect_argb(void *pBuf, int width, int height)
{
    ColorARGB *pPixelBuf = (ColorARGB *)pBuf;
    int i, j;

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            ColorARGB *p = &pPixelBuf[i * width + j];
            p->a = 0xFF;
            p->r = 0x00;
            p->g = 0x00;
            p->b = 0x00;
        }
    }
}

static struct touch *
touch_create(int width, int height)
{
	struct touch *touch;

    LOG_GEN_PRINTF("\n");
    touch = (struct touch*)malloc(sizeof *touch);
	if (touch == NULL) {
		fprintf(stderr, "out of memory\n");
		exit(1);
	}
    LOG_GEN_PRINTF("\n");
	touch->display = wl_display_connect(NULL);
	assert(touch->display);

    LOG_GEN_PRINTF("\n");
	touch->has_argb = 0;
    LOG_GEN_PRINTF("\n");
	touch->registry = wl_display_get_registry(touch->display);
    LOG_GEN_PRINTF("\n");
	wl_registry_add_listener(touch->registry, &registry_listener, touch);
    LOG_GEN_PRINTF("\n");
	wl_display_dispatch(touch->display);
    LOG_GEN_PRINTF("\n");
	wl_display_roundtrip(touch->display);

    LOG_GEN_PRINTF("\n");
	if (!touch->has_argb) {
		fprintf(stderr, "WL_SHM_FORMAT_ARGB32 not available\n");
		exit(1);
	}

    LOG_GEN_PRINTF("\n");
	touch->width = width;
	touch->height = height;
    LOG_GEN_PRINTF("\n");
	touch->surface = wl_compositor_create_surface(touch->compositor);
    LOG_GEN_PRINTF("\n");
	touch->shell_surface = wl_shell_get_shell_surface(touch->shell,
							  touch->surface);
    LOG_GEN_PRINTF("\n");
	create_shm_buffer(touch);

    LOG_GEN_PRINTF("\n");
	if (touch->shell_surface) {
        LOG_GEN_PRINTF("\n");
		wl_shell_surface_add_listener(touch->shell_surface,
					      &shell_surface_listener, touch);
        LOG_GEN_PRINTF("\n");
		wl_shell_surface_set_toplevel(touch->shell_surface);
	}

    LOG_GEN_PRINTF("\n");
	wl_surface_set_user_data(touch->surface, touch);
    LOG_GEN_PRINTF("\n");
    wl_shell_surface_set_title(touch->shell_surface, TITLE);

    LOG_GEN_PRINTF("\n");
    //memset(touch->data, 0x80, width * height * 4);
    //fill_rect_argb(touch->data, width, height);
    LOG_GEN_PRINTF("\n");
	wl_surface_attach(touch->surface, touch->buffer, 0, 0);
    LOG_GEN_PRINTF("\n");
	wl_surface_damage(touch->surface, 0, 0, width, height);
    LOG_GEN_PRINTF("\n");
	wl_surface_commit(touch->surface);

    LOG_GEN_PRINTF("\n");
	return touch;
}

int simple_touch_main()
{
	
	int ret = 0;

    LOG_GEN_PRINTF("\n");
	touch = touch_create(1280, 720);

    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "ivi-shell -a %s:255 &", TITLE);
    system(cmd);
    snprintf(cmd, sizeof(cmd), "ivi-shell -z %s:255 &", TITLE);
    system(cmd);

    // LOG_GEN_PRINTF("\n");
    // while (ret != -1) {
        // LOG_GEN_PRINTF("\n");
		// ret = wl_display_dispatch(touch->display);
    // }

    LOG_GEN_PRINTF("\n");
	return 0;
}
