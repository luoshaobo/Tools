///////////////////////////////////////////////////////////////////
// Copyright (C) 2019 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file car_downloadagent_transaction.h
//  This file handles OTA
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author
// @Init date   14-Feb-2019
///////////////////////////////////////////////////////////////////

#ifndef VOC_TRANSACTIONS_CAR_DOWNLOAD_AGENT_TRQANSACTION_H_
#define VOC_TRANSACTIONS_CAR_DOWNLOAD_AGENT_TRQANSACTION_H_

#include "voc_framework/transactions/smart_transaction.h"
#include "voc_framework/signals/ip_command_broker_signal.h"
#include "voc_framework/signal_sources/ip_command_broker_signal_source.h"
#include "signals/geely/gly_vds_dlagent_signal.h"
#include "timestamp.h"
#include "../voc_persist_data_mgr/include/voc_persist_data_mgr.h"
#include "voc_framework/src/http/http_get.h"
#include "voc_framework/signals/http_signal.h"

namespace volvo_on_call
{

class CarDownloadAgentTransaction : public fsm::SmartTransaction
{
 public:

    /**
     * \brief Creates a "list paired devices" transaction.
     */
    CarDownloadAgentTransaction();

    ////////////////////////////////////////////////////////////
    // @brief : Initialize
    // @param[in]
    // @return     void
    // @author     Tian Yong, Jan 29, 2019
    ////////////////////////////////////////////////////////////
    void init();

    /**
     * @brief handle OTAAssignmentNotification signal.
     * @param[in] signal                        received signal
     * @return                                  True if operation was ok.
     *                                          False otherwise.
     * @author  Tian Yong, 06-Dec-2018
     */
    bool HandleOTAAssignmentNotification(std::shared_ptr<fsm::Signal> signal);

    /**
     * @brief handle OTAUpdateInventoryInstructionsResponse signal.
     * @param[in] signal                        received signal
     * @return                                  True if operation was ok.
     *                                          False otherwise.
     * @author  Tian Yong, 06-Dec-2018
     */
    bool HandleOTAUpdateInventoryInstructionsResponse(std::shared_ptr<fsm::Signal> signal);

    /**
     * @brief handle OTAClientConfigurationSyncRequest signal.
     * @param[in] signal                        received signal
     * @return                                  True if operation was ok.
     *                                          False otherwise.
     * @author  Tian Yong, 06-Dec-2018
     */
    bool HandleOTAClientConfigurationSyncRequest(std::shared_ptr<fsm::Signal> signal);

    //////////////////////////////////////////////////////
    // @brief : handle OTAAssignmentSyncReq signal.
    // @param[in]  signal     received signal
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Tian Yong, Jan 30, 2019
    //////////////////////////////////////////////////////
    bool HandleOTAAssignmentSyncRequest(std::shared_ptr<fsm::Signal> signal);

    //////////////////////////////////////////////////////
    // @brief : handle kOTAHMILanguageSettingsNotification signal.
    // @param[in]  signal     received signal
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Tian Yong, Jan 31, 2019
    //////////////////////////////////////////////////////
    bool HandleOTAHMILanguageSettings(std::shared_ptr<fsm::Signal> signal);

    //////////////////////////////////////////////////////
    // @brief : handle kDownloadConsentNotification signal.
    // @param[in]  signal     received signal
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Shen Jiaojiao, Feb 27, 2019
    //////////////////////////////////////////////////////
    bool HandleDownloadConsent(std::shared_ptr<fsm::Signal> signal);

    //////////////////////////////////////////////////////
    // @brief : handle GlyVdsDLAgent signal.
    // @param[in]  signal     received signal
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Tian Yong, Mar 4, 2019
    //////////////////////////////////////////////////////
    bool HandleVdsFOTASignal(std::shared_ptr<fsm::Signal> signal);

    //////////////////////////////////////////////////////
    // @brief : handle kOTAQueryAvailableStorageResp signal.
    // @param[in]  signal     received signal
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Shen Jiaojiao, Mar 15, 2019
    //////////////////////////////////////////////////////
    bool HandleOTAQueryAvailableStorageResp(std::shared_ptr<fsm::Signal> signal);

    // @brief : handle InstallationSummaryNotificationSignal signal.
    // @param[in]  signal     received signal
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Li Jianhui, Mar 18, 2019
    //////////////////////////////////////////////////////
    bool HanldeInstallationSummaryNotification(std::shared_ptr<fsm::Signal> signal);

    // @brief : handle ExceptionReportNotificationSignal signal.
    // @param[in]  signal     received signal
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Tan Chang, Mar 29, 2019
    //////////////////////////////////////////////////////
    bool HandleExceptionReportNotification(std::shared_ptr<fsm::Signal> signal);

    //////////////////////////////////////////////////////
    // @brief : handle kInstallationConsentNotification signal.
    // @param[in]  signal     received signal
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Shen Jiaojiao, Mar 15, 2019
    //////////////////////////////////////////////////////
    bool HandleInstallationConsent(std::shared_ptr<fsm::Signal> signal);

    //////////////////////////////////////////////////////
    // @brief : handle kOTAAssignBOOTResp signal.
    // @param[in]  signal     received signal
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Shen Jiaojiao, Mar 15, 2019
    //////////////////////////////////////////////////////
    bool HandleOTAAssignBOOTResp(std::shared_ptr<fsm::Signal> signal);
    
    //////////////////////////////////////////////////////
    // @brief : handle http callback message signal.
    // @param[in]  signal     received signal
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Shen TianYong, Mar 21, 2019
    //////////////////////////////////////////////////////
    bool HandleHttpGetCallbackMesage(std::shared_ptr<fsm::Signal> signal);

    //////////////////////////////////////////////////////
    // @brief : handle WriteAssignmentDataResponse signal.
    // @param[in]  signal     received signal
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Shen TianYong, Mar 25, 2019
    //////////////////////////////////////////////////////
    bool HandleWriteAssignmentDataResponse(std::shared_ptr<fsm::Signal> signal);

    //////////////////////////////////////////////////////
    // @brief : Receive urls from tsp and start to download.
    // @param[in]  signal     received signal
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Shen TianYong, Mar 21, 2019
    //////////////////////////////////////////////////////
    bool HanldeOTAAssignmentData(fsm::AssignmentData_Data &data);

    //////////////////////////////////////////////////////
    // @brief : Send Assignment Assignment Data.
    // @param[in]  data         Available assignment data from TSP.
    // @param[in]  command
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Tian Yong, Mar 4, 2019
    //////////////////////////////////////////////////////
    bool SendAvailableAssignmentData(fsm::AvailableAssignment_Data &data, App_ServiceCommand command);

    //////////////////////////////////////////////////////
    // @brief : Send Assignment Notification Data.
    // @param[in]  msg     AssignmentNotification_t
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Shen Jiaojiao, Feb 27, 2019
    //////////////////////////////////////////////////////
    bool SendAssignmentNotificationData(fsm::FotaAssignmentNotification_Data &msg);

    //////////////////////////////////////////////////////
    // @brief : Send AvailableAssignmentRequest Data.
    // @param[in]  msg     AssignmentNotification_t
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Tian Yong, Mar 4, 2019
    //////////////////////////////////////////////////////
    bool SendAvailableAssignmentRequestData(fsm::AvailableAssignmentRequest_Data &msg);

    //////////////////////////////////////////////////////
    // @brief : Send ExceptionReportNotification Data.
    // @param[in]  msg     EcuExceptionReport_Data
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Tan Chang, Mar 29, 2019
    //////////////////////////////////////////////////////
    bool SendExceptionReportNotificationData(fsm::EcuExceptionReport_Data &msg);

    //////////////////////////////////////////////////////
    // @brief : Send InstallationSummary Notification Data.
    // @param[in]  msg     InstallationInstruction_Data
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Li Jianhui, Feb 27, 2019
    //////////////////////////////////////////////////////
    bool SendInstallationSummaryNotificationData(const fsm::InstallationSummary_Data &msg);

    //////////////////////////////////////////////////////
    // @brief : Confirm Configuration Status.
    // @param[in]  sConfigurationStatus     Configuration Status
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Shen Jiaojiao, Feb 27, 2019
    //////////////////////////////////////////////////////
    bool ConfirmConfigurationStatus(std::string sConfigurationStatus);

    //////////////////////////////////////////////////////
    // @brief : Send OTAQueryAvailableStorage message.
    // @param[in]
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Shen Jiaojiao, Mar 15, 2019
    //////////////////////////////////////////////////////
    bool SendOTAQueryAvailableStorage();

    //////////////////////////////////////////////////////
    // @brief : Send file data to vgm.
    // @param[in]  data
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     TinaYong, Mar 25, 2019
    //////////////////////////////////////////////////////
    bool SendFileData(const HttpData &data);

    //////////////////////////////////////////////////////
    // @brief : receive http callback message.
    // @param[in]  callback     callback message
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Tian Yong, Mar 21, 2019
    //////////////////////////////////////////////////////
    void httpCallBack(ResponseCommunicatorDataHttpsGet &callback);

private:

    //////////////////////////////////////////////////////
    // @brief : Convert OpOTAInstallationSummary_Notification_Data to .InstallationInstruction_Data
    // @param[in]  ipcbData    OpOTAInstallationSummary_Notification_Data
    // @param[out]  msg    InstallationInstruction_Data
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Li Jianhui, Mar 19, 2019
    //////////////////////////////////////////////////////
    bool Convert(const OpOTAInstallationSummary_Notification_Data& data,fsm::InstallationSummary_Data &msg);

        //////////////////////////////////////////////////////
    // @brief : Convert OpOTAExceptionReports_Notification_Data to .EcuExceptionReport_Data
    // @param[in]  ipcbData    OpOTAInstallationSummary_Notification_Data
    // @param[out]  msg    EcuExceptionReport_Data
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Tan Chang, Mar 29, 2019
    //////////////////////////////////////////////////////
    bool Convert(const OpOTAExceptionReports_Notification_Data& data,fsm::EcuExceptionReport_Data &msg);

    //////////////////////////////////////////////////////
    // @brief : get local config.
    // @param[in]
    // @return
    // @author     Shen Jiaojiao, Feb 27, 2019
    //////////////////////////////////////////////////////
    bool LoadPersistData();

    ////////////////////////////////////////////////////////////
    // @brief :format string from a utc time
    // @param[in]
    // @return     format string
    // @author     Shen Jiaojiao, Feb 27, 2019
    ////////////////////////////////////////////////////////////
    std::string GetNowTimeStamp();

    ////////////////////////////////////////////////////////////
    // @brief :format Timestamp to string
    // @param[in]  ts Timestamp
    // @return     format string
    // @author     Shen Jiaojiao, Feb 27, 2019
    ////////////////////////////////////////////////////////////
    std::string DateTimeToString(Timestamp ts);

    //////////////////////////////////////////////////////
    // @brief : get utc time.
    // @param[in]
    // @return     timestamp
    // @author     Shen Jiaojiao, Feb 27, 2019
    //////////////////////////////////////////////////////
    Timestamp::TimeVal GetUtcTime()
    {
        Timestamp now;
        return now.epochMicroseconds();
    }

    //////////////////////////////////////////////////////
    // @brief : Send StartDownload Notification to IHU and TSP.
    // @param[in]  uuid
    // @param[in]  timeStamp
    // @return     True if operation was ok.
    //             False otherwise.
    // @author     Tian Yong, Apr 1, 2019
    //////////////////////////////////////////////////////
    void sendStartDownloadNotification(std::string &uuid, std::string &timeStamp);

    void clearDownloadStatus();

public:
    enum States
    {
        kStart = 0,      ///< Start.
        kHandleSignal,   ///< handle signal.
        kCanceled,       ///< Canceled.
        kFailed,         ///< Failed.
        kComplete,       ///< Complete.
        kRejected,       ///< Rejected.
        kStop,           ///< Stop.
    };

private:
    struct{
        uint32_t chunkNumber;
        uint32_t chunks;
        uint32_t chunkSize;             // unit byte
        std::string fileName;
        uint64_t fileSize;              // unit byte
        uint32_t curentUrl;
        std::vector<std::string> urls;
        std::shared_ptr<fsm::t_http_get> pHttpGet;
    }m_downloadStatus;
    
    IpcbIOTAService &m_OTAService;

    std::string m_configurationStatus;
    bool m_downloadConsent;

    std::string m_installationOrderId;
    uint32_t m_language;
    uint64_t m_availablestorage;
    uint32_t m_installationconsent;
};

} // namespace volvo_on_call

#endif //VOC_TRANSACTIONS_CAR_DOWNLOAD_AGENT_TRQANSACTION_H_

/** \}    end of addtogroup */

