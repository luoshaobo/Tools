/*
* Copyright (C) 2018 Continental Automotive GmbH
*
* Licensed under the "Supplier unrestricted white box" category, as
* described in the 20160322-1_SW-amendment TCAM.pdf agreement
* between Volvo Cars and Continental Automotive GmbH.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
/**
 *
 * @file vpom.hpp
 * @brief Main handling for VolvoPositioningMananger
 *
 *  \addtogroup vpom
 *  \{
 */

#ifndef VPOM_H
#define VPOM_H

#include "dlt/dlt.h"
#include <signal.h>
#include <thread>
#include <mutex>

extern "C"
{
#include "tpsys.h"
#include "tplog.h"
#include <locationmgr.h>
}
#include "vpom_IPositioningService.hpp"
#include "ipcb_IClient.hpp"
#include <vehiclecomm/vehicle_comm_interface.hpp>
#include <vehiclecomm/vehicle_comm_messages.hpp>
#include "vpom_lcm.hpp"

#define VPOM_LM_INIT_RETRY_DELAY_MS 5000
#define VPOM_SAT_GPS_MIN 1
#define VPOM_SAT_GPS_MAX 32
#define VPOM_SAT_SBAS_MIN 120
#define VPOM_SAT_SBAS_MAX 151
#define VPOM_SAT_GLONASS_MIN 65
#define VPOM_SAT_GLONASS_MAX 96
#define VPOM_SAT_QZSS_MIN 193
#define VPOM_SAT_QZSS_MAX 200
#define VPOM_SAT_GALILEO_MIN 301
#define VPOM_SAT_GALILEO_MAX 336

namespace vpom
{

//Local config begin
typedef enum {
    VPOM_FAKE_POS,
    VPOM_PRINT_GNSS_POS_DATA,
    VPOM_GNSS_POS_DATA_INTERVAL,
    VPOM_START_GNSS_POS_DATA_NOTIFICATIONS,
    VPOM_LOCCONFCONTI_SIZE,
} VpomLocConfConti;

#define VPOM_COLL_CONTI "IPCommandBroker_Conti" //Local config identifier
#define VPOM_FAKE_POS_STR "fake_pos"
#define VPOM_PRINT_GNSS_POS_DATA_STR "print_gnss_pos_data"
#define VPOM_GNSS_POS_DATA_INTERVAL_STR "gnss_pos_data_interval"
#define VPOM_START_GNSS_POS_DATA_NOTIFICATIONS_STR "start_gnss_pos_data_notifications"

typedef struct {
    const std::string coll_name;
    const std::string par_name;
    bool is_string_value;
    int int_value;
    std::string str_value;
} VpomLocConf;

typedef struct {
    int fake_pos;                             ///< Fake position
    int print_gnss_pos_data;                  ///< Print GNSS position data
    int gnss_pos_data_interval;               ///< GNSS position data interval
    int start_gnss_pos_data_notifications;    ///< Start GNSS position data notifications
} Conti;
//Local config end

struct SatelliteInfo {
    uint32_t num_gps_satellites;
    uint32_t num_sbas_satellites;
    uint32_t num_glonass_satellites;
    uint32_t num_galileo_satellites;
    uint32_t num_satellites;
    bool     is_data_valid;      /**< Is satellite data valid */
};

enum SatelliteSystem {
    SAT_UNDEFINED,
    SAT_GPS,
    SAT_SBAS,
    SAT_GLONASS,
    SAT_GALILEO,
    SAT_QZSS
};

} // namespace vpom

class VolvoPositioningManager : public std::thread {

private:
    void LmInit(void);
    bool LmExit(void);
    static void SatelliteCb(tpLM_Satellite_t *info);
    bool getGpsTime(guint64 timestamp_ms, GPSSystemTime *gpsTime);
    enum vpom::SatelliteSystem getGpsSystem(uint16_t id);
    void sendGnssNotifications();
    void sendGnssNotification();
    void PrintLmPos(tpLM_Position_t *pos);
    void PrintVpomPos(GNSSData *pos);
    int32_t DegreesDToFixedPoint32 (double degrees);
    bool LmPosToVpomPos(tpLM_Position_t     *pos_from,
                        tpLM_Time_t         *utc,
                        tpLM_SourceStatus_t *status,
                        GNSSData      *pos_to);
    bool VpomPosToVehicleCommPos(GNSSData *pos_from,
                                 vc::ReqSendPosnFromSatltCon *pos_to);
    void gnssCreateFakePos(GNSSData *pos);
    void ReadLocalConfig();
    bool SetLocalConfigConti(void);

    void VCResponseCb(vc::MessageBase* message, vc::ReturnValue status);
    void EventCallback(vc::MessageBase* message);

    bool CheckNotifyCondition();  //New add
    bool GnssPositionCopy(GNSSData *p_dest, GNSSData *p_src);
    bool VpomPosToVgmPos(OpPosnFromSatltPush_NotificationCyclic_Data *pos_to, GNSSData *pos_from);
    bool VehModandDateCopy(VehModMngtGlbSafe1_Data *info_dest, VehModMngtGlbSafe1_Data *info_src);
    void VehModandDateInfoNotificationCycleCb(VehModMngtGlbSafe1_Data *info);
    void PrintVehModandDateInfo(VehModMngtGlbSafe1_Data* info);
    static void VpomServiceProxy(OperationId id, OperationType type,uint64_t requestId, DataBase* data);

    bool setPosnFromSatlt_Data(OpPosnFromSatltPush_NotificationCyclic_Data *pos_to, GNSSData *pos_from);
    bool setOpPosnFromSatltPush_Data(PosnBrief_Data *pos_to, GNSSData *pos_from);
    bool setOpPosnFromSatltPush_Data(BiasedPosnFromSatlt_Data *pos_to, GNSSData *pos_from);
    bool setOpPosnFromSatltPush_Data(GNSSSolution_Data *pos_to, GNSSData *pos_from);
    
    GNSSData m_posData;
    VehModMngtGlbSafe1_Data m_vehModandDateInfo;    
    bool m_ipNetworkIsActive;
    IpcbIPositioningService &m_IpcbPosSrv;
    vc::CarModeState carMode;
    vc::CarUsageModeState usageMode;
    
    
    bool active_;
    bool lmIsReady;
    bool notifyLoop_;
    bool notifyLoopEnd_;
    vpom::SatelliteInfo satellite_info_;
    std::mutex gnss_position_mutex_;
    std::mutex gnss_subscriber_mutex_;
    std::mutex gnss_lm_timer_mutex_;
    long vc_session_id;

    //Local config begin
    vpom::VpomLocConf vpomLocConfConti_[vpom::VPOM_LOCCONFCONTI_SIZE] = {
            {VPOM_COLL_CONTI, VPOM_FAKE_POS_STR, false, 1, ""},
            {VPOM_COLL_CONTI, VPOM_PRINT_GNSS_POS_DATA_STR, false, 1, ""},
            {VPOM_COLL_CONTI, VPOM_GNSS_POS_DATA_INTERVAL_STR, false, 1000, ""},
            {VPOM_COLL_CONTI, VPOM_START_GNSS_POS_DATA_NOTIFICATIONS_STR, false, 0, ""},
    };
    vpom::Conti conti_;
    //Local config end

    std::thread notifyThread_;

    int Suspend(void);
    int Resume(void);
    int Stop(void);
    static int LcmHandler(VpomLcmSignal sig);

public:

    /**
     * @brief    Constructor
     */
    VolvoPositioningManager();

    /**
     * @brief   Destructor
     */
    ~VolvoPositioningManager();

    IpcbIGNSSService & ipcbGnssProxy_;

    /**
     * @brief   Instance getter
     */
    static VolvoPositioningManager &getInstance();

    /**
    * @brief                                   Request of postion data from location manager
    *
    * @return                                  True if ok.
    *                                          False otherwise.
    */
    bool LmGetPosition(GNSSData *posData);

    /**
    * @brief                                   Request for raw position data
    *
    * @return                                  True if ok.
    *                                          False otherwise.
    */
    void PositionDataRawRequest(uint64_t request_id);

    /**
    * @brief                                   Request for raw position data
    *
    * @return                                  True if ok.
    *                                          False otherwise.
    */
    static void PositionDataRawRequestCb(uint64_t request_id);
    //static void VpomServiceProxy(OperationId id, OperationType type,uint64_t requestId, DataBase* data);

    /**
    * @brief                                   Initialization of volvo positioning manager
    *
    * @return                                  True if initialization was ok.
    *                                          False otherwise.
    */
    bool Init();

    /**
    * @brief                                   De-initialization of volvo positioning manager
    *
    * @return                                  True if de-initialization was ok.
    *                                          False otherwise.
    */
    void Deinit();
};

#endif // VPOM_H

/** \}    end of addtogroup */
