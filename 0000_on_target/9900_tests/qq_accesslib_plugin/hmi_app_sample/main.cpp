#include <QCoreApplication>
#include <QtGlobal>
#include <QDir>
#include <QLibrary>
#include "qq_interface.h"

#define log_printf printf
#define LOG_PRINTF(format,...) do { log_printf(format, ## __VA_ARGS__); } while(0)
#define LOG_GEN() do { log_printf("=== LOG_GEN: [%s: %u: %s]\n", __FILE__, __LINE__, __PRETTY_FUNCTION__); } while(0)
#define LOG_GEN_PRINTF(format,...) do { log_printf((std::string("=== LOG_GEN: [%s: %u: %s] ")+format).c_str(), __FILE__, __LINE__, __PRETTY_FUNCTION__, ## __VA_ARGS__); } while(0)

typedef QqApi::Interface *(GetQqInterfaceFuncType)(QqApi::Callback *pCallback);

class QqCallback : public QqApi::Callback
{
public:
    virtual ~QqCallback()
    {
        LOG_GEN();
    }

    virtual unsigned int getApiVersion()
    {
        LOG_GEN();
        return QqApi::currentApiVersion;
    }

    virtual void post(QqApi::EventId event)
    {
        LOG_GEN();
        //
        // TODO: handle all of the event from BL respectively.
        //
        LOG_PRINTF("QqCallback(event=%u)\n", event);
    }
    
    virtual void post2(QqApi::EventId event, QqApi::OpSerialNumber opSerialNumber)
    {
        LOG_GEN();
        //
        // TODO: handle all of the event from BL respectively.
        //
        LOG_PRINTF("QqCallback(event=%u, opSerialNumber=%lu)\n", event, opSerialNumber);
    }
};

#ifdef USE_STATIC_PLUGIN
QqApi::Interface *getQqInterface_aux(QqApi::Callback *pCallback)
{
    QqApi::Interface *pQqInterface = NULL;
    
    LOG_GEN_PRINTF("+++ static linked plugin +++\n");
    
    pQqInterface = getQqInterface(pCallback);
    if (pQqInterface == NULL) {
        LOG_PRINTF("*** Error: getQqInterface_aux(): getQqInterface() failed\n");
    }
    
    return pQqInterface;
}
#else 
QqApi::Interface *getQqInterface_aux(QqApi::Callback *pCallback)
{
    QLibrary qqDll(QDir::currentPath() + "/../qq_accesslib_plugin/libqq_accesslib_plugin.so");
    GetQqInterfaceFuncType *pGetQqInterface = NULL;
    QqApi::Interface *pQqInterface = NULL;
    
    LOG_GEN_PRINTF("+++ dynamic linked plugin +++\n");
    
    if (qqDll.load()) {
        pGetQqInterface = (GetQqInterfaceFuncType *)qqDll.resolve("getQqInterface");
        if (pGetQqInterface != NULL) {
            pQqInterface = pGetQqInterface(pCallback);
            if (pQqInterface != NULL) {
                //
            } else {
                LOG_PRINTF("*** Error: getQqInterface_aux(): pGetQqInterface() failed\n");
            }
        } else {
            LOG_PRINTF("*** Error: getQqInterface_aux(): qqDll.resolve(\"getQqInterface\") failed\n");
        }
    } else {
        LOG_PRINTF("*** Error: getQqInterface_aux(): qqDll.load() failed\n");
        LOG_PRINTF("getQqInterface_aux(): %s", qqDll.errorString().toUtf8().data());
    }
    
    return pQqInterface;
}
#endif // USE_STATIC_PLUGIN

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QqCallback qqCallback;
    
    QqApi::Interface *pQqInterface = NULL;
    unsigned int qqDllApiVersion = 0;
    
    LOG_GEN();
    
    pQqInterface = getQqInterface_aux(&qqCallback);
    if (pQqInterface != NULL) {
        qqDllApiVersion = pQqInterface->getApiVersion();
        if (qqDllApiVersion == QqApi::currentApiVersion) {
            if (pQqInterface->initialize()) {
                //
                // NOTE: after initializated successfully, all of the APIs can be called
                //
                pQqInterface->getCommonInterface().requestToShowBlScreen(QqApi::BlScreenId_MainScreen);
                pQqInterface->getCommonInterface().requestToShowBlScreen2(QqApi::BlScreenId_PlayingScreen);
                pQqInterface->getPlayerInterface().play();
            } else {
                LOG_PRINTF("*** Error: main(): pQqInterface->initialize() failed\n");
            }
        } else {
            LOG_PRINTF("*** Error: main(): version not matched\n");
        }
    } else {
        LOG_PRINTF("*** Error: main(): pGetQqInterface() failed\n");
    }

    return a.exec();
}
