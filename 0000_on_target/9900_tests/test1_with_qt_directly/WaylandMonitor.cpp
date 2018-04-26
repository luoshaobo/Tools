#include "WaylandMonitor.h"
#include "WaylandListener.h"
#include <QDebug>
//#include <CLogger.h>
#include <QQuickWindow>

static QString g_logContex;

#define LOG(ctx) qDebug()
#define LOG_INFO(ctx) qDebug()
#define LOG_WARNING(ctx) qDebug()
#define LOG_ERROR(ctx) qDebug()
#define LOG_FATAL(ctx) qDebug()
#define LOG_DEBUG(ctx) qDebug()
#define LOG_VERBOSE(ctx) qDebug()

using uifwk::zones::WaylandListener;
using uifwk::zones::WaylandMonitor;
using uifwk::zones::WaylandIviShell;

WaylandListener::WaylandListener()
{
}

WaylandListener::~WaylandListener()
{
}

WaylandMonitor::WaylandMonitor(WaylandIviShell &iviShell, WaylandListener &listener)
	: mIviShell(iviShell)
	, mListener(listener)
#ifdef PLATFORM_OVIP
	, mIviShellListener()
#endif
{
	LOG_INFO("Surf")
			<< "WaylandMonitor::ctor"
#ifdef PLATFORM_OVIP
			<< "for platform OVIP"
#endif
			;

#ifdef PLATFORM_OVIP
	mIviShellListener.surface_registered = onSurfaceRegisteredStatic;
	mIviShellListener.surface_deregistered = onSurfaceUnregisteredStatic;

	int result = wl_ivi_shell_add_listener(iviShell, &mIviShellListener, this);

	LOG_INFO("Surf")
			<< "WaylandMonitor::ctor ivi_shell"
			<< iviShell
			<< "add_listener_result"
			<< result;
#endif
}

WaylandMonitor::~WaylandMonitor()
{
	LOG_INFO("Surf") << "WaylandMonitor::dtor";
}

#ifdef PLATFORM_OVIP
void WaylandMonitor::onSurfaceRegistered(void *, wl_ivi_shell *, const char* surface)
{
	mListener.onSurfaceRegistered(QString(surface));
}

void WaylandMonitor::onSurfaceUnregistered(void *, wl_ivi_shell *, const char* surface)
{
	mListener.onSurfaceUnregistered(QString(surface));
}

void WaylandMonitor::onSurfaceRegisteredStatic(void *data, wl_ivi_shell *ivi_shell, const char* surface)
{
	if (WaylandMonitor *self = static_cast<WaylandMonitor *>(data))
		self->onSurfaceRegistered(data, ivi_shell, surface);
}

void WaylandMonitor::onSurfaceUnregisteredStatic(void *data, wl_ivi_shell *ivi_shell, const char* surface)
{
	if (WaylandMonitor *self = static_cast<WaylandMonitor *>(data))
		self->onSurfaceUnregistered(data, ivi_shell, surface);
}
#endif
