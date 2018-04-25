#include "api-surface-bl.h"
#include "Manager.h"
#include "log.h"

using namespace api_data::surface;
using namespace api_bl::surface::winston;

namespace
{
    Manager *manager_ = nullptr;
    int users_ = 0;
}

namespace api_bl
{
    namespace surface
    {
        Manager *init()
        {
            LOG_GEN_PRINTF("\n");
            if (++users_ == 1) {
                LOG_GEN_PRINTF("\n");
                manager_ = new Manager;
            }

            return manager_;
        }

        void uninit(Manager *manager)
        {
            LOG_GEN_PRINTF("\n");
            if (manager == manager_)
            {
                LOG_GEN_PRINTF("\n");
                if (--users_ == 0) {
                    LOG_GEN_PRINTF("\n");
                    delete manager_;
                }
            }
        }

        void checkout(Manager *)
        {
            LOG_GEN_PRINTF("\n");
        }

        void commit(Manager *manager)
        {
            LOG_GEN_PRINTF("\n");
            if (manager) {
                LOG_GEN_PRINTF("\n");
                manager->commit();
            }
        }

        void setPosition(Manager *manager, const Name &name, const Position &position)
        {
            LOG_GEN_PRINTF("\n");
            if (manager) {
                LOG_GEN_PRINTF("\n");
                manager->setPosition(name, position);
            }
        }

        void setSize(Manager *manager, const Name &name, const Size &size)
        {
            LOG_GEN_PRINTF("\n");
            if (manager) {
                LOG_GEN_PRINTF("\n");
                manager->setSize(name, size);
            }
        }

        void setOpacity(Manager *manager, const Name &name, Ramp ramp)
        {
            LOG_GEN_PRINTF("\n");
            if (manager) {
                manager->setOpacity(name, ramp);
            }
        }

        void setCloseness(Manager *manager, const Name &name, Ramp ramp)
        {
            LOG_GEN_PRINTF("\n");
            if (manager) {
                LOG_GEN_PRINTF("\n");
                manager->setCloseness(name, ramp);
            }
        }
    }
}
