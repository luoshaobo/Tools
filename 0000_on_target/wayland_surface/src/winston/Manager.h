#ifndef API_SURFACE_WINSTON_SENTINEL_H
#define API_SURFACE_WINSTON_SENTINEL_H

#include "api-surface-data.h"
#include "IviShell.h"
#include "Transaction.h"
#include "Bridge.h"
#include "Listener.h"
#include "Properties.h"
#include <map>
#include <set>

namespace api_data
{
    namespace surface
    {
        struct Manager : public api_bl::surface::winston::Listener
        {
            Manager();
            ~Manager();

            void setPosition(const Name &name, const Position &position);
            void setSize(const Name &name, const Size &size);

            void setOpacity(const Name &name, Ramp ramp);
            void setCloseness(const Name &name, Ramp ramp);

            void commit();

        private:
            api_bl::surface::winston::IviShell iviShell_;
            api_bl::surface::winston::Transaction transaction_;
            api_bl::surface::winston::Bridge bridge_;

            std::set<std::string> registered_;
            std::map<std::string, api_bl::surface::winston::Properties> properties_;

            virtual void onSurfaceRegistered(const char *name);
            virtual void onSurfaceUnregistered(const char *name);
        };
    }
}

#endif // API_SURFACE_WINSTON_SENTINEL_H
