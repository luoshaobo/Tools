#include "WaylandController.h"
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

using uifwk::zones::WaylandAnimation;
using uifwk::zones::WaylandController;
using uifwk::zones::WaylandIviShell;

WaylandAnimation::WaylandAnimation(WaylandIviShell &iviShell) :
	mIviShell(iviShell),
	mAnimationGroup(0)
{
	LOG_INFO("Surf") << "WaylandAnimation::ctor";

#ifdef PLATFORM_OVIP
	mAnimationGroup = wl_ivi_shell_create_animation_group(mIviShell);
#endif
}

WaylandAnimation::WaylandAnimation(WaylandAnimation &rhs) :
	mIviShell(rhs.mIviShell),
	mAnimationGroup(rhs.mAnimationGroup)
{
	rhs.mAnimationGroup = 0;

	LOG_INFO("Surf") << "WaylandAnimation::move ctor";
}

WaylandAnimation::~WaylandAnimation()
{
	LOG_INFO("Surf") << "WaylandAnimation::dtor";

#ifdef PLATFORM_OVIP
	if (wl_ivi_animation_group *animationGroup = static_cast<wl_ivi_animation_group *>(mAnimationGroup))
	{
		// start animations
		wl_ivi_animation_group_start(animationGroup);

		// no need for group object anymore, animations will run till finish though
		wl_ivi_animation_group_destroy(animationGroup);

		// send out requests to the compositor
		mIviShell.flush();
	}
#endif
}

WaylandAnimation &WaylandAnimation::setZOrder(const QString &surface, int zOrder)
{
	LOG_INFO("Surf")
			<< "WaylandAnimation::setZOrder("
			<< surface.toLatin1()
			<< ","
			<< zOrder
			<< ")";

#ifdef PLATFORM_OVIP
	if (wl_ivi_animation_group *animationGroup = static_cast<wl_ivi_animation_group *>(mAnimationGroup))
	{
		wl_fixed_t zero = wl_fixed_from_int(0);
		wl_fixed_t zOrderFixed = wl_fixed_from_int(zOrder);

		wl_ivi_animation_group_animate_scalar(
					animationGroup,
					surface.toLatin1(),
					WL_IVI_ANIMATION_GROUP_PROPERTY_Z_ORDER,
					zero, zero,
					zOrderFixed,
					WL_IVI_ANIMATION_GROUP_EASING_IN,
					WL_IVI_ANIMATION_GROUP_EASING_CURVE_LINEAR);
	}
#endif

	return *this;
}

WaylandAnimation &WaylandAnimation::setOpacity(const QString &surface, int opacity)
{
	LOG_INFO("Surf")
			<< "WaylandAnimation::setOpacity("
			<< surface
			<< ","
			<< opacity
			<< ")";

#ifdef PLATFORM_OVIP
	if (wl_ivi_animation_group *animationGroup = static_cast<wl_ivi_animation_group *>(mAnimationGroup))
	{
		wl_fixed_t zero = wl_fixed_from_int(0);
		wl_fixed_t opacityFixed = wl_fixed_from_int(opacity);
		if(opacity == 1)
		{
			wl_ivi_animation_group_animate_scalar(
						animationGroup,
						surface.toLatin1(),
						WL_IVI_ANIMATION_GROUP_PROPERTY_ALPHA,
						zero, zero,
						zero,
						WL_IVI_ANIMATION_GROUP_EASING_IN,
						WL_IVI_ANIMATION_GROUP_EASING_CURVE_LINEAR);

			wl_ivi_animation_group_animate_scalar(
						animationGroup,
						surface.toLatin1(),
						WL_IVI_ANIMATION_GROUP_PROPERTY_ALPHA,
						zero, 800,
						opacityFixed,
						WL_IVI_ANIMATION_GROUP_EASING_OUT,
						WL_IVI_ANIMATION_GROUP_EASING_CURVE_QUAD);
		}
		else
		{
			wl_ivi_animation_group_animate_scalar(
						animationGroup,
						surface.toLatin1(),
						WL_IVI_ANIMATION_GROUP_PROPERTY_ALPHA,
						zero, zero,
						opacityFixed,
						WL_IVI_ANIMATION_GROUP_EASING_IN,
						WL_IVI_ANIMATION_GROUP_EASING_CURVE_LINEAR);
		}
	}
#endif

	return *this;
}

WaylandAnimation &WaylandAnimation::setPosition(const QString &surface, int x, int y)
{
	LOG_INFO("Surf")
			<< "WaylandAnimation::setPosition("
			<< surface
			<< ","
			<< x
			<< ","
			<< y
			<< ")";

#ifdef PLATFORM_OVIP
	if (wl_ivi_animation_group *animationGroup = static_cast<wl_ivi_animation_group *>(mAnimationGroup))
	{
		wl_fixed_t zero = wl_fixed_from_int(0);
		wl_fixed_t xFixed = wl_fixed_from_int(x);
		wl_fixed_t yFixed = wl_fixed_from_int(y);

		wl_ivi_animation_group_animate_cubicbezier2d(
					animationGroup,
					surface.toLatin1(),
					WL_IVI_ANIMATION_GROUP_PROPERTY_POSITION,
					zero, zero,
					xFixed/3, yFixed/3, xFixed/3*2, yFixed/3*2, xFixed, yFixed,
					WL_IVI_ANIMATION_GROUP_EASING_IN,
					WL_IVI_ANIMATION_GROUP_EASING_CURVE_LINEAR);
	}
#endif

	return *this;
}

WaylandAnimation &WaylandAnimation::setSize(const QString &surface, int width, int height)
{
	LOG_INFO("Surf")
			<< "WaylandAnimation::setSize("
			<< surface
			<< ","
			<< width
			<< ","
			<< height
			<< ")";

#ifdef PLATFORM_OVIP
	if (wl_ivi_animation_group *animationGroup = static_cast<wl_ivi_animation_group *>(mAnimationGroup))
	{
		wl_fixed_t zero = wl_fixed_from_int(0);
		wl_fixed_t wFixed = wl_fixed_from_int(width);
		wl_fixed_t hFixed = wl_fixed_from_int(height);

		wl_ivi_animation_group_animate_cubicbezier2d(
					animationGroup,
					surface.toLatin1(),
					WL_IVI_ANIMATION_GROUP_PROPERTY_DST_SIZE,
					zero, zero,
					wFixed/3, hFixed/3, wFixed/3*2, hFixed/3*2, wFixed, hFixed,
					WL_IVI_ANIMATION_GROUP_EASING_IN,
					WL_IVI_ANIMATION_GROUP_EASING_CURVE_LINEAR);
	}
#endif

	return *this;
}

WaylandAnimation &WaylandAnimation::setPickingThreshold(const QString &surface, int pickingThreshold)
{
	LOG_INFO("Surf")
			<< "WaylandAnimation::setPickingThreshold("
			<< surface
			<< ","
			<< pickingThreshold
			<< ")";

#ifdef PLATFORM_OVIP
	if (wl_ivi_animation_group *animationGroup = static_cast<wl_ivi_animation_group *>(mAnimationGroup))
	{
		wl_fixed_t zero = wl_fixed_from_int(0);
		wl_fixed_t pickingThresholdFixed = static_cast<wl_fixed_t>(pickingThreshold); // why this is different from the rest?

		// defines animation
		wl_ivi_animation_group_animate_scalar(
					animationGroup,
					surface.toLatin1(),
					WL_IVI_ANIMATION_GROUP_PROPERTY_PICKING_THRESHOLD,
					zero, zero,
					pickingThresholdFixed,
					WL_IVI_ANIMATION_GROUP_EASING_IN,
					WL_IVI_ANIMATION_GROUP_EASING_CURVE_LINEAR);
	}
#endif

	return *this;
}

WaylandController::WaylandController(WaylandIviShell &iviShell) :
	mIviShell(iviShell)
{
	LOG_INFO("Surf") << "WaylandController::ctor";
}

WaylandController::~WaylandController()
{
	LOG_INFO("Surf") << "WaylandController::dtor";
}

WaylandAnimation *WaylandController::createAnimation()
{
    return new WaylandAnimation(mIviShell);
}
