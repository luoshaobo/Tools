#ifndef API_SURFACE_WINSTON_PROPERTIES_H
#define API_SURFACE_WINSTON_PROPERTIES_H

#include <api-surface-data.h>
#include "Transaction.h"
#include <string>

namespace api_bl
{
    namespace surface
    {
        namespace winston
        {
            class Properties
            {
            public:
                Properties();

                void setDirty(bool dirty) { dirty_ = dirty; }
                void setPosition(const api_data::surface::Position &position) { dirty_ = true; newPosition_ = position; }
                void setSize(const api_data::surface::Size &size) { dirty_ = true; newSize_ = size; }
                void setO(api_data::surface::Ramp ramp) { dirty_ = true; newO_ = ramp; }
                void setZ(api_data::surface::Ramp ramp) { dirty_ = true; newZ_ = ramp; }

                void commit(const std::string &surface, Transaction &transaction);
                void invalidate();

            private:
                bool dirty_;
                api_data::surface::Position position_, newPosition_;
                api_data::surface::Size size_, newSize_;
                api_data::surface::Ramp z_, newZ_;
                api_data::surface::Ramp o_, newO_;
            };
        }
    }
}

#endif // API_SURFACE_WINSTON_PROPERTIES_H
