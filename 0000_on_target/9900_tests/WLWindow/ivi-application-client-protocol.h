/**
 * @file
 *          WaylandWindow.h
 *
 * @brief
 *
 * @par Author (last changes):
 *          - Hui Wang
 *          - Phone +86 (0)21 6080 4221
 *          - Hui.6.Wang@continental-corporation.com
 *
 * @par Project Leader:
 *          - Jie Zeng
 *          - Phone +86 (0)21 6080 3395
 *          - jie.zeng@continental-corporation.com
 *
 * @par Responsible Architect:
 *          - Shouruo Qin
 *          - Phone +86 (0)21 6080 3167
 *          - shouruo.qing@continental-corporation.com
 *
 * @par Project:
 *          PSA WAVE3 OVIP NAC RCC
 *
 * @par SW-Component:
 *          carlife
 *
 * @par SW-Package:
 *          carlife
 *
 * @par SW-Module:
 *          svc
 *
 * @par Description:
 *          Play media stream and video stream
 *
 * @note
 *
 * @par Module-History:
 *
 * @verbatim
 *
 *  Date            Author                  Reason
 *  24.11.2015      Hui Wang             Initial version.
 *
 * @endverbatim
 *
 * @par Copyright Notice:
 *
 * Copyright (c) Continental AG and subsidiaries 2015
 * Continental Automotive Holding (Shanghai)
 * Alle Rechte vorbehalten. All Rights Reserved.

 * The reproduction, transmission or use of this document or its contents is
 * not permitted without express written authority.
 * Offenders will be liable for damages. All rights, including rights created
 * by patent grant or registration of a utility model or design, are reserved.
 */

#ifndef IVI_APPLICATION_CLIENT_PROTOCOL_H
#define IVI_APPLICATION_CLIENT_PROTOCOL_H

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include "wayland-client.h"

struct wl_client;
struct wl_resource;

struct ivi_surface;
struct ivi_application;

extern const struct wl_interface ivi_surface_interface;
extern const struct wl_interface ivi_application_interface;

/**
 * ivi_surface - application interface to surface in ivi compositor
 * @visibility: visibility of surface in ivi compositor has changed
 * @configure: (none)
 *
 * 
 */
struct ivi_surface_listener {
	/**
	 * visibility - visibility of surface in ivi compositor has
	 *	changed
	 * @visibility: (none)
	 *
	 * The new visibility state is provided in argument visibility.
	 * If visibility is 0, the surface has become invisible. If
	 * visibility is not 0, the surface has become visible.
	 */
	void (*visibility)(void *data,
			   struct ivi_surface *ivi_surface,
			   int32_t visibility);
	/**
	 * configure - (none)
	 * @width: (none)
	 * @height: (none)
	 */
	void (*configure)(void *data,
			  struct ivi_surface *ivi_surface,
			  int32_t width,
			  int32_t height);
};

static inline int
ivi_surface_add_listener(struct ivi_surface *ivi_surface,
			 const struct ivi_surface_listener *listener, void *data)
{
	return wl_proxy_add_listener((struct wl_proxy *) ivi_surface,
				     (void (**)(void)) listener, data);
}

#define IVI_SURFACE_DESTROY	0

static inline void
ivi_surface_set_user_data(struct ivi_surface *ivi_surface, void *user_data)
{
	wl_proxy_set_user_data((struct wl_proxy *) ivi_surface, user_data);
}

static inline void *
ivi_surface_get_user_data(struct ivi_surface *ivi_surface)
{
	return wl_proxy_get_user_data((struct wl_proxy *) ivi_surface);
}

static inline void
ivi_surface_destroy(struct ivi_surface *ivi_surface)
{
	wl_proxy_marshal((struct wl_proxy *) ivi_surface,
			 IVI_SURFACE_DESTROY);

	wl_proxy_destroy((struct wl_proxy *) ivi_surface);
}

#define IVI_APPLICATION_SURFACE_CREATE     0

static inline void
ivi_application_set_user_data(struct ivi_application *ivi_application, void *user_data)
{
	wl_proxy_set_user_data((struct wl_proxy *) ivi_application, user_data);
}

static inline void *
ivi_application_get_user_data(struct ivi_application *ivi_application)
{
	return wl_proxy_get_user_data((struct wl_proxy *) ivi_application);
}

static inline void
ivi_application_destroy(struct ivi_application *ivi_application)
{
	wl_proxy_destroy((struct wl_proxy *) ivi_application);
}

static inline struct ivi_surface *
ivi_application_surface_create(struct ivi_application *ivi_application, uint32_t ivi_id, struct wl_surface *surface)
{
	struct wl_proxy *id;

	id = wl_proxy_marshal_constructor((struct wl_proxy *) ivi_application,
			 IVI_APPLICATION_SURFACE_CREATE, &ivi_surface_interface, ivi_id, surface, NULL);

	return (struct ivi_surface *) id;
}

#ifdef  __cplusplus
}
#endif

#endif
