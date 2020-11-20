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
 *
 *  \file     default_cache.h
 *  \brief    Transfer Manager Default Cache fallback interface.
 *  \author   Florian Schindler
 *
 *  \addtogroup transfermanager
 *  \{
 */

#ifndef __HTTPS_GET_H__
#define __HTTPS_GET_H__

/// ** INCLUDES *****************************************************************

#include <string>
#include <memory>
#include <mutex>
#include <vector>
#include <condition_variable>


#include "icommunicatorproxy.h"
#include "icommunicationmanagerproxyevents.h"
class ICommunicationManagerProxy;
class CommunicationManagerProxy;

namespace fsm
{
  struct t_http_response
  {
    int http_code = -1;
    std::vector<std::string> http_headers;
    std::string http_body;
  };
  class t_http_get
  {
  public:
    typedef std::function<void (ResponseCommunicatorDataHttpsGet &callback)> t_callback_fun;
    t_http_get(t_callback_fun fun);
    int get(const std::string &url, const std::vector<std::string> &http_headers);
    int http_request(const std::string &url, const std::vector<std::string> &http_headers);
    int get_call_back(const t_http_response &response);

    void init_coma_client_callback(ResponseComaInitClient& response);
    void http_request_callback(ResponseCommunicatorDataHttpsGet &callback);

  public:
    int init();
    int init_coma_client();
    bool is_init();
    bool is_processing();
    int get_communicator();
    void get_communicator_callback(ICommunicatorProxy::EventCommunicatorActivationChanged& event);

  private:
    bool m_is_init = false;
    bool m_is_processing = false;
    std::shared_ptr<std::recursive_mutex> m_mutex;
    ICommunicationManagerProxy* m_comaProxy = 0;
    ICommunicatorProxy *m_ICommunicator = 0;

    bool m_init_coma_client_callback = false;
    bool m_init_coma_client_success = false;

    std::shared_ptr<std::mutex> m_init_coma_mutex;
    std::shared_ptr<std::condition_variable> m_init_coma_condition;  

    bool m_get_communicator_callback = false;
    bool m_get_communicator_success = false;

    t_callback_fun m_callback;
    
  };

} // namespace fsm

#endif // #ifndef FSM_TRANSFERMANAGER_DEFAULT_CACHE_H_

/** \}    end of addtogroup */

