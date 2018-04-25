#ifndef API_SURFACE_WINSTON_LISTENER_H
#define API_SURFACE_WINSTON_LISTENER_H

namespace api_bl
{
    namespace surface
    {
        namespace winston
        {
            class Listener
            {
            public:
                Listener();
                virtual ~Listener();

                virtual void onSurfaceRegistered(const char *name) = 0;
                virtual void onSurfaceUnregistered(const char *name) = 0;
            };
        }
    }
}

#endif // API_SURFACE_WINSTON_LISTENER_H
