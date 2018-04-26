#include "WaylandIviShell.h"
#include <QDebug>
//#include <CLogger.h>

static QString g_logContex;

#define LOG(ctx) qDebug()
#define LOG_INFO(ctx) qDebug()
#define LOG_WARNING(ctx) qDebug()
#define LOG_ERROR(ctx) qDebug()
#define LOG_FATAL(ctx) qDebug()
#define LOG_DEBUG(ctx) qDebug()
#define LOG_VERBOSE(ctx) qDebug()


using uifwk::zones::WaylandIviShell;

void when_surface_registered(const char *surface_id);

static void surface_registered(void *data,
                   struct wl_ivi_shell *wl_ivi_shell,
                   const char *surface_id)
{
    LOG_INFO("Surf") << "=== surface_registered: " << surface_id;
    when_surface_registered(surface_id);
}

static void surface_deregistered(void *data,
                     struct wl_ivi_shell *wl_ivi_shell,
                     const char *surface_id)
{

}

WaylandIviShell::WaylandIviShell()
	: QObject()
	, mIviShell(0)
	, mIviShellId(0)
	, mNotifier(0)
#ifdef PLATFORM_OVIP
	, mDisplay(wl_display_connect(0))
	, mRegistry(mDisplay ? wl_display_get_registry(mDisplay) : 0)
	, mRegistryListener()
	, mIviShellListener()
#endif
{
	LOG_INFO("Surf") << "WaylandIviShell::ctor";

#ifdef PLATFORM_OVIP
	if (mDisplay)
	{
		mNotifier = new QSocketNotifier(wl_display_get_fd(mDisplay), QSocketNotifier::Read);
		QObject::connect(mNotifier, &QSocketNotifier::activated, this, &WaylandIviShell::sltSocketNotification);
	}

	mRegistryListener.global = onRegistryAddStatic;
	mRegistryListener.global_remove = onRegistryRemoveStatic;

	if (mRegistry)
	{
		wl_registry_add_listener(mRegistry, &mRegistryListener, this);
		wl_display_roundtrip(mDisplay);
	}

    mIviShellListener.surface_registered = surface_registered;
    mIviShellListener.surface_deregistered = surface_deregistered;
#endif
}

WaylandIviShell::~WaylandIviShell()
{
	LOG_INFO("Surf") << "WaylandIviShell::dtor";

	delete mNotifier;

#ifdef PLATFORM_OVIP
	if (mRegistry)
		wl_registry_destroy(mRegistry);

	if (mDisplay)
		wl_display_disconnect(mDisplay);
#endif
}

WaylandIviShell::operator wl_ivi_shell *()
{
	return mIviShell;
}

void WaylandIviShell::dispatch()
{
#ifdef PLATFORM_OVIP
	if (mDisplay)
		wl_display_dispatch(mDisplay);
#endif
}

void WaylandIviShell::flush()
{
#ifdef PLATFORM_OVIP
	if (mDisplay)
		wl_display_flush(mDisplay);
#endif
}

#ifdef PLATFORM_OVIP
void WaylandIviShell::onRegistryAdd(void *data, wl_registry *registry, uint32_t id, const char *interface, uint32_t version)
{
	LOG_INFO("Surf")
			<< "WaylandIviShell::onRegistryAdd("
			<< data
			<< ","
			<< registry
			<< ","
			<< id
			<< ","
			<< interface
			<< ","
			<< version
			<< ")";

	if (interface && (strcmp(interface, "wl_ivi_shell") == 0))
	{
		mIviShellId = id;

		// OMG, wl_ivi_shell_interface is some wayland global
		mIviShell = static_cast<wl_ivi_shell *>(wl_registry_bind(mRegistry, id, &wl_ivi_shell_interface, version));
        wl_ivi_shell_add_listener(mIviShell, &mIviShellListener, NULL);
        LOG_INFO("Surf") << "=== mIviShell is set!!! ";
	}
}

void WaylandIviShell::onRegistryRemove(void *data, wl_registry *registry, uint32_t id)
{
	LOG_INFO("Surf")
			<< "WaylandIviShell::onRegistryRemove("
			<< data
			<< ","
			<< registry
			<< ","
			<< id
			<< ")";

	if (id == mIviShellId)
	{
		wl_ivi_shell_destroy(mIviShell);
		mIviShell = 0;
		mIviShellId = 0;
	}
}

void WaylandIviShell::onRegistryAddStatic(void *data, wl_registry *registry, uint32_t id, const char *interface, uint32_t version)
{
	if (WaylandIviShell *self = static_cast<WaylandIviShell *>(data))
		self->onRegistryAdd(data, registry, id, interface, version);
}

void WaylandIviShell::onRegistryRemoveStatic(void *data, wl_registry *registry, uint32_t id)
{
	if (WaylandIviShell *self = static_cast<WaylandIviShell *>(data))
		self->onRegistryRemove(data, registry, id);
}
#endif

void WaylandIviShell::sltSocketNotification(int socket)
{
	LOG_INFO("Surf") << "WaylandIviShell::sltSocketNotification(" << socket << ")";

	dispatch();
}
