#ifndef QQ_INTERFACE_H
#define QQ_INTERFACE_H

#include <string>

//
// NOTE:
// 1) All of the strings should be defined as std::string in which conent with UTF8 encoding is saved. 
//
                                                 
namespace QqApi {

const unsigned short currentApiVersionHigh = 2;
const unsigned short currentApiVersionLow = 1;
const unsigned int currentApiVersion = ((currentApiVersionHigh << 16) | currentApiVersionLow);

typedef unsigned long OpSerialNumber;
const OpSerialNumber OpSerialNumber_INAVLID = 0;

enum EventId
{
    EventId_INVALID = 0,

    //
    // operations
    //
    EventId_RequestToShowBlScreen,
    EventId_RequestToShowHmiScreen,
    EventId_RequestToGetFocus,
    EventId_RequestToGiveUpFocus,
    EventId_RequestToShowStatusbar,
    EventId_RequestToHideStatusbar,
    EventId_RequestToShowZone1,
    EventId_RequestToHideZone1,
    
    //
    // notifications
    //
    EventId_DomainAvailabilityChanged,
    EventId_BlScreenDrawn,
    EventId_CurrentBlScreenChanged,
    EventId_StatusbarVisibilityChanged,
    EventId_Zone1VisibilityChanged,
    EventId_RequestedHmiScreenChanged,
    EventId_BlFocusGot,
    EventId_BlFocusLost,
    EventId_FocusDomainChanged,
    
    EventId_SongNameChanged,
    EventId_SingerNameChanged,
    EventId_AlbumNameChanged,
    EventId_PlayStatusChanged,
    EventId_CycleModeChanged,
    
    EventId_MAX
};

enum BlScreenId
{
    BlScreenId_INVALID = 0,
    BlScreenId_MainScreen,
    BlScreenId_PlayingScreen,
    BlScreenId_LastScreen,
    BlScreenId_CurrentScreen,
    BlScreenId_OtherScreen,                 // NOTE: only to be set by BL
    BlScreenId_MAX
};

enum HmiScreenId
{
    HmiScreenId_INVALID = 0,
    HmiScreenId_HomeScreen,
    HmiScreenId_PreviousScreen,
    HmiScreenId_AudioSettingScreen,
    HmiScreenId_NaviScreen,
    HmiScreenId_OtherScreen,                // NOTE: only to be set by HMI
    HmiScreenId_MAX
};

enum FocusDomain
{
    FocusDomain_INVALID = 0,
    FocusDomain_HMI,
    FocusDomain_BL,
    FocusDomain_MAX
};

enum HkEventId
{
    HkEventId_INVALID = 0,
    HkEventId_Home,
    HkEventId_Back,
    HkEventId_MAX
};

enum HkEventType
{
    HkEventType_INVALID = 0,
    HkEventType_Short,
    HkEventType_Long,
    HkEventType_MAX
};

enum IDriveDirHkEventId
{
    IDriveDirHkEventId_INVALID = 0,
    IDriveDirHkEventId_Center,
    IDriveDirHkEventId_Up,
    IDriveDirHkEventId_Down,
    IDriveDirHkEventId_Left,
    IDriveDirHkEventId_Right,
    IDriveDirHkEventId_UpLeft,
    IDriveDirHkEventId_UpRight,
    IDriveDirHkEventId_DownLeft,
    IDriveDirHkEventId_DownRight,
    IDriveDirHkEventId_MAX
};

enum IDriveDirHkEventType
{
    IDriveDirHkEventType_INVALID = 0,
    IDriveDirHkEventType_Press,
    IDriveDirHkEventType_ShortPressRelease,
    IDriveDirHkEventType_HoldingPress,
    IDriveDirHkEventType_HoldingPressRelease,
    IDriveDirHkEventType_MAX
};

enum IDriveTurnDirection
{
    IDriveTurnDirection_INVALID = 0,
    IDriveTurnDirection_Clockwise,
    IDriveTurnDirection_CounterClockwise,
    IDriveTurnDirection_MAX
};

enum CycleMode
{
    CycleMode_INVALID = 0,
    CycleMode_OrderCycle,
    CycleMode_SingleCycle,
    CycleMode_Random,
    CycleMode_MAX
};

enum PlayStatus
{
    PlayStatus_INVALID = 0,
    PlayStatus_Stopped,
    PlayStatus_Playing,
    PlayStatus_Paused,
    PlayStatus_Buffering,
    PlayStatus_FastForwarding,
    PlayStatus_FastBackwarding,
    PlayStatus_MAX
};

class ICommon
{
public:
    // NOTE: the first method
    virtual ~ICommon() {}
    
    //
    // getters
    //
    virtual bool getDomainAvailability() = 0;
    virtual HmiScreenId getRequestedHmiScreen() = 0;
    virtual FocusDomain getFocusDomain() = 0;
    virtual bool getStatusbarVisibility() = 0;
    virtual bool getZone1Visibility() = 0;
    
    //
    // setters
    //
    
    //
    // operations
    //
    virtual void requestToShowBlScreen(BlScreenId blScreen) = 0;
    virtual OpSerialNumber requestToShowBlScreen2(BlScreenId blScreen) = 0;  // reserved
    virtual void requestToGetFocus() = 0;
    virtual void requestToGiveUpFocus() = 0;
    
    virtual void sendHKEvent(HkEventId hkEventId, HkEventType hkEventType) = 0;
    virtual void sendIDriveDirectionEvent(IDriveDirHkEventId hkEventId, IDriveDirHkEventType hkEventType) = 0;
    virtual void sendIDriveRotationEvent(IDriveTurnDirection turnDirection, unsigned char stepSize) = 0;
    
    //
    // notifications
    //
    virtual void blScreenShown() = 0;
    virtual void blScreenHidden() = 0;
    virtual void hmiScreenShown(HmiScreenId hmiScreen) = 0;
    virtual void toShowHmiScreenRejected() = 0;
    virtual void hmiFocusGot() = 0;
    virtual void hmiFocusLost() = 0;
    virtual void statusbarShown() = 0;
    virtual void statusbarHidden() = 0;
    virtual void zone1Shown() = 0;
    virtual void zone1Hidden() = 0;
    virtual void hmiFullscreenPopupShown() = 0;
    virtual void lastHmiFullscreenPopupHidden() = 0;
};

class IPlayer
{
public:
    // NOTE: the first method
    virtual ~IPlayer() {}
    
    //
    // getters
    //
    virtual std::string getSongName() = 0;
    virtual std::string getSingerName() = 0;
    virtual std::string getAlbumName() = 0;
    virtual PlayStatus getPlayStatus() = 0;
    virtual CycleMode getCycleMode() = 0;
    
    //
    // setters
    //
    virtual void setCycleMode(CycleMode cycleMode) = 0;
    
    //
    // operations
    //
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void goPrev() = 0;
    virtual void goNext() = 0;
    
    //
    // notifications
    //
};

class Interface
{
public:
    // NOTE: the first method
    virtual ~Interface() {}
    // NOTE: the second method
    virtual unsigned int getApiVersion() = 0;
    
    virtual bool initialize() = 0;
    virtual bool deinitialize() = 0;
    
    virtual ICommon &getCommonInterface() = 0;
    virtual IPlayer &getPlayerInterface() = 0;
};

class Callback
{
public:
     // NOTE: the first method
     virtual ~Callback() {}
     // NOTE: the second method
     virtual unsigned int getApiVersion() = 0;
     
     virtual void post(EventId event) = 0;
     virtual void post2(EventId event, OpSerialNumber opSerialNumber) = 0;  // reserved
};

} // namespace QqApi {

extern "C" {
QqApi::Interface *getQqInterface(QqApi::Callback *pCallback);                             // exported in the plugin dll provided BL team
} // extern "C" {

#endif // #ifndef QQ_INTERFACE_H
