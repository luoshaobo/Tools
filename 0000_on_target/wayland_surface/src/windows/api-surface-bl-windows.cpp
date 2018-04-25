#include "api-surface-bl.h"
#include <windows.h>
#include <limits>

using namespace api_data::surface;

namespace
{
    std::wstring s2ws(const std::string &s)
    {
        std::wstring ws(s.begin(), s.end());
        return ws;
    }
}

namespace api_data
{
    namespace surface
    {
        struct Manager
        {
        };
    }
}

namespace api_bl
{
    namespace surface
    {
        Manager *init()
        {
            static Manager manager;
            return &manager;
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

        void setPosition(Manager *, const Name &name, const Position &position)
        {
            if (HWND hWnd = FindWindow(NULL, s2ws(name).c_str()))
                SetWindowPos(hWnd, HWND_BOTTOM, position.first, position.second, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }

        void setSize(Manager *, const Name &name, const Size &size)
        {
            if (HWND hWnd = FindWindow(NULL, s2ws(name).c_str()))
                SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, size.first, size.second, SWP_NOMOVE || SWP_NOZORDER);
        }

        void setOpacity(Manager *, const Name &name, Ramp ramp)
        {
            if (HWND hWnd = FindWindow(NULL, s2ws(name).c_str()))
            {
                const auto mn = std::numeric_limits<Ramp>::min();
                const auto mx = std::numeric_limits<Ramp>::max();
                const auto md = mn + (mx - mn) / 2;

                ShowWindow(hWnd, ramp < md ? SW_HIDE : SW_SHOW);
            }
        }

        void setCloseness(Manager *, const Name &name, Ramp ramp)
        {
            if (HWND hWnd = FindWindow(NULL, s2ws(name).c_str()))
            {
                HWND hWndInsertAfter = HWND_NOTOPMOST;

                switch (ramp)
                {
                    case std::numeric_limits<Ramp>::max(): hWndInsertAfter = HWND_TOPMOST; break;
                    case std::numeric_limits<Ramp>::min(): hWndInsertAfter = HWND_BOTTOM; break;
                    default: break; // leave HWND_NOTOPMOST
                }

                SetWindowPos(hWnd, hWndInsertAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            }
        }
    }
}
