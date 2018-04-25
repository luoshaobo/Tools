#include "Transaction.h"
#include "IviShell.h"
#include "log.h"

using namespace api_bl::surface::winston;

Transaction::Transaction(IviShell &iviShell) :
    mIviShell(iviShell),
    mGroup(0)
{
    LOG_GEN_PRINTF("\n");
//    LOG_INFO("Surf") << "Transaction::ctor";

    mGroup = wl_ivi_shell_create_animation_group(mIviShell);
}

Transaction::~Transaction()
{
    LOG_GEN_PRINTF("\n");
//    LOG_INFO("Surf") << "Transaction::dtor";

    commit(false);
}

Transaction &Transaction::setZOrder(const std::string &surface, int zOrder)
{
    LOG_GEN_PRINTF("surface=%s, zOrder=%d\n", surface.c_str(), zOrder);
//    LOG_INFO("Surf")
//            << "Transaction::setZOrder("
//            << surface.c_str()
//            << ","
//            << zOrder
//            << ")";

    if (mGroup)
    {
        LOG_GEN_PRINTF("\n");
        wl_fixed_t zero = wl_fixed_from_int(0);
        wl_fixed_t zOrderFixed = wl_fixed_from_int(zOrder);

        wl_ivi_animation_group_animate_scalar(
                    mGroup,
                    surface.c_str(),
                    WL_IVI_ANIMATION_GROUP_PROPERTY_Z_ORDER,
                    zero, zero,
                    zOrderFixed,
                    WL_IVI_ANIMATION_GROUP_EASING_IN,
                    WL_IVI_ANIMATION_GROUP_EASING_CURVE_LINEAR);
    }

    return *this;
}

Transaction &Transaction::setOpacity(const std::string &surface, int opacity)
{
    LOG_GEN_PRINTF("surface=%s, opacity=%d\n", surface.c_str(), opacity);
//    LOG_INFO("Surf")
//            << "Transaction::setOpacity("
//            << surface
//            << ","
//            << opacity
//            << ")";

    if (mGroup)
    {
        LOG_GEN_PRINTF("\n");
        wl_fixed_t zero = wl_fixed_from_int(0);
        wl_fixed_t opacityFixed = wl_fixed_from_int(opacity);

        wl_ivi_animation_group_animate_scalar(
                    mGroup,
                    surface.c_str(),
                    WL_IVI_ANIMATION_GROUP_PROPERTY_ALPHA,
                    zero, zero,
                    opacityFixed,
                    WL_IVI_ANIMATION_GROUP_EASING_IN,
                    WL_IVI_ANIMATION_GROUP_EASING_CURVE_LINEAR);
    }

    return *this;
}

Transaction &Transaction::setPosition(const std::string &surface, int x, int y)
{
    LOG_GEN_PRINTF("surface=%s, x=%d, y=%d\n", surface.c_str(), x, y);
//    LOG_INFO("Surf")
//            << "Transaction::setPosition("
//            << surface
//            << ","
//            << x
//            << ","
//            << y
//            << ")";

    if (mGroup)
    {
        LOG_GEN_PRINTF("\n");
        wl_fixed_t zero = wl_fixed_from_int(0);
        wl_fixed_t xFixed = wl_fixed_from_int(x);
        wl_fixed_t yFixed = wl_fixed_from_int(y);

        wl_ivi_animation_group_animate_cubicbezier2d(
                    mGroup,
                    surface.c_str(),
                    WL_IVI_ANIMATION_GROUP_PROPERTY_POSITION,
                    zero, zero,
                    xFixed/3, yFixed/3, xFixed/3*2, yFixed/3*2, xFixed, yFixed,
                    WL_IVI_ANIMATION_GROUP_EASING_IN,
                    WL_IVI_ANIMATION_GROUP_EASING_CURVE_LINEAR);
    }

    return *this;
}

Transaction &Transaction::setSize(const std::string &surface, int width, int height)
{
    LOG_GEN_PRINTF("surface=%s, width=%d, height=%d\n", surface.c_str(), width, height);
//    LOG_INFO("Surf")
//            << "Transaction::setSize("
//            << surface
//            << ","
//            << width
//            << ","
//            << height
//            << ")";

    if (mGroup)
    {
        LOG_GEN_PRINTF("\n");
        wl_fixed_t zero = wl_fixed_from_int(0);
        wl_fixed_t wFixed = wl_fixed_from_int(width);
        wl_fixed_t hFixed = wl_fixed_from_int(height);

        wl_ivi_animation_group_animate_cubicbezier2d(
                    mGroup,
                    surface.c_str(),
                    WL_IVI_ANIMATION_GROUP_PROPERTY_DST_SIZE,
                    zero, zero,
                    wFixed/3, hFixed/3, wFixed/3*2, hFixed/3*2, wFixed, hFixed,
                    WL_IVI_ANIMATION_GROUP_EASING_IN,
                    WL_IVI_ANIMATION_GROUP_EASING_CURVE_LINEAR);
    }

    return *this;
}

Transaction &Transaction::setPickingThreshold(const std::string &surface, int pickingThreshold)
{
    LOG_GEN_PRINTF("surface=%s, pickingThreshold=%d\n", surface.c_str(), pickingThreshold);
//    LOG_INFO("Surf")
//            << "Transaction::setPickingThreshold("
//            << surface
//            << ","
//            << pickingThreshold
//            << ")";

    if (mGroup)
    {
        LOG_GEN_PRINTF("\n");
        wl_fixed_t zero = wl_fixed_from_int(0);
        wl_fixed_t pickingThresholdFixed = static_cast<wl_fixed_t>(pickingThreshold); // why this is different from the rest?

        // defines animation
        wl_ivi_animation_group_animate_scalar(
                    mGroup,
                    surface.c_str(),
                    WL_IVI_ANIMATION_GROUP_PROPERTY_PICKING_THRESHOLD,
                    zero, zero,
                    pickingThresholdFixed,
                    WL_IVI_ANIMATION_GROUP_EASING_IN,
                    WL_IVI_ANIMATION_GROUP_EASING_CURVE_LINEAR);
    }

    return *this;
}

void Transaction::commit(bool restart)
{
    LOG_GEN_PRINTF("\n");
    if (mGroup)
    {
        LOG_GEN_PRINTF("\n");
        // start animations
        wl_ivi_animation_group_start(mGroup);

        // no need for group object anymore, animations will run till finish though
        wl_ivi_animation_group_destroy(mGroup);

        // send out requests to the compositor
        mIviShell.flush();

        mGroup = restart ? wl_ivi_shell_create_animation_group(mIviShell) : nullptr;
    }
}
