#ifndef APISURFACEDATA_H_INCLUDED
#define APISURFACEDATA_H_INCLUDED

#include <string>
#include <utility>

namespace api_data
{
    namespace surface
    {
        typedef std::string Name;
        typedef std::pair<short, short> Position;
        typedef std::pair<unsigned short, unsigned short> Size;

        typedef unsigned char Ramp;

        struct Manager;
    }
}

#endif // APISURFACEDATA_H_INCLUDED
