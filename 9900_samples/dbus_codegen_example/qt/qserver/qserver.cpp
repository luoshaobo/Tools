#include <QCoreApplication>
#include "Frobber.h"
#include "NetCorpMyAppFrobberAdaptor.h"

#define MY_APP_BUS_NAME                         "net.corp.MyApp"
#define MY_APP_SERVICE1_PATH                    "/net/corp/MyApp"

#define MY_APP_LOG_POS()                        printf("%s()[%d]\n", __FUNCTION__, __LINE__)

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Frobber frobber;
    FrobberAdaptor frobberAdaptor(&frobber);

    QDBusConnection connection =
#ifdef USE_SESSION_BUS
        QDBusConnection::sessionBus();
#else
        QDBusConnection::systemBus();
#endif

    connection.registerObject(MY_APP_SERVICE1_PATH, &frobber);
    connection.registerService(MY_APP_BUS_NAME);

    return a.exec();
}
