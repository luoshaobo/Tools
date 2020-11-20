#ifndef VEHICLE_COMM_TYPES_HPP
#define VEHICLE_COMM_TYPES_HPP

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

/** @file blem_pairing_service_interface.hpp
 * This file implements a library to talk to VCanager (pairing service) over dbus
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        22-Dec-2017
 ***************************************************************************/

#include <functional>

namespace vc {

/**
    @brief VC return value type used in the VehicleComm module.
*/
typedef enum {
    RET_OK,
    RET_ERR_INVALID_ARG,
    RET_ERR_EXTERNAL,
    RET_ERR_INTERNAL,
    RET_ERR_OUT_OF_MEMORY,
    RET_ERR_TIMEOUT,
    RET_ERR_BUSY,
    RET_ERR_VALUE_NOT_AVAILABLE,
    RET_ERR_INVALID_STATE,
} ReturnValue;

const char *const ReturnValueStr[] = {
    "RET_OK",
    "RET_ERR_INVALID_ARG",
    "RET_ERR_EXTERNAL",
    "RET_ERR_INTERNAL",
    "RET_ERR_OUT_OF_MEMORY",
    "RET_ERR_TIMEOUT",
    "RET_ERR_BUSY",
    "RET_ERR_VALUE_NOT_AVAILABLE",
    "RET_ERR_INVALID_STATE"
};

/* Message-endpoints */

/**
    @brief A VC-endpoint/node identifier

    VehicleComm works by sending VC-messages between VC-endpoints (nodes).
    This enum is a list of all endpoints to which a VC-message can be sent
    or received from.
*/
typedef enum {
    ENDPOINT_UNKNOWN,
    ENDPOINT_VUC,
    ENDPOINT_LAN,
    ENDPOINT_VGM,
    ENDPOINT_IHU,
    ENDPOINT_TS,
    ENDPOINT_DS,
    ENDPOINT_MP,
} Endpoint;

const char *const EndpointStr[] = {
        "Unknown",
        "VUC",
        "LAN",
        "VGM",
        "IHU",
        "TS",
        "DS",
        "MP",
};

/**
    @brief A VC message-type identifier

    A MessageID can be one of the types listed in this enum.
*/
typedef enum {
    MESSAGE_UNKNOWN,
    MESSAGE_REQUEST,
    MESSAGE_RESPONSE,
    MESSAGE_EVENT,
    MESSAGE_ERROR,
} MessageType;

const char *const MessageTypeStr[] = {
    "MESSAGE_UNKNOWN",
    "MESSAGE_REQUEST",
    "MESSAGE_RESPONSE",
    "MESSAGE_EVENT",
    "MESSAGE_ERROR",
};

/**
    @brief A VC-message identifier

    A VC message-ID is the collective name for all the different kinds of messages.
    Each of the following types can be casted to a VC message-ID:
        RequestID
        ResponseID
        EventID
*/
typedef enum {} MessageID;

/**
    @brief The base-class of a Message

    This class is the core of a Message and contains all general data
    that is not tied to a specific message-data or message-type.
*/
class MessageBase {
private:
    static long unique_id_current;

public:
    MessageID id_ = (MessageID)0;           /**< The message-ID */
    MessageType type_ = MESSAGE_UNKNOWN;    /**< The message-type */

    long unique_id_ = -1;                       /**< Value that is unique to the message */
    int client_id_ = -1;                        /**< The identifier for a specific client */
    long session_id_ = -1;                      /**< Identifier for a request/response */
    Endpoint src_ = ENDPOINT_UNKNOWN;       /**< The source/sender endpoint for the message */
    Endpoint dst_ = ENDPOINT_UNKNOWN;       /**< The destination/receiver endpoint for the message */

    void SetUniqueID() {
        if (unique_id_ == -1)
            unique_id_ = ++unique_id_current;
    }

    MessageBase(bool copy_unique_id) {if (!copy_unique_id) SetUniqueID();};
    MessageBase() : MessageBase(false) {};
    virtual ~MessageBase() {};

    virtual unsigned char *VGetData() = 0;
    unsigned char *GetData() {
        return this->VGetData();
    }

    virtual void VSetData(const unsigned char* m) = 0;
    void SetData(const unsigned char* m) {
        this->VSetData(m);
    }
   
    virtual int VGetSize() = 0;
    int GetSize() {
        return this->VGetSize();
    }
};

/**
    @brief Template-class for Message

    This template-class is used to create a Message containing a message
    of a specific type. The base-class is used to pass messages of different
    message-types in the same manor.
*/
template<typename S>
class Message : public MessageBase {
public:
    Message() { message = {};};
    ~Message() {};

    S message;

    Message(const unsigned char* m) {
        if (m == NULL)
            message = {};
        else
            SetMessage(*(S*)m);
    }

    Message(S m) {
        SetMessage(m);
    }

    void SetMessage(S& m) {
      message = m;
    }

    unsigned char * VGetData() {
      return (unsigned char*)&message;
    }

    void VSetData(const unsigned char* m) {
      message = *(S*)m;
    }

    int VGetSize() {
      return sizeof(S);
    }
};

typedef std::function<void(MessageBase*, ReturnValue status)> ResponseCallback;
typedef std::function<void(MessageBase*)> EventCallback;

}; // namespace VC

#endif // VEHICLE_COMM_TYPES_HPP
