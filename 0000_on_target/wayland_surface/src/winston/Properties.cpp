#include "Properties.h"
#include <limits>
#include "log.h"

using api_bl::surface::winston::Properties;
using api_bl::surface::winston::Transaction;

namespace
{
    const api_data::surface::Position DEFAULT_POSITION = { 0, 0 };
    const api_data::surface::Size DEFAULT_SIZE = { -1, -1 };
    const api_data::surface::Ramp DEFAULT_Z = 0;
    const api_data::surface::Ramp DEFAULT_OPACITY = std::numeric_limits<api_data::surface::Ramp>::max();
}

Properties::Properties() :
    dirty_(false),
    position_(DEFAULT_POSITION),
    newPosition_(position_),
    size_(DEFAULT_SIZE),
    newSize_(size_),
    z_(DEFAULT_Z),
    newZ_(z_),
    o_(DEFAULT_OPACITY),
    newO_(o_)
{
    LOG_GEN_PRINTF("\n");
}

void Properties::commit(const std::string &surface, Transaction &transaction)
{
    LOG_GEN_PRINTF("\n");

    if (!dirty_) {
        return;
    }

    if (position_ != newPosition_)
    {
        LOG_GEN_PRINTF("\n");
        position_ = newPosition_;
        transaction.setPosition(surface, position_.first, position_.second);
    }

    LOG_GEN_PRINTF("\n");
    if (size_ != newSize_)
    {
        LOG_GEN_PRINTF("\n");
        size_ = newSize_;
        transaction.setSize(surface, size_.first, size_.second);
    }

    LOG_GEN_PRINTF("\n");
    if (newZ_ != z_) {
        LOG_GEN_PRINTF("\n");
        transaction.setZOrder(surface, z_ = newZ_);
    }

    LOG_GEN_PRINTF("\n");
    if (newO_ != o_) {
        LOG_GEN_PRINTF("\n");
        transaction.setOpacity(surface, o_ = newO_);
    }
}

void Properties::invalidate()
{
    LOG_GEN_PRINTF("\n");
    dirty_ = false;
    position_ = DEFAULT_POSITION;
    size_ = DEFAULT_SIZE;
    z_ = DEFAULT_Z;
    o_ = DEFAULT_OPACITY;
}
