#include "api-surface-bl.h"

using namespace api_data::surface;

namespace api_bl
{
    namespace surface
    {
        Manager *init()
        {
            return nullptr;
        }

        void uninit(Manager *)
        {
        }

        void checkout(Manager *)
        {
        }

        void commit(Manager *)
        {
        }

        void setPosition(Manager *, const Name &, const Position &)
        {
        }

        void setSize(Manager *, const Name &, const Size &)
        {
        }

        void setOpacity(Manager *, const Name &, Ramp)
        {
        }

        void setCloseness(Manager *, const Name &, Ramp)
        {
        }
    }
}
