#include <QCoreApplication>
#include <QtGlobal>
#include <QDir>
#include <QLibrary>
#include "kaola_interface.h"

#define log_printf printf
#define LOG_PRINTF(format,...) do { log_printf(format, ## __VA_ARGS__); } while(0)
#define LOG_GEN() do { log_printf("=== LOG_GEN: [%s: %u: %s]\n", __FILE__, __LINE__, __PRETTY_FUNCTION__); } while(0)
#define LOG_GEN_PRINTF(format,...) do { log_printf((std::string("=== LOG_GEN: [%s: %u: %s] ")+format).c_str(), __FILE__, __LINE__, __PRETTY_FUNCTION__, ## __VA_ARGS__); } while(0)

typedef KaolaApi::Interface *(GetKaolaInterfaceFuncType)(KaolaApi::Callback *pCallback);

class KaolaCallback : public KaolaApi::Callback
{
public:
    virtual ~KaolaCallback()
    {
        LOG_GEN();
    }

    virtual unsigned int getApiVersion()
    {
        LOG_GEN();
        return KaolaApi::currentApiVersion;
    }

    virtual void post(KaolaApi::EventId event)
    {
        LOG_GEN();
        //
        // TODO: handle all of the event from BL respectively.
        //
        LOG_PRINTF("KaolaCallback(event=%u)\n", event);
    }
    
    virtual void post2(KaolaApi::EventId event, KaolaApi::OpSerialNumber opSerialNumber)
    {
        LOG_GEN();
        //
        // TODO: handle all of the event from BL respectively.
        //
        LOG_PRINTF("KaolaCallback(event=%u, opSerialNumber=%lu)\n", event, opSerialNumber);
    }
};

#ifdef USE_STATIC_PLUGIN
KaolaApi::Interface *getKaolaInterface_aux(KaolaApi::Callback *pCallback)
{
    KaolaApi::Interface *pKaolaInterface = NULL;
    
    LOG_GEN_PRINTF("+++ static linked plugin +++\n");
    
    pKaolaInterface = getKaolaInterface(pCallback);
    if (pKaolaInterface == NULL) {
        LOG_PRINTF("*** Error: getKaolaInterface_aux(): getKaolaInterface() failed\n");
    }
    
    return pKaolaInterface;
}
#else 
KaolaApi::Interface *getKaolaInterface_aux(KaolaApi::Callback *pCallback)
{
    QLibrary kaolaDll(QDir::currentPath() + "/../kaola_accesslib_plugin/libkaola_accesslib_plugin.so");
    GetKaolaInterfaceFuncType *pGetKaolaInterface = NULL;
    KaolaApi::Interface *pKaolaInterface = NULL;
    
    LOG_GEN_PRINTF("+++ dynamic linked plugin +++\n");
    
    if (kaolaDll.load()) {
        pGetKaolaInterface = (GetKaolaInterfaceFuncType *)kaolaDll.resolve("getKaolaInterface");
        if (pGetKaolaInterface != NULL) {
            pKaolaInterface = pGetKaolaInterface(pCallback);
            if (pKaolaInterface != NULL) {
                //
            } else {
                LOG_PRINTF("*** Error: getKaolaInterface_aux(): pGetKaolaInterface() failed\n");
            }
        } else {
            LOG_PRINTF("*** Error: getKaolaInterface_aux(): kaolaDll.resolve(\"getKaolaInterface\") failed\n");
        }
    } else {
        LOG_PRINTF("*** Error: getKaolaInterface_aux(): kaolaDll.load() failed\n");
        LOG_PRINTF("getKaolaInterface_aux(): %s", kaolaDll.errorString().toUtf8().data());
    }
    
    return pKaolaInterface;
}
#endif // USE_STATIC_PLUGIN

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    KaolaCallback kaolaCallback;
    
    KaolaApi::Interface *pKaolaInterface = NULL;
    unsigned int kaolaDllApiVersion = 0;
    
    LOG_GEN();
    
    pKaolaInterface = getKaolaInterface_aux(&kaolaCallback);
    if (pKaolaInterface != NULL) {
        kaolaDllApiVersion = pKaolaInterface->getApiVersion();
        if (kaolaDllApiVersion == KaolaApi::currentApiVersion) {
            if (pKaolaInterface->initialize()) {
                //
                // NOTE: after initializated successfully, all of the APIs can be called
                //
                pKaolaInterface->getCommonInterface().requestToShowBlScreen(KaolaApi::BlScreenId_MainScreen);
                pKaolaInterface->getCommonInterface().requestToShowBlScreen2(KaolaApi::BlScreenId_PlayingScreen);
                pKaolaInterface->getPlayerInterface().play();
            } else {
                LOG_PRINTF("*** Error: main(): pKaolaInterface->initialize() failed\n");
            }
        } else {
            LOG_PRINTF("*** Error: main(): version not matched\n");
        }
    } else {
        LOG_PRINTF("*** Error: main(): pGetKaolaInterface() failed\n");
    }

    return a.exec();
}
