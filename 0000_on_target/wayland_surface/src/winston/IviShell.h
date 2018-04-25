#ifndef API_SURFACE_WINSTON_IVISHELL_H
#define API_SURFACE_WINSTON_IVISHELL_H

#include <wayland-client.h>

#include <QObject>
#include <QSocketNotifier>

struct wl_ivi_shell;

namespace api_bl
{
    namespace surface
    {
        namespace winston
        {
            class IviShell : public QObject
            {
                Q_OBJECT

            public:
                IviShell();
                ~IviShell();

                operator wl_ivi_shell *();

                void dispatch();
                void flush();

            private:
                wl_ivi_shell *mIviShell;
                unsigned int mIviShellId;
                QSocketNotifier *mNotifier;

                wl_display *mDisplay;
                wl_registry *mRegistry;
                wl_registry_listener mRegistryListener;

                void onRegistryAdd(wl_registry *registry, uint32_t id, const char *interface, uint32_t version);
                void onRegistryRemove(wl_registry *registry, uint32_t id);

                static void onRegistryAddStatic(void *data, wl_registry *registry, uint32_t id, const char *interface, uint32_t version);
                static void onRegistryRemoveStatic(void *data, wl_registry *registry, uint32_t id);

                IviShell(const IviShell &) = delete;
                IviShell &operator =(const IviShell &) = delete;

            private slots:
                void sltSocketNotification(int socket);
            };
        }
    }
}

#endif // API_SURFACE_WINSTON_IVISHELL_H
