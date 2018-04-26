#ifndef WAYLANDCONTROLLER_H
#define WAYLANDCONTROLLER_H

#include <QString>

class Test_SurfaceManager;

namespace uifwk
{
	namespace zones
	{
		class WaylandIviShell;

		class WaylandAnimation
		{
		public:
            WaylandAnimation(WaylandAnimation &rhs);
			~WaylandAnimation();

			WaylandAnimation &setZOrder(const QString &surface, int zOrder);
			WaylandAnimation &setOpacity(const QString &surface, int opacity);
			WaylandAnimation &setPosition(const QString &surface, int x, int y);
			WaylandAnimation &setSize(const QString &surface, int width, int height);
			WaylandAnimation &setPickingThreshold(const QString &surface, int pickingThreshold);

		private:
			WaylandIviShell &mIviShell;
			void *mAnimationGroup;

			WaylandAnimation(WaylandIviShell &iviShell);

            Q_DISABLE_COPY(WaylandAnimation)

			friend class WaylandController;
            friend class ::Test_SurfaceManager;
		};

		class WaylandController
		{
		public:
			explicit WaylandController(WaylandIviShell &iviShell);
			~WaylandController();

            WaylandAnimation *createAnimation();

		private:
			WaylandIviShell &mIviShell;
		};
	}
}

#endif // WAYLANDCONTROLLER_H
