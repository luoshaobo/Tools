#ifndef MAIN_H
#define MAIN_H

#include <QCoreApplication>
#include "NetCorpMyAppFrobberProxy.h"

#define MY_APP_BUS_NAME                         "net.corp.MyApp"
#define MY_APP_SERVICE1_PATH                    "/net/corp/MyApp"

#define MY_APP_LOG_POS()                        printf("%s()[%d]\n", __FUNCTION__, __LINE__)

class Foo : public QObject
{
    Q_OBJECT

public:
    Foo() : QObject(NULL) {}
    virtual ~Foo() {}

public slots:
    void sltNotification(const QByteArray &icon_blob, int height, const QStringList &messages)
    {
        int i;

        MY_APP_LOG_POS();

        printf("%s(): icon_blob=%s, height=%d, arg_messages=[", __FUNCTION__, (const char *)icon_blob.data(), height);
        for (i = 0; i < messages.size(); i++) {
            if (i != 0) {
                printf(", ");
            }
            printf("\"%s\"", (const char *)messages[i].toLatin1().data());
        }
        printf("]\n");
    }
};


#endif // #ifndef MAIN_H
