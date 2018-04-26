#ifndef WAYLANDIVISHELL_H
#define WAYLANDIVISHELL_H

#ifdef PLATFORM_OVIP
	#include <wayland-client.h>
	#include <wayland-ivi/wayland-ivi-shell-client-protocol.h>
#endif

#include <QObject>
#include <QSocketNotifier>

struct wl_ivi_shell;

namespace uifwk
{
	namespace zones
	{
		class WaylandIviShell : public QObject
		{
			Q_OBJECT

		public:
			WaylandIviShell();
			~WaylandIviShell();

			operator wl_ivi_shell *();

			void dispatch();
			void flush();

		private:
			wl_ivi_shell *mIviShell;
			unsigned int mIviShellId;
			QSocketNotifier *mNotifier;

#ifdef PLATFORM_OVIP
			wl_display *mDisplay;
			wl_registry *mRegistry;
			wl_registry_listener mRegistryListener;
			wl_ivi_shell_listener mIviShellListener; // looks like this must be kept alive

			void onRegistryAdd(void *data, wl_registry *registry, uint32_t id, const char *interface, uint32_t version);
			void onRegistryRemove(void *data, wl_registry *registry, uint32_t id);

			static void onRegistryAddStatic(void *data, wl_registry *registry, uint32_t id, const char *interface, uint32_t version);
			static void onRegistryRemoveStatic(void *data, wl_registry *registry, uint32_t id);
#endif

			WaylandIviShell(const WaylandIviShell &);
			WaylandIviShell &operator=(const WaylandIviShell &);

		private slots:
			void sltSocketNotification(int socket);
		};
	}
}

#endif // WAYLANDIVISHELL_H
