#include <QCoreApplication>
#include <QDBusServer>
#include "Frobber.h"
#include "NetCorpMyAppFrobberAdaptor.h"

#define MY_APP_BUS_NAME                         "net.corp.MyApp"
#define MY_APP_SERVICE1_PATH                    "/net/corp/MyApp"

#define MY_APP_LOG_POS()                        printf("%s()[%d]\n", __FUNCTION__, __LINE__)

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QDBusServer p2pDbusServer(QLatin1String("unix:path=/tmp/testsocket"));
    
    Frobber frobber;
    FrobberAdaptor frobberAdaptor(&frobber);
    
    QObject::connect(&p2pDbusServer, &QDBusServer::newConnection, [&frobber](const QDBusConnection &connection) {
        ((QDBusConnection *)&connection)->registerObject(MY_APP_SERVICE1_PATH, &frobber);
        ((QDBusConnection *)&connection)->registerService(MY_APP_BUS_NAME);
    });

    return a.exec();
}
