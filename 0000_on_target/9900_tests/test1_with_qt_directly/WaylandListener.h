#ifndef WAYLANDLISTENER_H
#define WAYLANDLISTENER_H

#include <QString>

namespace uifwk
{
	namespace zones
	{
		class WaylandListener
		{
		public:
			WaylandListener();
			virtual ~WaylandListener();

			virtual void onSurfaceRegistered(const QString &name) = 0;
			virtual void onSurfaceUnregistered(const QString &name) = 0;
		};
	}
}

#endif // WAYLANDLISTENER_H
