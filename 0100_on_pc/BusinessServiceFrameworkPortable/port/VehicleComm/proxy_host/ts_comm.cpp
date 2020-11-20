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

/** @file ts_comm.cpp
 * This file implements communication with telematics over dbus interface
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Johan Bohlin & Ola Lilja
 * @date        07-Dec-2016
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <getopt.h>
#include <glib.h>
#include <dbus/dbus.h>
#include <dbus/dbus-shared.h>
#include "tplog.h"

#include "vc_utility.hpp"

#include "ts_comm.hpp"

namespace vc {

/**
    @brief Get a Endpoint from a RequestID.

    This method will go through the entries in the routing-table
    and find the Endpoint for a RequestID.

    @param[in]  request_id     A RequestID to be looked up.
    @return     A VCDestination
*/
Endpoint TSComm::DestinationLookup(RequestID request_id)
{
    for (int i = 0; i < LENGTH(routing_table_ts); i++)
        if (routing_table_ts[i].request_id == request_id)
            return routing_table_ts[i].dst;

    return ENDPOINT_UNKNOWN;
};

/**
    @brief A message has arrived from a VehicleCommClient-user.

    This callback is called from VCProxyHost, when a message
    has arrived from a VehicleCommClient-user, via D-bus, to VehicleComm.

    @param[in]  id            A MessageID
    @param[in]  unique_id     The unique identifier of the incoming message created
    @param[in]  client_id     The identifier of the VehicleCommClient-user
    @param[in]  session_id    An identifier assigned to this request
    @param[in]  data          The content from the user
    @return     A ReturnValue
*/
ReturnValue TSComm::AddIncMessage(int id, long unique_id, int client_id, long session_id, const unsigned char* data)
{
    LOG(LOG_DEBUG, "TSComm: %s: New message: %s (unique_id = %d, client_id = %d, session_id = %d)",
            __FUNCTION__, MessageStr((MessageID)id, MESSAGE_REQUEST), unique_id, client_id, session_id);

    Endpoint dst = DestinationLookup((RequestID)id);
    LOG(LOG_DEBUG, "TSComm: %s: Message-destination: %s", __FUNCTION__, EndpointStr[dst]);

    MessageBase *m = InitMessage((MessageID)id, MESSAGE_REQUEST, client_id, session_id,
                                ENDPOINT_TS, dst, data);
    if (m == NULL) {
        LOG(LOG_WARN, "TSComm: %s InitMessage failed!", __FUNCTION__);
        return RET_ERR_INTERNAL;
    }

    ReturnValue ret = mq_->AddMessage(m);
    if (ret != RET_OK) {
        LOG(LOG_WARN, "TSComm: %s AddMessage failed (err = %s)!", __FUNCTION__, ReturnValueStr[ret]);
        return ret;
    }

    return RET_OK;
}



// Public

TSComm::TSComm(MessageQueue *mq)
    : gdbus_([this](int id, long unique_id, int client_id, int session_id,
                const unsigned char* data){return TSComm::AddIncMessage(id, unique_id, client_id, session_id, data);})
{
    mq_ = mq;
}

/**
    @brief Initialize TSComm.

    Do all initialization stuff needed for TSComm.
*/
ReturnValue TSComm::Init()
{
    LOG(LOG_DEBUG, "TSComm: %s.", __FUNCTION__);

    if(gdbus_.Init() != RET_OK) {
        LOG(LOG_ERR, "TSComm:  %s: gdbus Init failed", __FUNCTION__);
        return RET_ERR_EXTERNAL;
    }

    return RET_OK;
}

/**
    @brief Start execution of TSComm thread.

    Currently, TSComm does not need its own thread.

    @return     A VC return-value
*/
ReturnValue TSComm::Start()
{
    LOG(LOG_DEBUG, "TSComm: %s.", __FUNCTION__);
    return RET_OK;
}

/**
    @brief Send message to TS/VehicleCommClient.

    This method is used by the user of TSComm to send a Message to the TS/VehicleCommClient.

    @return     A VC return-value
*/
ReturnValue TSComm::SendMessage(MessageBase *m)
{
    ReturnValue ret;
    LOG(LOG_DEBUG, "TSComm: %s.", __FUNCTION__);

    if (m == NULL) {
        LOG(LOG_ERR, "TSComm: %s: Message is NULL!", __FUNCTION__);
        return RET_ERR_INVALID_ARG;
    }

    ret = gdbus_.SendMessage(m->id_, m->type_, m->unique_id_, m->client_id_, m->session_id_, m->GetData(), m->GetSize());
    if (ret != RET_OK) {
        if (m->type_ == MESSAGE_REQUEST)
            SendErrorMessage(mq_, m, ret);
        LOG(LOG_ERR, "TSComm: %s failed: %s", __FUNCTION__, ReturnValueStr[ret]);
    }

    return ret;
}

} // namespace vc
