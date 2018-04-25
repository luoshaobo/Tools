#include "Bridge.h"
#include "IviShell.h"
#include "Listener.h"
#include "log.h"
#include <QDebug>

using namespace api_bl::surface::winston;

extern bool getMonitorMode();

Bridge::Bridge(IviShell &iviShell, Listener &listener)
    : mListener(listener)
    , mIviShellListener()
{
    LOG_GEN_PRINTF("\n");
//    LOG_INFO("Surf")
//            << "Bridge::ctor"
//            << "for platform OVIP"
//            ;

    mIviShellListener.surface_registered = onSurfaceRegisteredStatic;
    mIviShellListener.surface_deregistered = onSurfaceUnregisteredStatic;

//    int result =
    wl_ivi_shell_add_listener(iviShell, &mIviShellListener, this);

//    LOG_INFO("Surf")
//            << "Bridge::ctor ivi_shell"
//            << iviShell
//            << "add_listener_result"
//            << result;
}

Bridge::~Bridge()
{
    LOG_GEN_PRINTF("\n");
//    LOG_INFO("Surf") << "Bridge::dtor";
}

void Bridge::onSurfaceRegistered(wl_ivi_shell *, const char* surface)
{
    LOG_GEN_PRINTF("\n");
    if (getMonitorMode()) {
        qInfo()
            << "Bridge::onSurfaceRegistered("
            << surface
            << ")";
    }
    mListener.onSurfaceRegistered(surface);
}

void Bridge::onSurfaceUnregistered(wl_ivi_shell *, const char* surface)
{
    LOG_GEN_PRINTF("\n");
    if (getMonitorMode()) {
        qInfo()
            << "Bridge::onSurfaceUnregistered("
            << surface
            << ")";
    }
    mListener.onSurfaceUnregistered(surface);
}

void Bridge::onSurfaceRegisteredStatic(void *data, wl_ivi_shell *ivi_shell, const char* surface)
{
    LOG_GEN_PRINTF("\n");
    if (Bridge *self = static_cast<Bridge *>(data)) {
        LOG_GEN_PRINTF("\n");
        self->onSurfaceRegistered(ivi_shell, surface);
    }
}

void Bridge::onSurfaceUnregisteredStatic(void *data, wl_ivi_shell *ivi_shell, const char* surface)
{
    LOG_GEN_PRINTF("\n");
    if (Bridge *self = static_cast<Bridge *>(data)) {
        LOG_GEN_PRINTF("\n");
        self->onSurfaceUnregistered(ivi_shell, surface);
    }
}
