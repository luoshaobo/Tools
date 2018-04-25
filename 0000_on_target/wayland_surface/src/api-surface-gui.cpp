#include "api-surface-gui.h"
#include "api-surface-bl.h"
#include <logger.h>
#include "log.h"

using namespace api_data::surface;

namespace api_gui
{
    namespace surface
    {
        Manager *init()
        {
            LOG_INFO("SURF|", "api_gui::surface::init");
            return api_bl::surface::init();
        }

        void uninit(Manager *manager)
        {
            LOG_INFO("SURF|", "api_gui::surface::uninit", manager);
            api_bl::surface::uninit(manager);
        }

        void checkout(Manager *manager)
        {
            LOG_INFO("SURF|", "api_gui::surface::checkout", manager);
            api_bl::surface::checkout(manager);
        }

        void commit(Manager *manager)
        {
            LOG_INFO("SURF|", "api_gui::surface::commit", manager);
            api_bl::surface::commit(manager);
        }

        void setPosition(Manager *manager, const Name &name, const Position &position)
        {
            LOG_INFO("SURF|", "api_gui::surface::setPosition", manager, name, position);
            api_bl::surface::setPosition(manager, name, position);
        }

        void setSize(Manager *manager, const Name &name, const Size &size)
        {
            LOG_INFO("SURF|", "api_gui::surface::setSize", manager, name, size);
            api_bl::surface::setSize(manager, name, size);
        }

        void setOpacity(Manager *manager, const Name &name, Ramp ramp)
        {
            LOG_INFO("SURF|", "api_gui::surface::setOpacity", manager, name, ramp);
            api_bl::surface::setOpacity(manager, name, ramp);
        }

        void setCloseness(Manager *manager, const Name &name, Ramp ramp)
        {
            LOG_INFO("SURF|", "api_gui::surface::setCloseness", manager, name, ramp);
            api_bl::surface::setCloseness(manager, name, ramp);
        }
    }
}
