#include "qclient.h"

int main(int argc, char *argv[])
{
    MY_APP_LOG_POS();

    QCoreApplication a(argc, argv);
    bool verbose;
    Foo foo;
    QDBusConnection connection = QDBusConnection::connectToPeer("unix:path=/tmp/testsocket","QtP2P");
    
    net::Corp::MyApp::Frobber frobber(
        MY_APP_BUS_NAME,
        MY_APP_SERVICE1_PATH,
        connection
    );

    QObject::connect(
        &frobber,
        SIGNAL(Notification(const QByteArray &, int, const QStringList &)),
        &foo,
        SLOT(sltNotification(const QByteArray &, int, const QStringList &))
    );

    verbose = frobber.verbose();
    printf("verbose=%d\n", verbose);
    verbose = !verbose;
    frobber.setVerbose(verbose);        // set property; NOTE: synchronous call.
    verbose = frobber.verbose();
    printf("verbose=%d\n", verbose);

    frobber.HelloWorld("John");

    printf("the call finishes.\n");

    return a.exec();
}
