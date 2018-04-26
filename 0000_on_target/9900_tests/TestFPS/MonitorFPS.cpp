/**
 * @file
 *          sem_hmi_mon.c
 * @brief
 *          Monitors the the GPU load and FSP of HMI applications
 *
 * @par CM-Info:
 *          - %name:           sem_hmi_mon.c %
 *          - %full_filespec:  sem_hmi_mon.c-OvipRbt#3.1.6:csrc:OvipIas#1 %
 *          - %date_created:   Mon Jun 22 17:05:44 2015 %
 *          - %derived_by:     uidv1425 %
 *
 * @par Project or Platform:
 *          OVIP
 * @par OS-Version:
 *          Linux Monta-Vista
 * @par SW-Component:
 *          System Software (SSW)
 * @par SW-Package:
 *          System Services
 * @par SW-Module:
 *          Diagnostics
 *
 * @par Bugs:
 *          - None
 * @note
 *          - N/A
 *
 * @warning
 *          - N/A
 *
 * @par Description:*
 *
 * @par Module-History:
 * @verbatim
 *  Date          Author                Reason
 *  21.05.2015    Claudiu Bors			Added a readable name for surfaces
 *  20.05.2015    Claudiu Bors			Fix for OvipRbt#35770
 *  14.05.2015    Claudiu Bors			Fixed klockwork issues in ovip-ssw-sem-hmi-mon code cw1520
 *  30.04.2015    Claudiu Bors			Fixed OvipRbt#23945
 *  03.04.2015    Claudiu Bors			Added NULL check for clean-up function
 *  27.08.2014    Claudiu Bors          Fixed memory leaks
 *  01.08.2014    Claudiu Bors          Added notification when the cheat code is active
 *  03.07.2014    Claudiu Bors			Initial version
 *
 * @endverbatim
 *
 * @par Copyright Notice:
 * @verbatim
 * Copyright (c) Continental AG and subsidiaries 2013
 * Alle Rechte vorbehalten. All Rights Reserved.
 * The reproduction, transmission or use of this document or its contents is
 * not permitted without express written authority.
 * Offenders will be liable for damages. All rights, including rights created
 * by patent grant or registration of a utility model or design, are reserved.
 *
 * @endverbatim
 */
#include <wayland-client.h>
#include <wayland-ivi/wayland-ivi-shell-client-protocol.h>
#include <wayland-client-protocol.h>
#include <wayland-ivi/wayland-ivi-client-util.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define FPS_FRAMES (61)

#define bool_t bool
#define str_t char

/**
 * Data structure used an array of deltas
 * And time stamp for last frame received and current frame
 */
struct fps_counter {
	uint32_t dts[FPS_FRAMES];
	uint32_t current;
	uint32_t last;
};

/**
 * Data structure used to store all information used by a specific  surface
 */
struct surface_info {
	struct wl_list m_link;
	struct wl_ivi_observer *observers;
	char *surf_name;
	char *readable_name;
	bool readable;
	bool old_info;

	struct fps_counter fps;
};

/**
 * Weston structure used to store applications specific information
 * and a list of all surfaces connected to the shell
 */
typedef struct {
	struct wl_display *display;
	struct wl_registry *registry;
	struct wl_ivi_shell *ivi_shell;
	struct wl_list surfaces;
	uint32_t last_show;
} wayland_globals_t;

static uint32_t timestamp(void);
static void fps_update(struct surface_info * const si, uint32_t rTime);
static int cmp_uint32(void const *va, void const *vb);
static void fps_detailed(struct fps_counter const * fps_count, float *median, float *max, float *min);
static void observe(wayland_globals_t *wl, char const *surface_name);
static inline float nonzero(uint32_t num);
static void show_fps_info(wayland_globals_t * wl, uint32_t crtime);
static void ivi_surface_deregistered(void *derg_data, struct wl_ivi_shell *ivishell, const char* surface_id);
static void observer_attached(void *wl_data, struct wl_ivi_observer *wl_observer, int32_t dx, int32_t dy, int32_t sWidth, int32_t sHeight);
static void ivi_surface_registered(void *wl_data, struct wl_ivi_shell *shell, const char* surfaceName);
static void observer_destroyed(void *data, struct wl_ivi_observer *wl_ivi_observer);
static void observer_frame(void *wl_data, struct wl_ivi_observer *wl_observer, uint32_t oTime);
static void registry_handle_global(void *void_data, struct wl_registry *WLregistry, uint32_t reg_id, const char *interface_name, uint32_t vers);
static void registry_handle_global_remove(void *da, struct wl_registry *reg, uint32_t ID);
static void clean_up(const wayland_globals_t* wl);
bool_t create_readble_name(const char* surface_id, char ** readable_name);

static volatile bool g_running = true;

/**
 * Returns the current time stamp in milliseconds
 * @return the current time stamp in milliseconds
 */
static uint32_t timestamp(void)
{
	struct timespec ts;
	memset(&ts, (int) 0, sizeof (struct timespec));
	clock_gettime((clockid_t) CLOCK_MONOTONIC, &ts);
	uint32_t time_stamp = (uint32_t) ts.tv_nsec / 1000000U;
	time_stamp += (uint32_t) ts.tv_sec * 1000U;
	return time_stamp;
}

/**
 * Added frame info to surface_info
 * @param si surface info pointer
 * @param rTime the timestamp when frames are received
 */
static void fps_update(struct surface_info * const si, uint32_t rTime)
{
	uint32_t dt = si->fps.last == 0 ? 1 : (rTime - si->fps.last);
	si->fps.last = rTime;
	si->fps.dts[si->fps.current++] = dt;
	si->fps.current %= FPS_FRAMES;
}

static void observer_attached(void *wl_data,
		struct wl_ivi_observer *wl_observer,
		int32_t dx,
		int32_t dy,
		int32_t sWidth,
		int32_t sHeight)
{
}

static void observer_frame(void *wl_data, struct wl_ivi_observer *wl_observer, uint32_t oTime)
{
	struct surface_info *si = wl_data;
	fps_update(si, oTime);
}
static void observer_destroyed(void *data, struct wl_ivi_observer *wl_ivi_observer){
}


struct wl_ivi_observer_listener ivi_observer_listener = {
		&observer_attached,
		&observer_frame,
		&observer_destroyed,
};

/**
 * Checkes  that a numbers is not 0  to avoid dividing by 0
 * @param n the number to be checked
 * @return n or 1 in case of n=0
 */
static inline float nonzero(uint32_t num)
{
	return num != 0 ? num : 1.0f;
}

/**
 * Generic function for comparing
 * @param va value A
 * @param vb value B
 * @return returns the diferance between va and vb
 */
static int cmp_uint32(void const *va, void const *vb)
{
	const uint32_t temp_a = *(const uint32_t*) va;
	const uint32_t temp_b = *(const uint32_t*) vb;
	int ret = 0;

	if (temp_a == temp_b) {
		ret = 0;
	} else if (temp_a > temp_b) {
		ret = 1;
	} else {
		ret = -1;
	}
	return ret;
}

/**
 * Calculates the the frame rate from an observed surface.
 * @param fps input the frame rate structure of a surface
 * @param median
 * @param max
 */
static void fps_detailed(struct fps_counter const * fps_count, float *median, float *max, float *min)
{
	uint32_t framesArr[FPS_FRAMES];
	int j;

	for (j = 0; j < FPS_FRAMES; j++) {
		framesArr[j] = fps_count->dts[j];
	}

	qsort(framesArr, (size_t) FPS_FRAMES, sizeof (framesArr[0]), cmp_uint32);

	*median = 1000.f / nonzero(framesArr[FPS_FRAMES / 2]);
	*max = 1000.f / nonzero(framesArr[0]);
    *min = 1000.f / nonzero(framesArr[FPS_FRAMES - 1]);
}

/**
 * Shows the frame rate for each surface
 * @wl weston global list
 * @crtime current time
 */
static void show_fps_info(wayland_globals_t * wl, uint32_t crtime)
{
    uint32_t time_out = 300000U;

	wl->last_show = crtime;
	struct surface_info *si;

	str_t sValue[1000];
	memset(sValue, (int) 0, sizeof (sValue));
	strcat(sValue, "FPS of last 5 minutes:\n");

	wl_list_for_each(si, &wl->surfaces, m_link)
	{
        float max = 0.0f, min = 0.0f, median = 0.0f;
        fps_detailed(&si->fps, &median, &max, &min);

		if ((si->fps.last + time_out > crtime) && ((median < 1000.0f) && (max < 1000.0f))) {
			str_t temp[70];
			if( si->readable == true)
			{
                snprintf(temp, sizeof (temp), "%s: min=%.2f, med=%.2f, max=%.2f\n", si->readable_name, min, median, max);
			}
			else
			{
                snprintf(temp, sizeof (temp), "%s: min=%.2f, med=%.2f, max=%.2f\n", si->surf_name, min, median, max);
			}
			strncat(sValue , temp, (size_t)(sizeof (sValue) - strlen(sValue) - 1U));
		}
		else
		{
			si->old_info = true;
		}
	}
    printf("%s\n", sValue);
}

/**
 * initiates a surface and adds  the flags it should observe
 */

static void observe(wayland_globals_t *wl, char const *surface_name)
{
	struct surface_info *infop;
	infop = calloc((size_t) 1, sizeof (struct surface_info));
	if (infop != NULL)
	{
		infop->surf_name = strdup(surface_name);
		if(infop->surf_name == NULL)
		{
			free(infop);
		}
		else
		{
			infop->readable = create_readble_name(surface_name, &infop->readable_name);
			infop->observers = wl_ivi_shell_observe(wl->ivi_shell, surface_name, WL_IVI_SHELL_OBSERVABLE_FRAME);
			if (infop->observers != NULL )
			{
				wl_ivi_observer_add_listener(infop->observers, &ivi_observer_listener, infop);
				infop->old_info = false;
				wl_list_insert(wl->surfaces.next, &infop->m_link);
			}
		}
	}
	else
	{
	}
}
/**
 * Call back for ivi_shell surface register
 */

static void ivi_surface_registered(void *wl_data, struct wl_ivi_shell *shell, const char* surfaceName)
{
		wayland_globals_t* wl = (wayland_globals_t*) wl_data;
		observe(wl, surfaceName);

}
/**
 * Call back for ivi_shell surface deregistered
 */
static
void ivi_surface_deregistered(void *derg_data, struct wl_ivi_shell *ivishell, const char* surface_id)
{
	wayland_globals_t* wl = (wayland_globals_t*) derg_data;
	struct surface_info *si, *tmp;

	wl_list_for_each_safe(si, tmp, &wl->surfaces, m_link)
	{
		if ((strcmp(si->surf_name, surface_id) == 0) && (si->old_info == true ))
		{
			free(si->surf_name);
			if( si->readable == true)
			{
				free(si->readable_name);
			}
			if(si->observers != NULL){
				wl_proxy_destroy((struct wl_proxy *) si->observers);
			}
			wl_list_remove(&si->m_link);
			free(si);
		}
	}
}

struct wl_ivi_shell_listener ivi_shell_listener = {
		&ivi_surface_registered,
		&ivi_surface_deregistered,
};

static void registry_handle_global(void *void_data, struct wl_registry *WLregistry, uint32_t reg_id, const char *interface_name, uint32_t vers)
{
	wayland_globals_t* wl = (wayland_globals_t*) void_data;

	if (strcmp(interface_name, "wl_ivi_shell") == 0) {
		wl->ivi_shell = wl_registry_bind(WLregistry, (uint32_t) reg_id, &wl_ivi_shell_interface, (uint32_t) vers);
		if (wl->ivi_shell != NULL) {
			wl_ivi_shell_add_listener(wl->ivi_shell, &ivi_shell_listener, wl);
		}
	}
}

static void registry_handle_global_remove(void *da, struct wl_registry *reg, uint32_t ID)
{
}

struct wl_registry_listener registry_listener = {
		&registry_handle_global,
		&registry_handle_global_remove,
};

static void clean_up(const wayland_globals_t *wl)
{
	struct surface_info *surf, *p;

	wl_list_for_each_safe(surf, p, &wl->surfaces, m_link)
	{
		free(surf->surf_name);
		if( surf->readable == true)
		{
			free(surf->readable_name);
		}
		wl_list_remove(&surf->m_link);
		if(surf->observers != NULL){
			wl_proxy_destroy((struct wl_proxy *) surf->observers);
		}
		free(surf);
	}

	if(wl->registry != NULL)
	{
		wl_registry_destroy(wl->registry);
	}
	if(wl->ivi_shell != NULL)
	{
		wl_ivi_shell_destroy(wl->ivi_shell);
	}
	if(wl->display != NULL)
	{
		wl_display_disconnect(wl->display);
	}
}
/*
 * Tries to create a readable name for a surface_id
 * "1.2817.surfaces.zones.uifwk.ovip"
 * pid = the application id of the application
 * zorder = the Z order of the surface registered
 * Return true if success in creating a readable name
 * */

bool_t create_readble_name(const char * surface_id ,char ** readable_name)
{
	bool_t ret = false;
	char name[64];
	int app_pid = 0;
	int zorder = 0;
	const unsigned int max_num_matches = 2;

	/*surface_id = 1.2817.surfaces.zones.uifwk.ovip */
	int matches = sscanf(surface_id,"%d.%d", &zorder, &app_pid);
	/*if both matches are casted into integers*/
	if (matches == max_num_matches )
	{
		sprintf(name, "/proc/%d/comm", app_pid);
		FILE* f = fopen(name,"r");
		if(f)
		{
			size_t size;
			size = fread(name, sizeof(char), 64, f);
			if(size > 0)
			{
				/*remove the new line terminator that is read from the file*/
				if(name[size-1] == '\n')
				{
					name[size-1] = '\0';
				}
			}
			fclose( f );
		}

		char temp[128];
		/*Apped the Z order to the sur*/
		snprintf(temp, sizeof (temp), "%s-Z%d", name, zorder);

		if(readable_name != NULL )
		{
			*readable_name =strdup(temp);
			if(readable_name != NULL)
			{
				ret = true;
			}
		}
	}

	return ret;
}

int main1(int argc, char* argv[])
{
	uint32_t crtime = timestamp();
	wayland_globals_t wl;

	memset(&wl, (int) 0, sizeof (wl));
	wl_list_init(&wl.surfaces);

	wl.display = wl_display_connect(NULL);
	if (wl.display) {
		wl.registry = wl_display_get_registry(wl.display);
		if (wl.registry != NULL) {
			wl_registry_add_listener(wl.registry, &registry_listener, &wl);
		} else {
		}
		wl_display_roundtrip(wl.display);

		if (wl.ivi_shell) {
			int ret = 0;
			while ((g_running== true ) && (ret != -1)) {
                if (true) {
					wl_display_flush(wl.display);
					ret = wl_display_dispatch(wl.display);
					crtime = timestamp();
					if ((crtime - wl.last_show >= 5000U)) {
						show_fps_info(&wl, crtime);
                    } else {
                        //printf("loop\n");
                    }
				}
				else{
					sleep(1U);
				}
			}
		} else {
		}
	} else {
	}

	clean_up(&wl);

	return 0;
}
