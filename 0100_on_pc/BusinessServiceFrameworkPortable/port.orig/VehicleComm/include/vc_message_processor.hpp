#ifndef VC_MESSAGE_PROCESSOR_HPP
#define VC_MESSAGE_PROCESSOR_HPP

/**
* Copyright (C) 2017 Continental Automotive GmbH
*
* Licensed under the "Supplier unrestricted white box" category, as
* described in the 20160322-1_SW-amendment TCAM.pdf agreement
* between Volvo Cars and Continental Automotive GmbH.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/

/** @file vc_message_processor.hpp
 * This file handles message processing for VehicleComm
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Johan Bohlin & Ola Lilja
 * @date        23-Feb-2017
 ***************************************************************************/

#include <thread>
#include <condition_variable>

#include "vehicle_comm_messages.hpp"
#include "vc_message_queue.hpp"
#include "vc_common.hpp"
#include "vc_data_storage.hpp"

namespace vc {

#define LOG_VC_F(fp, loglevel, ...) do { \
        LOG(loglevel, ##__VA_ARGS__); \
        fprintf(fp, ##__VA_ARGS__); \
        fprintf(fp, "\n"); \
    } while (0)

#define DSGETINT_MTX(par, collection, param, is_locked) do { \
            par = reinterpret_cast<Parameter<int>*>(ds_->FindParameter(collection, param)); \
            if (!par) { \
                if (is_locked) \
                    ds_->StopReadWriteDepValues(); \
                LOG(LOG_WARN, "MessageProcessor: %s: Parameter %s (collection = %s) not found in DataStorage!", __FUNCTION__, param, collection); \
                return RET_ERR_VALUE_NOT_AVAILABLE; \
            } \
        } while (0)

#define DSGETSTR_MTX(par, collection, param, is_locked) do { \
            par = reinterpret_cast<Parameter<std::string>*>(ds_->FindParameter(collection, param)); \
            if (!par) { \
                if (is_locked) \
                    ds_->StopReadWriteDepValues(); \
                LOG(LOG_WARN, "MessageProcessor: %s: Parameter %s (collection = %s) not found in DataStorage!", __FUNCTION__, param, collection); \
                return RET_ERR_VALUE_NOT_AVAILABLE; \
            } \
        } while (0)

#define DSGETCHARARR_MTX(par, collection, param, is_locked) do { \
            par = reinterpret_cast<Parameter<std::vector<unsigned char>>*>(ds_->FindParameter(collection, param)); \
            if (!par) { \
                if (is_locked) \
                    ds_->StopReadWriteDepValues(); \
                LOG(LOG_WARN, "MessageProcessor: %s: Parameter %s (collection = %s) not found in DataStorage!", __FUNCTION__, param, collection); \
                return RET_ERR_VALUE_NOT_AVAILABLE; \
            } \
        } while (0)

#define DSGETSTR(par, collection, param) DSGETSTR_MTX(par, collection, param, false)
#define DSGETINT(par, collection, param) DSGETINT_MTX(par, collection, param, false)
#define DSGETCHARARR(par, collection, param) DSGETCHARARR_MTX(par, collection, param, false)
#define COLL_DSGETSTR(par, collection, param) DSGETSTR_MTX(par, collection, param, true)
#define COLL_DSGETINT(par, collection, param) DSGETINT_MTX(par, collection, param, true)
#define COLL_DSGETCHARARR(par, collection, param) DSGETCHARARR_MTX(par, collection, param, true)
#define COLL_DEPREADWRITE_START() ds_->StartReadWriteDepValues();
#define COLL_DEPREADWRITE_STOP() ds_->StopReadWriteDepValues();

const uint8_t MESSAGE_CATALOGUE_VERSION[] = {0x13, 0x04, 0x01};

typedef struct {
    RequestID request_id;
    char* data;
    Endpoint endpoint;
    bool completed;
    int tries;
    long session;
} MpInitRequestIds;


/**
    @brief A class performs operations on request from user to vehicle comm

    This class executes commands from user. Example could be that a user send
    one command and MessageProcessor does several operations before answering.
*/

class MessageProcessor {
  private:
    static MessageQueue *mq_;                   /**< A list of sessions keeping track of requests sent to the VuC */
    static DataStorage *ds_;                    /**< Pointer to access datastorage directly */

    bool exit_ = false;
    std::list<MpInitRequestIds> req_list_;
    FILE *fp_;

    // Handling-functions for individual requests
    ReturnValue HandleVucSwBuild(MessageBase *m);
    ReturnValue HandleBleSwBuild(MessageBase *m);
    ReturnValue HandleVucSwVersion(MessageBase *m);
    ReturnValue HandleBleSwVersion(MessageBase *m);
    ReturnValue HandleLastBubState(MessageBase *m);
    ReturnValue HandleCarCfgParaFaults(MessageBase *m);
    ReturnValue HandleCarUsageMode(MessageBase *m);
    ReturnValue HandleCarMode(MessageBase *m);
    ReturnValue HandleElectEngLvl(MessageBase *m);
    ReturnValue HandleGetVucPowerMode(MessageBase *m);
    ReturnValue HandleGetVucWakeupReason(MessageBase *m);
    ReturnValue HandleVINNumber(MessageBase *m);
    ReturnValue HandleGetWifiData(MessageBase *m);
    ReturnValue HandleGetPSIMAvailability(MessageBase *m);
    ReturnValue HandleGetFuelType(MessageBase *m);
    ReturnValue HandleGetPropulsionType(MessageBase *m);
    ReturnValue HandleGetOhcBtnState(MessageBase *m);
    ReturnValue HandleGetNumberOfDoors(MessageBase *m);
    ReturnValue HandleGetSteeringWheelPosition(MessageBase *m);
    ReturnValue HandleGetFuelTankVolume(MessageBase *m);
    ReturnValue HandleGetAssistanceServices(MessageBase *m);
    ReturnValue HandleGetAutonomousDrive(MessageBase *m);
    ReturnValue HandleGetCombinedInstrument(MessageBase *m);
    ReturnValue HandleGetTelematicModule(MessageBase *m);
    ReturnValue HandleGetConnectivity(MessageBase *m);
    ReturnValue HandleGetConnectedServiceBooking(MessageBase *m);
    ReturnValue HandleGetTcamHwVersion(MessageBase *m);
    ReturnValue HandleGetRfVersion(MessageBase *m);
    ReturnValue HandleGetFblSwBuild(MessageBase *m);
    ReturnValue HandleGetCarCfgTheftNotification(MessageBase *m);
    ReturnValue HandleGetCarCfgStolenVehicleTracking(MessageBase *m);
    ReturnValue HandleGetGNSSReceiver(MessageBase *m);
    ReturnValue HandleGetApprovedCellularStandards(MessageBase *m);
    ReturnValue HandleGetDigitalVideoRecorder(MessageBase *m);
    ReturnValue HandleGetDriverAlertControl(MessageBase *m);
    ReturnValue HandleGetDoorState(MessageBase *m);
    //add by uia93888 
    ReturnValue HandleGetRMTStatus(MessageBase *m);
    ReturnValue HandleGetEngineStates(MessageBase *m);

    void ProcessParameterChange(const std::string& coll_name, const ParameterBase& p);

    // Message-processor tasks

    // Task - Check for end-of-trip
    CarUsageModeState usage_mode_endoftrip = CAR_ABANDONED;
    CarModeState car_mode_endoftrip = CAR_NORMAL;
    ReturnValue Task_CheckEndOfTrip(const ParameterBase& p);

    // Task - Initial request (on start)
    int cur_session_init_req_;
    std::mutex mtx_init_requests_;
    std::condition_variable cv_init_requests_;
    std::thread thr_init_requests_;
    void PrintVersionsToFile(MessageBase *m);
    void Task_InitRequests();
    ReturnValue HandleInitResponse(MessageBase *m);

    // Test
    ReturnValue HandleTestSetFakeValuesDefault(MessageBase *m);
    ReturnValue HandleTestSetFakeValueInt(MessageBase *m);
    ReturnValue HandleTestSetFakeValueStr(MessageBase *m);

  public:
    MessageProcessor(MessageQueue *mq, DataStorage *ds);
    ~MessageProcessor();

    ReturnValue Init();
    ReturnValue Start();
    ReturnValue Stop();
    ReturnValue SendMessage(MessageBase *m);
};

} // namespace vc

#endif // VC_MESSAGE_PROCESSOR_HPP
