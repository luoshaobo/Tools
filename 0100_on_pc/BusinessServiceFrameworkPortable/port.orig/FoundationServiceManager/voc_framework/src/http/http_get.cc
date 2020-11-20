/**
 * Copyright (C) 2017-2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     default_cache.cpp
 *  \brief    Transfer Manager Default Cache fallback implementation.
 *  \author   Florian Schindler
 *
 *  \addtogroup transfermanager
 *  \{
 */

// ** INCLUDES *****************************************************************
#include "http_get.h"

#include <iostream>

#include "comalog.h"
#include "icommunicatorproxy.h"
#include "icommunicationmanagerproxyevents.h"
#include "icommunicationmanagerproxy.h"
#include "thread.h"
#include "dlt/dlt.h"
#include "comalog.h"

//using namespace fsm;
//#define  DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, ...) printf(__VA_ARGS__)

namespace fsm
{

  t_http_get::t_http_get(t_callback_fun fun)
  {
    COMALOG(COMALOG_DEBUG, "%s(), constructed \n", __FUNCTION__);
    m_mutex = std::make_shared<std::recursive_mutex>();

    m_init_coma_mutex = std::make_shared<std::mutex>();
    m_init_coma_condition =  std::make_shared<std::condition_variable>();

    m_callback = fun;
  }

  int t_http_get::get(const std::string &url, const std::vector<std::string> &http_headers)
  {
    COMALOG(COMALOG_DEBUG, "---------%s(), url=%s \n", 
        __FUNCTION__, url.c_str());
    size_t i = 0;
    for(i = 0; i < http_headers.size(); ++i)
    {
      COMALOG(COMALOG_DEBUG, "---------%s(), http_headers=%s, i= %d \n", 
          __FUNCTION__, http_headers[i].c_str(), i);
    }
    int ret = http_request(url, http_headers);
    return ret;
  }

  int t_http_get::http_request(const std::string &url, const std::vector<std::string> &http_headers)
  {
    RequestStatus requestStatus;

    ICommunicatorData *pICommunicatorData = m_ICommunicator->getICommunicatorData();

    pICommunicatorData->httpsGet(requestStatus, url.c_str(), http_headers,
      std::bind(&t_http_get::http_request_callback, this, _1));

    return 0;
  }

  void t_http_get::http_request_callback(ResponseCommunicatorDataHttpsGet &callback)
  {
    COMALOG(COMALOG_DEBUG, "---------%s(), ComaClient http_request_callback() requestId:%u %s\n", 
        __FUNCTION__,
        callback.communicatorrequestidentifier().requestid()
        , Status_Name(callback.status()).c_str());
    COMALOG(COMALOG_DEBUG, "---------%s(), ComaClient http_request_callback() curlcode:%d string:%s\n", 
        __FUNCTION__,
        callback.curlcode(), callback.curlstring().c_str());
    COMALOG(COMALOG_DEBUG, "---------%s(),Status:%d StatusName:%s\n", 
        __FUNCTION__,
        callback.status(), Status_Name(callback.status()).c_str());
    COMALOG(COMALOG_DEBUG, "---------%s(),OcspStatus:%d OcspStatusName:%s\n", 
        __FUNCTION__,
        callback.ocspstatus(), OcspStatus_Name(callback.ocspstatus()).c_str());
  
    COMALOG(COMALOG_DEBUG, "---------%s(), HttpStatus: %d\n", 
        __FUNCTION__,
        callback.httpcode());
    COMALOG(COMALOG_DEBUG, "---------%s(), Header:\n",
         __FUNCTION__
       );
    for (auto i = 0; i < callback.header_size(); i++)
    {
      COMALOG(COMALOG_DEBUG, "---------%s(), response header=%s\n", 
         __FUNCTION__,
        callback.header().Get(i).c_str());
      
    }
    COMALOG(COMALOG_DEBUG, "---------%s(), response body=%s\n", 
         __FUNCTION__,
        callback.payload().c_str());
#if 0
    for (i = 0; i < callback.payload().length(); i++)
    {
      COMALOG(COMALOG_DEBUG, "---------body[i]=%d, i=%d\n", callback.payload()[i], i);
    }
#endif
    COMALOG(COMALOG_DEBUG, "---------%s(), call callback\n", 
         __FUNCTION__);
    m_callback(callback);
  }

  int t_http_get::get_call_back(const t_http_response &response)
  {
    COMALOG(COMALOG_DEBUG, "---------%s(), response.http_code=%d \n", 
        __FUNCTION__, response.http_code);
    size_t i = 0; 
    for(i = 0; i < response.http_headers.size(); ++i)
    {
      COMALOG(COMALOG_DEBUG, "---------%s(), response.http_headers=%s \n", 
          __FUNCTION__, response.http_headers[i].c_str());
    }
    COMALOG(COMALOG_DEBUG, "---------%s(), response.http_body=%s \n", 
        __FUNCTION__, response.http_body.c_str());
    return 0;
  }

  int t_http_get::init()
  {
    {
      std::unique_lock<std::recursive_mutex> lk(*m_mutex);
      if(m_is_processing)
      {
        COMALOG(COMALOG_ERROR, "---------%s(), erro , m_is_processing is true \n", 
            __FUNCTION__);
        return false;
      }
      m_is_processing = true;
      
      if(m_is_init)
      {
        COMALOG(COMALOG_ERROR, "---------%s(), warning , already init \n", 
            __FUNCTION__);
        return true;
      }
    }

    std::shared_ptr<bool> auto_reset_processing(&m_is_processing, [](bool *p){
                  if(*p)
                  {
                    *p = false;
                  }
              }
          );

    int ret = 0;
    ret = init_coma_client();
    if(ret != 0)
    {
      COMALOG(COMALOG_ERROR, "---------%s(), error , init_coma_client \n", 
          __FUNCTION__);
    }
    ret = get_communicator();
    if(ret != 0)
    {
      COMALOG(COMALOG_ERROR, "---------%s(), error , get_communicator \n", 
          __FUNCTION__);
    }
    m_is_init = true;
    return 0;
  }

  int t_http_get::get_communicator()
  {
    COMALOG(COMALOG_DEBUG, "---------%s(), begin get communicator \n", 
                __FUNCTION__);

    ResourceDescriptor rd;

    std::function<void (ICommunicatorProxy::EventCommunicatorActivationChanged&)>
            callback = std::bind(&t_http_get::get_communicator_callback, this, std::placeholders::_1);
    std::unique_lock<std::mutex> lk(*m_init_coma_mutex, std::defer_lock_t());
    m_comaProxy->getCommunicator(rd, 
                                m_ICommunicator, 
                                callback);
    
    t_http_get* cap_this = this;
    int ret = (*m_init_coma_condition).wait_for(lk,
                             std::chrono::milliseconds(10000),
                             [cap_this]{return cap_this->m_get_communicator_success ;});
    COMALOG(COMALOG_DEBUG, "---------%s(), ret = %d \n", 
                __FUNCTION__, ret);

    return 0;
  }

  void t_http_get::get_communicator_callback(ICommunicatorProxy::EventCommunicatorActivationChanged& event)
  {
    COMALOG(COMALOG_DEBUG, "---------%s(), called \n", 
            __FUNCTION__);
    std::lock_guard<std::mutex> lk(*m_init_coma_mutex);
    if (event.activated == true)
    {
      m_get_communicator_callback = true;
      m_get_communicator_success = true;
    }
    else
    {
      m_get_communicator_callback = false;
      m_get_communicator_success = false;
      COMALOG(COMALOG_ERROR, "---------%s(), error, event.activated is not true \n", 
          __FUNCTION__);
    }
    (*m_init_coma_condition).notify_all(); 
  }

  int t_http_get::init_coma_client()
  {
    std::string sSharedLibName = "libcomaclient.so";
    COMALOG(COMALOG_DEBUG, "---------%s(), begin load: %s \n", 
        __FUNCTION__, sSharedLibName.c_str());
    m_comaProxy = ::createCommunicationManagerProxy("TestClientComaProxy", sSharedLibName);
    COMALOG(COMALOG_DEBUG, "---------%s(), end load: %s \n", 
        __FUNCTION__, sSharedLibName.c_str());

    bool bInit = false;
    RequestStatus status;

    //std::unique_lock<std::mutex> lk(*m_init_coma_mutex, std::defer_lock_t());
    std::unique_lock<std::mutex> lk(*m_init_coma_mutex);
    bInit = m_comaProxy->init(status,
            std::bind(&t_http_get::init_coma_client_callback, this, _1));

    t_http_get* cap_this = this;
    bool timeout = (*m_init_coma_condition).wait_for(lk,
                             std::chrono::milliseconds(10000),
                             [&]{return m_init_coma_client_success == true;});
    COMALOG(COMALOG_DEBUG, "---------%s(), timeout = %d \n", 
          __FUNCTION__, timeout);
    if(m_init_coma_client_success != true)
    {
      COMALOG(COMALOG_ERROR, "---------%s(), error, init coma client failed, m_init_coma_client_success= %d \n", 
          __FUNCTION__, m_init_coma_client_success);
      return -1;
    }
  
    if (bInit == false)
    {
      COMALOG(COMALOG_ERROR, "---------%s(), error, bInit is false \n", 
          __FUNCTION__);
      return -1;
    }
    if (m_comaProxy->getInitStatus() != ICommunicationManagerProxy::INITIALISATIONSTATUS_SUCCESS)
    {
      COMALOG(COMALOG_ERROR, "---------%s(), m_comaProxy->getInitStatus() = %d \n", 
          __FUNCTION__, m_comaProxy->getInitStatus());
      return -1;
    }
    return 0;
  }

  void t_http_get::init_coma_client_callback(ResponseComaInitClient& response)
  {
    COMALOG(COMALOG_DEBUG, "---------%s(), called \n", 
            __FUNCTION__);
    std::lock_guard<std::mutex> lk(*m_init_coma_mutex);
    if (response.status() == ::com::contiautomotive::communicationmanager::messaging::internal::STATUS_SUCCESS)
    {
        m_init_coma_client_callback = true;
        m_init_coma_client_success = true;
        COMALOG(COMALOG_DEBUG, "---------%s(), success, response.status() = %d, %d , m_init_coma_client_success= %d\n", 
            __FUNCTION__, response.status(), 
            ::com::contiautomotive::communicationmanager::messaging::internal::STATUS_SUCCESS,
            m_init_coma_client_success);
    }
    else
    {
        m_init_coma_client_callback = false;
        m_init_coma_client_success = false;
        
        COMALOG(COMALOG_ERROR, "---------%s(), error, response.status() = %d, m_init_coma_client_success=%d \n", 
            __FUNCTION__, response.status(), m_init_coma_client_success);
    }

    (*m_init_coma_condition).notify_all(); // Coma client-side instance is initialized.
}

} // namespace fsm

/** \}    end of addtogroup */
