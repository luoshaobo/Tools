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

#include <stdlib.h>
#include <stdint.h>
#include "wayland-util.h"

extern const struct wl_interface ivi_surface_interface;
extern const struct wl_interface wl_surface_interface;

static const struct wl_interface *types[] = {
	NULL,
	NULL,
	NULL,
	&wl_surface_interface,
	&ivi_surface_interface,
};

static const struct wl_message ivi_surface_requests[] = {
	{ "destroy", "", types + 0 },
};

static const struct wl_message ivi_surface_events[] = {
	{ "visibility", "i", types + 0 },
	{ "configure", "ii", types + 0 },
};

const struct wl_interface ivi_surface_interface = {
	"ivi_surface", 1,
	1, ivi_surface_requests,
	2, ivi_surface_events,
};

static const struct wl_message ivi_application_requests[] = {
	{ "surface_create", "uon", types + 2 },
};

const struct wl_interface ivi_application_interface = {
	"ivi_application", 1,
	1, ivi_application_requests,
	0, NULL,
};

