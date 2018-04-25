#ifndef API_SURFACE_WINSTON_TRANSACTION_H
#define API_SURFACE_WINSTON_TRANSACTION_H

#include <string>
#include <wayland-ivi/wayland-ivi-shell-client-protocol.h>

namespace api_bl
{
    namespace surface
    {
        namespace winston
        {
            class IviShell;

            class Transaction
            {
            public:
                Transaction(IviShell &iviShell);
                ~Transaction();

                Transaction &setZOrder(const std::string &surface, int zOrder);
                Transaction &setOpacity(const std::string &surface, int opacity);
                Transaction &setPosition(const std::string &surface, int x, int y);
                Transaction &setSize(const std::string &surface, int width, int height);
                Transaction &setPickingThreshold(const std::string &surface, int pickingThreshold);

                void commit(bool restart);

            private:
                IviShell &mIviShell;
                wl_ivi_animation_group *mGroup;

                Transaction(Transaction &rhs) = delete;
                Transaction &operator =(Transaction &rhs) = delete;
            };
        }
    }
}

#endif // API_SURFACE_WINSTON_TRANSACTION_H
