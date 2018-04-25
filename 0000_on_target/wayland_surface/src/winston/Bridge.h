#ifndef API_SURFACE_WINSTON_BRIDGE_H
#define API_SURFACE_WINSTON_BRIDGE_H

#include <wayland-ivi/wayland-ivi-shell-client-protocol.h>

namespace api_bl
{
    namespace surface
    {
        namespace winston
        {
            class IviShell;
            class Listener;

            class Bridge
            {
            public:
                Bridge(IviShell &iviShell, Listener &listener);
                ~Bridge();

            private:
                Listener &mListener;

                wl_ivi_shell_listener mIviShellListener;

                void onSurfaceRegistered(wl_ivi_shell *ivi_shell, const char* surface);
                void onSurfaceUnregistered(wl_ivi_shell *ivi_shell, const char* surface);

                static void onSurfaceRegisteredStatic(void *data, wl_ivi_shell *ivi_shell, const char* surface);
                static void onSurfaceUnregisteredStatic(void *data, wl_ivi_shell *ivi_shell, const char* surface);

                Bridge(const Bridge &) = delete;
                Bridge &operator =(const Bridge &) = delete;
            };
        }
    }
}

#endif // API_SURFACE_WINSTON_BRIDGE_H
