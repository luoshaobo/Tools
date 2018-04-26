#include "testqmlviewplugin_plugin.h"
#include "myitem.h"

#include <qqml.h>

void TestQmlViewPluginPlugin::registerTypes(const char *uri)
{
    // @uri com.mycompany.qmlcomponents
    qmlRegisterType<MyItem>(uri, 1, 0, "MyItem");
}

namespace {
    GlobalConfig *g_pGlobalConfig = NULL;
}

extern "C" {
bool onLoadPlugin(QQmlContext *pContext) {
    printf("in dll: onLoadPlugin()!!!\n");
    
    g_pGlobalConfig = new GlobalConfig();
    pContext->setContextProperty("globalConfig", g_pGlobalConfig);
    
    return true;
}
} // extern "C" {
