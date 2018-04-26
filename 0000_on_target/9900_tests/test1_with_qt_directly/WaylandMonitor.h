#ifndef WAYLANDMONITOR_H
#define WAYLANDMONITOR_H

#include <QString>
#include "WaylandIviShell.h"

namespace uifwk
{
	namespace zones
	{
		class WaylandListener;
		class WaylandIviShell;

		class WaylandMonitor
		{
		public:
			WaylandMonitor(WaylandIviShell &iviShell, WaylandListener &listener);
			~WaylandMonitor();

		private:
			WaylandIviShell &mIviShell;
			WaylandListener &mListener;

#ifdef PLATFORM_OVIP
			wl_ivi_shell_listener mIviShellListener; // looks like this must be kept alive

			void onSurfaceRegistered(void *data, wl_ivi_shell *ivi_shell, const char* surface);
			void onSurfaceUnregistered(void *data, wl_ivi_shell *ivi_shell, const char* surface);

			static void onSurfaceRegisteredStatic(void *data, wl_ivi_shell *ivi_shell, const char* surface);
			static void onSurfaceUnregisteredStatic(void *data, wl_ivi_shell *ivi_shell, const char* surface);
#endif

			Q_DISABLE_COPY(WaylandMonitor)
		};
	}
}

#endif // WAYLANDMONITOR_H
