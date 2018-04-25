#ifndef APISURFACEGUI_H_INCLUDED
#define APISURFACEGUI_H_INCLUDED

#include "api-surface-data.h"

namespace api_gui
{
    namespace surface
    {
        using namespace api_data::surface;

        Manager *init();
        void uninit(Manager *);

        void checkout(Manager *);
        void commit(Manager *);

        void setPosition(Manager *, const Name &, const Position &);
        void setSize(Manager *, const Name &, const Size &);

        void setOpacity(Manager *, const Name &, Ramp);
        void setCloseness(Manager *, const Name &, Ramp);
    }
}

#endif // APISURFACEGUI_H_INCLUDED
