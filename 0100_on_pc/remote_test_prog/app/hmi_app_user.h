#ifndef HMI_APP_USER_H__76478239423905327572349239423728423842347237
#define HMI_APP_USER_H__76478239423905327572349239423728423842347237

#include <QObject>
#include <QThread>
#include <QCoreApplication>
#include "gwm_gui_setup.h"
#include "TK_Tools.h"

using namespace gwmGui;

namespace gwmGui {
    class StubScreen : public gwmGui::ScreenBase
    {
    public:
        StubScreen() : ScreenBase(0, "StubScreen") {}
        virtual ~StubScreen() {}
    };
    
    class ScreenL : public gwmGui::ScreenBase
    {
    public:
        ScreenL(int nLevel) : ScreenBase(nLevel, TK_Tools::FormatStr("ScreenL%04d", nLevel).c_str()) {}
        virtual ~ScreenL() {}
    };
    
    ScreenTable &getScreenTable();
    ScreenBase *getScreen(unsigned int nScreenId);
    int getScreenLevel(int nScreenId);
    
    QQmlEngine &getQmlEngine();
    QQmlContext *geRootQmlContext();
    QQuickWindow *getMainQmlWindow();    
    QQuickItem *getRootQuickItem();
}

class MainThreadInterceptor : public QObject
{
    Q_OBJECT
    
public:
    MainThreadInterceptor() {}
    virtual ~MainThreadInterceptor() {}
    
public:
    static MainThreadInterceptor *getInstance();
    static void sendCmdLineToMainThread(const QString &sCmdLine);
    static StubScreen *getStubScreen();
    
signals:
    void sigProcessCmdLine(const QString &sCmdLine);
    
private slots:
    void sltProcessCmdLine(const QString &sCmdLine);
    
private:
    static MainThreadInterceptor *m_pInstance;
    gwmGui::StubScreen m_stubScreen;
};

#endif // #ifndef HMI_APP_USER_H__76478239423905327572349239423728423842347237
