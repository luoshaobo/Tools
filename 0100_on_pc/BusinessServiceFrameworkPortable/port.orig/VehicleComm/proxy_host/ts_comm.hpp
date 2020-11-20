#ifndef TS_COMM_HPP
#define TS_COMM_HPP

/**
* Copyright (C) 2016 Continental Automotive GmbH
*
* Licensed under the "Supplier unrestricted white box" category, as
* described in the 20160322-1_SW-amendment TCAM.pdf agreement
* between Volvo Cars and Continental Automotive GmbH.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/

/** @file ts_comm.hpp
 * This file implements communication with telematics over dbus interface
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Johan Bohlin & Ola Lilja
 * @date        07-Dec-2016
 ***************************************************************************/

#include "vehicle_comm_messages.hpp"
#include "vc_message_queue.hpp"

#include "vc_proxy_host.hpp"

namespace vc {

/**
    @brief Routing-table for requests coming from TS
*/
static const VCRequestDestination routing_table_ts[] = {
    {
        .request_id = REQ_UNKNOWN,
        .dst = ENDPOINT_UNKNOWN
    },

    // VuC
    {
        .request_id = REQ_DOORLOCKUNLOCK,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_VUCTRACE_CONFIG,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_VUCTRACE_CONTROL,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_HORNNLIGHT,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_SWITCHTOBOOT,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_ANTENNASWITCH,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_LANLINKTRIGGER,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_ADCVALUE,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_BLE_DISCONNECT,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_BLE_DISCONNECTALL,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_BLE_GENERATEBDAK,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_BLE_GETBDAK,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_BLE_CONFIG_DATACOMSERVICE,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_BLE_CONFIG_IBEACON,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_BLE_CONFIG_GENERICACCESS,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_BLE_STARTPARINGSERVICE,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_BLE_STOPPARINGSERVICE,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_BLE_WAKEUP_RESPONSE,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_BLE_DATAACCESSAUTHCONFIRMED,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_BLE_STARTADVERTISING,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_BLE_STOPADVERTISING,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_DTCENTRIES,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_DTCCTLSETTINGS,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_IPWAKEUP,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_SETOHCLED,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_GETBELTINFORMATION,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_GETDIAGERRREPORT,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_CARCONFIGFACTRESTORE,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_GETDIDGLOBALSNAPSHOTDATA,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_DTCSDETECTED,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_PSIMDATA,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_THERMALMITIGATION,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_CARCFGPARAMETERFAULT,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_NETWORK_MANAGEMENT_HISTORY,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_PROGRAM_PRECOND_CHK,
        .dst = ENDPOINT_VUC
    },
#if 1 // nieyj
    {
        .request_id = REQ_WINCTRL,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_ROOFCTRL,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_WINVENTI,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_PM25ENQUIRE,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_VFCACTIVATE,
        .dst = ENDPOINT_VUC
    },
#endif
    //uia93888 remote start engine
    {
        .request_id = REQ_RMTENGINE,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_RMTENGINESECURITYRESPONSE,
        .dst = ENDPOINT_VUC
    },
    //uia93888 RTC awake Vuc
    {
        .request_id = REQ_RTCAWAKEVUC,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_DELAYENGINERUNNINGTIME,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_OPERATEREMOTECLIMATE,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_OPERATERMTSEATHEAT,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_OPERATERMTSEATVENTILATION,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_PARKINGCLIMATEOPER,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_OPERATERMTSEATHEAT,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_OPERATERMTSTEERWHLHEAT,
        .dst = ENDPOINT_VUC
    },
    //end uia93888
    {
        .request_id = REQ_ROLLOVERVALUE,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_SETROLLOVERCFG,
        .dst = ENDPOINT_VUC
    },
    // VGM
    {
        .request_id = REQ_SENDPOSNFROMSATLTCON,
        .dst = ENDPOINT_VGM
    },
    // IHU
    // Data-storage
    {
        .request_id = REQ_GETDSPARAMETER,
        .dst = ENDPOINT_DS
    },

    // Message-processor
    {
        .request_id = REQ_GETVUCSWBUILD,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETBLESWBUILD,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_VUCSWVERSION,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_BLESWVERSION,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_LASTBUBSTATE,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_CARUSAGEMODE,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_CARMODE,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_ELECTENGLVL,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETVUCPOWERMODE,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETVUCWAKEUPREASON,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETWIFIDATA,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETPSIMAVAILABILITY,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETFUELTYPE,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETPROPULSIONTYPE,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETOHCBTNSTATE,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETNUMBEROFDOORS,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETSTEERINGWHEELPOSITION,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETFUELTANKVOLUME,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETASSISTANCESERVICES,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETAUTONOMOUSDRIVE,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETCOMBINEDINSTRUMENT,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETTELEMATICMODULE,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETCONNECTIVITY,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETCONNECTEDSERVICEBOOKING,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETRFVERSION,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETTCAMHWVERSION,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETFBLSWBUILD,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_VINNUMBER,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETCARCFGTHEFTNOTIFICATION,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETCARCFGSTOLENVEHICLETRACKING,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETGNSSRECEIVER,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETAPPROVEDCELLULARSTANDARDS,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETDIGITALVIDEORECORDER,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETDRIVERALERTCONTROL,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETDOORSTATE,
        .dst = ENDPOINT_MP
    },
    // uia93888 remote start engine
    {
        .request_id = REQ_GETRMTSTATUS,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_GETENGINESTATES,
        .dst = ENDPOINT_MP
    },
    // Test
    {
        .request_id = REQ_VGMTESTMSG,
        .dst = ENDPOINT_VGM
    },
    {
        .request_id = REQ_IHUTESTMSG,
        .dst = ENDPOINT_IHU
    },
    {
        .request_id = REQ_TEST_SETFAKEVALUESDEFAULT,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_TEST_SETFAKEVALUEINT,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_TEST_SETFAKEVALUESTR,
        .dst = ENDPOINT_MP
    },
    {
        .request_id = REQ_TEST_TOGGLEUSAGEMODE,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_TEST_TOGGLECARMODE,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_TEST_CRASHSTATE,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_TEST_OHCSTATE,
        .dst = ENDPOINT_VUC
    },
    {
        .request_id = REQ_TEST_SETLOCALCONFIG,
        .dst = ENDPOINT_VUC
    },
};

/**
    @brief VC-endpoint handling communication with VehicleCommClients

    This class is a VC-endpoint handling requests from a VehicleCommClient and
    responses and events to a VehicleCommClient.
*/
class TSComm {
private:
    static VCRequestDestination dst_lookup[];       /**< Routing-table for requests coming from TS */

    static Endpoint DestinationLookup(RequestID request_id);

    MessageQueue *mq_;               /**< The main message-queue, belonging to the VehicleComm-instance */
    VCProxyHost gdbus_;             /**< GDBus-part of TSComm */

    ReturnValue AddIncMessage(int id, long unique_id, int client_id, long session_id, const unsigned char *data);

public:
    TSComm(MessageQueue *mq);

    ReturnValue Init();
    ReturnValue Start();
    ReturnValue SendMessage(MessageBase *m);
};

} // namespace vc

#endif // TSCOMM_HPP
