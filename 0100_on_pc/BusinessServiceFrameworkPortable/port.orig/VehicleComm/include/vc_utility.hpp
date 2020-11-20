#ifndef VC_UTILITY_HPP
#define VC_UTILITY_HPP

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

/** @file vc_utility.hpp
 * This file implements general functions used by several parts of the VehicleComm module.
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        07-Dec-2016
 ***************************************************************************/

#include <cstring>
#include "vc_common.hpp"
#include "vc_message_queue.hpp"

#define UNUSED(x) (void)(x)
#define LENGTH(array) (int)(sizeof((array))/sizeof((array[0])))
#define LENGTH2(array, i) (int)(sizeof((array[i]))/sizeof((array[i][0])))
#define CREATE_DATA(x) reinterpret_cast<const unsigned char*>(&x)
#define STRNCPY(dst,src,max_sz) std::strncpy(dst,src,max_sz-1); dst[max_sz-1]='\0'

namespace vc {

RequestID RequestLookup(ResponseID response_id);
ResponseID ResponseLookup(RequestID request_id);
const char *MessageStr(MessageID id, MessageType type);
const char *MessageStr(MessageBase *m);
RequestID RequestNameLookup(const std::string& req_name);
ResponseID ResponseNameLookup(const std::string& res_name);
EventID EventNameLookup(const std::string& ev_name);
bool MessageNameLookup(const std::string& msg_name, MessageType& type, MessageID& id);

MessageBase *InitMessage(MessageID id, MessageType type, int client_id, long session_id, Endpoint src, Endpoint dst, const unsigned char *data);
MessageBase *InitMessage(MessageID id, MessageType type, int client_id, long session_id, Endpoint src, const unsigned char *data);
MessageBase *CopyMessage(MessageBase * msg);
ReturnValue PrintMessage(MessageBase *m);
void SendErrorMessage(MessageQueue *mq, MessageBase *m_req, ReturnValue error);
ReturnValue SendResponseMessage(MessageQueue *mq, MessageBase *m_req, const unsigned char *data);
bool HasGenericResponse(MessageBase *m);


} // namespace vc

#endif // VC_UTILITY_HPP
