#include "IviShell.h"
#include <wayland-ivi/wayland-ivi-shell-client-protocol.h>
#include "log.h"
//#include <QDebug>

using namespace api_bl::surface::winston;

//extern bool getMonitorMode();

IviShell::IviShell()
    : QObject()
    , mIviShell(0)
    , mIviShellId(0)
    , mNotifier(0)
    , mDisplay(wl_display_connect(0))
    , mRegistry(mDisplay ? wl_display_get_registry(mDisplay) : 0)
    , mRegistryListener()
{
    LOG_GEN_PRINTF("\n");
//    LOG_INFO("Surf") << "IviShell::ctor";

    if (mDisplay)
    {
        LOG_GEN_PRINTF("\n");
        mNotifier = new QSocketNotifier(wl_display_get_fd(mDisplay), QSocketNotifier::Read);
        QObject::connect(mNotifier, &QSocketNotifier::activated, this, &IviShell::sltSocketNotification);
    }

    LOG_GEN_PRINTF("\n");
    mRegistryListener.global = onRegistryAddStatic;
    mRegistryListener.global_remove = onRegistryRemoveStatic;

    LOG_GEN_PRINTF("\n");
    if (mRegistry)
    {
        LOG_GEN_PRINTF("\n");
        wl_registry_add_listener(mRegistry, &mRegistryListener, this);
        wl_display_roundtrip(mDisplay);
    }
}

IviShell::~IviShell()
{
    LOG_GEN_PRINTF("\n");
//    LOG_INFO("Surf") << "IviShell::dtor";

    delete mNotifier;

    if (mRegistry) {
        LOG_GEN_PRINTF("\n");
        wl_registry_destroy(mRegistry);
    }

    LOG_GEN_PRINTF("\n");
    if (mDisplay) {
        LOG_GEN_PRINTF("\n");
        wl_display_disconnect(mDisplay);
    }
}

IviShell::operator wl_ivi_shell *()
{
    LOG_GEN_PRINTF("\n");
    return mIviShell;
}

void IviShell::dispatch()
{
    LOG_GEN_PRINTF("\n");
    if (mDisplay) {
        LOG_GEN_PRINTF("\n");
        wl_display_dispatch(mDisplay);
    }
}

void IviShell::flush()
{
    LOG_GEN_PRINTF("\n");
    if (mDisplay) {
        LOG_GEN_PRINTF("\n");
        wl_display_flush(mDisplay);
    }
}

void IviShell::onRegistryAdd(wl_registry *registry, uint32_t id, const char *interface, uint32_t version)
{
    LOG_GEN_PRINTF("\n");
    (void) registry;
    
    // if (getMonitorMode()) {
        // qInfo()
            // << "IviShell::onRegistryAdd("
            // << registry
            // << ","
            // << id
            // << ","
            // << interface
            // << ","
            // << version
            // << ")";
    // }

    if (interface && (strcmp(interface, "wl_ivi_shell") == 0))
    {
        LOG_GEN_PRINTF("\n");
        mIviShellId = id;

        // OMG, wl_ivi_shell_interface is some global
        mIviShell = static_cast<wl_ivi_shell *>(wl_registry_bind(mRegistry, id, &wl_ivi_shell_interface, version));
    }
}

void IviShell::onRegistryRemove(wl_registry *registry, uint32_t id)
{
    LOG_GEN_PRINTF("\n");
    (void) registry;
    
    // if (getMonitorMode()) {
        // qInfo()
            // << "IviShell::onRegistryRemove("
            // << registry
            // << ","
            // << id
            // << ")";
    // }

    if (id == mIviShellId)
    {
        LOG_GEN_PRINTF("\n");
        wl_ivi_shell_destroy(mIviShell);
        mIviShell = 0;
        mIviShellId = 0;
    }
}

void IviShell::onRegistryAddStatic(void *data, wl_registry *registry, uint32_t id, const char *interface, uint32_t version)
{
    LOG_GEN_PRINTF("\n");
    if (IviShell *self = static_cast<IviShell *>(data)) {
        LOG_GEN_PRINTF("\n");
        self->onRegistryAdd(registry, id, interface, version);
    }
}

void IviShell::onRegistryRemoveStatic(void *data, wl_registry *registry, uint32_t id)
{
    LOG_GEN_PRINTF("\n");
    if (IviShell *self = static_cast<IviShell *>(data)) {
        LOG_GEN_PRINTF("\n");
        self->onRegistryRemove(registry, id);
    }
}

void IviShell::sltSocketNotification(int /*socket*/)
{
    LOG_GEN_PRINTF("\n");
//    LOG_INFO("Surf") << "IviShell::sltSocketNotification(" << socket << ")";

    dispatch();
}
