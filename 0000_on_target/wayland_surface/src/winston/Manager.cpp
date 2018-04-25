#include "Manager.h"
#include "log.h"

using namespace api_data::surface;
using namespace api_bl::surface::winston;
using std::string;

Manager::Manager() :
    iviShell_(),
    transaction_(iviShell_),
    bridge_(iviShell_, *this),
    registered_(),
    properties_()
{
    LOG_GEN_PRINTF("\n");
}

Manager::~Manager()
{
    LOG_GEN_PRINTF("\n");
}

void Manager::setPosition(const Name &name, const Position &position)
{
    LOG_GEN_PRINTF("\n");
    properties_[name].setPosition(position);
}

void Manager::setSize(const Name &name, const Size &size)
{
    LOG_GEN_PRINTF("\n");
    properties_[name].setSize(size);
}

void Manager::setOpacity(const Name &name, Ramp ramp)
{
    LOG_GEN_PRINTF("\n");
    properties_[name].setO(ramp);
}

void Manager::setCloseness(const Name &name, Ramp ramp)
{
    LOG_GEN_PRINTF("\n");
    properties_[name].setZ(ramp);
}

void Manager::commit()
{
    LOG_GEN_PRINTF("\n");
    for (auto it = properties_.begin(); it != properties_.end(); ++it)
    {
        LOG_GEN_PRINTF("\n");
        const auto& name = it->first;

        if (registered_.count(name) > 0)
        {
            LOG_GEN_PRINTF("\n");
            auto &props = it->second;
            props.commit(name, transaction_);
        }
    }

    LOG_GEN_PRINTF("\n");
    transaction_.commit(true);
}

extern void list_surface_callback(const char *surface);
void Manager::onSurfaceRegistered(const char *name)
{
    LOG_GEN_PRINTF("name=%s\n", name);

    if (name)
    {
        list_surface_callback(name);
        LOG_GEN_PRINTF("\n");
        registered_.insert(name);

        auto it = properties_.find(name);
        if (it != properties_.end())
        {
            LOG_GEN_PRINTF("\n");
            auto &props = it->second;
            props.commit(name, transaction_);
        }
        else
        {
            LOG_GEN_PRINTF("\n");
            //transaction_.setZOrder(name, std::numeric_limits<Ramp>::min());
            //transaction_.setOpacity(name, std::numeric_limits<Ramp>::min());
        }

        LOG_GEN_PRINTF("\n");
        transaction_.commit(true);
    }
}

void Manager::onSurfaceUnregistered(const char *name)
{
    LOG_GEN_PRINTF("\n");
    if (name)
    {
        LOG_GEN_PRINTF("\n");
        registered_.erase(name);

        auto it = properties_.find(name);
        if (it != properties_.end()) {
            LOG_GEN_PRINTF("\n");
            //it->second.invalidate();
            it->second.setDirty(true);
        }
    }
}
