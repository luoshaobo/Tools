/*
 * Copyright (C) 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     ssm_global.h
 *  \brief    Signal Service Manager
 *  \author   Maksym Mozok & Gustav Evertsson
 *
 *  \addtogroup signalservicemanager
 *  \{
 */

#ifndef FSM_SSM_GLOBAL_H
#define FSM_SSM_GLOBAL_H

#include "ssm_runningclients.h"

#include <string>

namespace fsm
{

namespace signalservicemanager
{

/**
 * \brief Structure holding a message
 */
typedef struct MsgPkt {
    const std::string &topic_;   ///< MQTT topic
    const std::string &payload_; ///< Payload

    /**
     * \brief Constructor populating the message
     * \param[in] topic MQTT topic
     * \param[in] payload Payload

     */
    MsgPkt(const std::string &topic, const std::string &payload)
    : topic_(topic), payload_(payload) { }
} MsgPkt_t;

typedef void ssm_onMsgArrived(MsgPkt_t* msg_pkt_response);


/**
 * \brief Full error-code space API calls can generate.
 */
enum class ErrorCode
{
    kUndefined = (-1),          ///< Uninitialized value.

    kNone,                      ///< Operation was executed with success.
    kNotImplemented,            ///< Operation is not implemented.
    kBadState,                  ///< Object is in bad state and cannot satifsy operation.
    kBadKey,                    ///< Unexpected key or key not found.
    kNotPermitted,              ///< Operation not permitted.
    kBadValue,                  ///< Unexpected or disallowed value.
    kInvalidatedResource,       ///< Resource can be operated on but should be renewed.
    kBadResource,               ///< Resource was not identified or in a bad state.
    kDuplicateKey,              ///< Key is already present.
    kOutOfBounds,               ///< Parameter is out of bounds.
    kDestinationOutOfBounds,    ///< Destination parameter is out of bounds.
    kSourceOutOfBounds,         ///< Source parameter is out of bounds.
    kTimeout,                   ///< Operation wait-time expired.
    kPersistResourceNotFound,   ///< Persistence resource not found or not accessible

    kMax                        ///< Maximum value type should hold. Keep this last.
};

} // namespace signalservicemanager
} // namespace fsm

#endif // FSM_SSM_GLOBAL_H

/** \}    end of addtogroup */
