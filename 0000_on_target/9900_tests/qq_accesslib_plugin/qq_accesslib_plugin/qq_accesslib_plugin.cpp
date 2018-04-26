#include <stdio.h>
#include <stdlib.h>
#include "qq_interface.h"

#define log_printf printf
#define LOG_PRINTF(format,...) do { log_printf(format, ## __VA_ARGS__); } while(0)
#define LOG_GEN() do { log_printf("=== LOG_GEN: [%s: %u: %s]\n", __FILE__, __LINE__, __PRETTY_FUNCTION__); } while(0)
#define LOG_GEN_PRINTF(format,...) do { log_printf((std::string("=== LOG_GEN: [%s: %u: %s] ")+format).c_str(), __FILE__, __LINE__, __PRETTY_FUNCTION__, ## __VA_ARGS__); } while(0)

namespace QqApiImpl {

using namespace QqApi;

OpSerialNumber g_opSerialNumber(OpSerialNumber_INAVLID);

class QqCommon : public ICommon
{
public:
    QqCommon(Callback *pCallback) : m_pCallback(pCallback), m_domainAvailability(false)
    {
        LOG_GEN();
    }

    virtual ~QqCommon()
    {
        LOG_GEN();
    }
    
    bool initialize()
    {
        LOG_GEN();
        
        //
        // TODO: begin to build up the IPC with BL process
        //
        m_domainAvailability = false; // NOTE: it should be set to true only after the IPC has been built up and all of the APIs are available to call.
        m_pCallback->post(EventId_DomainAvailabilityChanged);

        return true;
    }

    bool deinitialize()
    {
        LOG_GEN();
        
        //
        // TODO: break the IPC here.
        //
        m_domainAvailability = false;
        m_pCallback->post(EventId_DomainAvailabilityChanged);
        return true;
    }

    virtual bool getDomainAvailability()
    {
        LOG_GEN();
        
        return m_domainAvailability;
    }

    virtual HmiScreenId getRequestedHmiScreen()
    {
        LOG_GEN();
        
        //
        // TODO: get the information from the data pool
        //
        return HmiScreenId_HomeScreen;
    }

    virtual FocusDomain getFocusDomain()
    {
        LOG_GEN();
        
        //
        // TODO: get the information from the data pool
        //
        return FocusDomain_BL;
    }

    virtual bool getStatusbarVisibility()
    {
        LOG_GEN();
        
        //
        // TODO: get the information from the data pool
        //
        return true;
    }

    virtual bool getZone1Visibility()
    {
        LOG_GEN();
        
        //
        // TODO: get the information from the data pool
        //
        return true;
    }

    virtual void requestToShowBlScreen(BlScreenId blScreen)
    {
        LOG_GEN();
        
        //
        // TODO: tell BL to draw the request BL screen
        //
        
        //
        // After the BL screen has been drawing onto the wayland surface, the statement as below needs to be called:
        //     m_pCallback->post(EventId_BlScreenDrawn);
        //
    }
    
    virtual OpSerialNumber requestToShowBlScreen2(BlScreenId blScreen)
    {
        LOG_GEN();
        
        ++g_opSerialNumber;
        
        //
        // TODO: tell BL to draw the request BL screen with opSerialNumber (which value is equal to g_opSerialNumber)
        //
        
        return g_opSerialNumber;
        
        //
        // After the BL screen has been drawing onto the wayland surface, the statement as below needs to be called:
        //     m_pCallback->post2(EventId_BlScreenDrawn, opSerialNumber);
        //
    }

    virtual void requestToGetFocus()
    {
        LOG_GEN();
        
        //
        // TODO: tell BL the request
        //
    }

    virtual void requestToGiveUpFocus()
    {
        LOG_GEN();
        
        //
        // TODO: tell BL the request
        //
    }

    virtual void sendHKEvent(HkEventId hkEventId, HkEventType hkEventType)
    {
        LOG_GEN();
        
        //
        // TODO: tell BL the HK event
        //
    }

    virtual void sendIDriveDirectionEvent(IDriveDirHkEventId hkEventId, IDriveDirHkEventType hkEventType)
    {
        LOG_GEN();
        
        //
        // TODO: tell BL the HK event
        //
    }

    virtual void sendIDriveRotationEvent(IDriveTurnDirection turnDirection, unsigned char stepSize)
    {
        LOG_GEN();
        
        //
        // TODO: tell BL the HK event
        //
    }

    virtual void blScreenShown()
    {
        LOG_GEN();
        
        //
        // TODO: tell BL the information
        //
    }

    virtual void blScreenHidden()
    {
        LOG_GEN();
        
        //
        // TODO: tell BL the information
        //
    }

    virtual void hmiScreenShown(HmiScreenId hmiScreen)
    {
        LOG_GEN();
        
        //
        // TODO: tell BL the information
        //
    }
    
    virtual void toShowHmiScreenRejected()
    {
        LOG_GEN();
        
        //
        // TODO: tell BL the information
        //
    }

    virtual void hmiFocusLost()
    {
        LOG_GEN();
        
        //
        // TODO: tell BL the information
        //
    }

    virtual void hmiFocusGot()
    {
        LOG_GEN();
        
        //
        // TODO: tell BL the information
        //
    }

    virtual void statusbarShown()
    {
        LOG_GEN();
        
        //
        // TODO: tell BL the information
        //
    }

    virtual void statusbarHidden()
    {
        LOG_GEN();
        
        //
        // TODO: tell BL the information
        //
    }

    virtual void zone1Shown()
    {
        LOG_GEN();
        
        //
        // TODO: tell BL the information
        //
    }

    virtual void zone1Hidden()
    {
        LOG_GEN();
        
        //
        // TODO: tell BL the information
        //
    }

    virtual void hmiFullscreenPopupShown()
    {
        LOG_GEN();
        
        //
        // TODO: tell BL the information
        //
    }

    virtual void lastHmiFullscreenPopupHidden()
    {
        LOG_GEN();
        
        //
        // TODO: tell BL the information
        //
    }

public:
    Callback *m_pCallback;
    bool m_domainAvailability;
};

class QqPlayer : public IPlayer
{
public:
    QqPlayer(Callback *pCallback) : m_pCallback(pCallback), m_cycleMode(CycleMode_OrderCycle), m_playStatus(PlayStatus_Stopped)
    {
        LOG_GEN();
    }

    virtual ~QqPlayer()
    {
        LOG_GEN();
    }
    
    bool initialize()
    {
        LOG_GEN();

        return true;
    }

    bool deinitialize()
    {
        LOG_GEN();
        
        return true;
    }
    
    virtual std::string getSongName()
    {
        LOG_GEN();
        
        return "(song name)";
    }
    
    virtual std::string getSingerName()
    {
        LOG_GEN();
        
        return "(singer name)";
    }
    
    virtual std::string getAlbumName()
    {
        LOG_GEN();
        
        return "(album name)";
    }
    
    virtual PlayStatus getPlayStatus()
    {
        LOG_GEN();
        
        return m_playStatus;
    }
    
    virtual CycleMode getCycleMode()
    {
        LOG_GEN();
        
        return m_cycleMode;
    }
    
    virtual void setCycleMode(CycleMode cycleMode)
    {
        LOG_GEN();
        
        m_cycleMode = cycleMode;
    }
    
    virtual void play()
    {
        LOG_GEN();
        
        m_playStatus = PlayStatus_Playing;
        m_pCallback->post(EventId_PlayStatusChanged);
    }
    
    virtual void pause()
    {
        LOG_GEN();
    }
    
    virtual void goPrev()
    {
        LOG_GEN();
    }
    
    virtual void goNext()
    {
        LOG_GEN();
    }
    
private:
    Callback *m_pCallback;
    CycleMode m_cycleMode;
    PlayStatus m_playStatus;
};

class QqInterfaceImpl : public Interface
{
public:
    QqInterfaceImpl(Callback *pCallback) : m_qqCommon(pCallback), m_qqPlayer(pCallback)
    {
        LOG_GEN();
    }

    virtual ~QqInterfaceImpl()
    {
        LOG_GEN();
    }

    virtual unsigned int getApiVersion()
    {
        LOG_GEN();
        return currentApiVersion; // NOTE: always return currentApiVersion!
    }

    virtual bool initialize()
    {
        LOG_GEN();
        
        m_qqCommon.initialize();
        m_qqPlayer.initialize();
        return true;
    }

    virtual bool deinitialize()
    {
        LOG_GEN();
        
        m_qqCommon.deinitialize();
        m_qqPlayer.deinitialize();
        return true;
    }
    
    virtual ICommon &getCommonInterface()
    {
        return m_qqCommon;
    }
    
    virtual IPlayer &getPlayerInterface()
    {
        return m_qqPlayer;
    }
    
public:
    QqCommon m_qqCommon;
    QqPlayer m_qqPlayer;
};

} // namespace QqApiImpl {

extern "C" {
QqApi::Interface *getQqInterface(QqApi::Callback *pCallback)  // NOTE: this function will be exported in the DLL, and be loaded dynamically by HMI application
{
    static QqApiImpl::QqInterfaceImpl aQqInterfaceImpl(pCallback);
    bool bSuc = true;
    QqApi::Interface *pInterface = &aQqInterfaceImpl;
    
    LOG_GEN();

    if (bSuc) {
        if (pCallback == NULL) {
            pInterface = NULL;
            bSuc = false;
            
            LOG_GEN();
            
            //
            // TODO: print some error log to indicate that the argument is invalid
            //
            LOG_PRINTF("*** Error: getQqInterface(): pCallback == NULL\n");
        }
    }

    if (bSuc) {
        if (pCallback->getApiVersion() != QqApi::currentApiVersion) {
            pInterface = NULL;
            bSuc = false;
            
            LOG_GEN();
            
            //
            // TODO: print some error log to indicate that the versions don't match
            //
            LOG_PRINTF("*** Error: getQqInterface(): version not matched\n");
        }
    }

    return pInterface;
}
} // extern "C" {
