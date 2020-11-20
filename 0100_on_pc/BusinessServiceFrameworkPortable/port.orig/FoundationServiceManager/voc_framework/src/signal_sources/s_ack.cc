/**
 * Copyright (C) 2018 Continental AG and subsidiaries
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     vocmo_signal_source.cc
 *  \brief    Vocmo signal source.
 *  \author   Dariusz Szymczak
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include <algorithm>
#include <mutex>
#include <functional>
#include <thread>
#include <chrono>
#include <thread>
#include <sstream>

#include "voc_framework/signal_sources/vocmo_signal_source.h"

#include "dlt/dlt.h"
#include "voc_framework/signals/signal_factory.h"

#include "inadifcontroller.h"

#include "timestamp.h"



DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

t_s_ack_signal::t_s_ack_signal(int signal_type)
    : Signal (fsm::VdServiceTransactionId(), (fsm::Signal::SignalType)signal_type)
{
  m_signal_type = signal_type;
}

t_s_ack_signal_pack::t_s_ack_signal_pack(std::shared_ptr<asn_wrapper::VDServiceRequest> vd_service, int signal_type)
{
  m_vd_service = vd_service;
  m_signal = std::make_shared<t_s_ack_signal>(signal_type);

  m_service_id = signal_type;
}

void *t_s_ack_signal_pack::GetPackPayload()
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                            "%s()  called",
                            __FUNCTION__);
    return NULL;
}

void * t_s_ack_signal_pack::PackGeelyAsn()
{ 
  DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                          "t_s_ack_signal_pack::%s()  called",
                          __FUNCTION__);
  return (void *)m_vd_service.get();
}

fsm::Signal *t_s_ack_signal_pack::GetSignal()
{
  return m_signal.get();
}

int t_s_ack_signal_pack::get_service_id()
{
  DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                          "t_send_s_ack::%s()  called begin",
                          __FUNCTION__);
  return m_service_id;
}

std::mutex t_send_s_ack::m_instance_mutex;
t_send_s_ack& t_send_s_ack::instance()
{
    std::unique_lock<std::mutex> lk(m_instance_mutex);
    static t_send_s_ack instance;
    if(! instance.m_init)
    {
        instance.init();
        instance.m_init = true;
    }
    return instance;
}

t_send_s_ack::t_send_s_ack()
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,
                            "t_send_s_ack::%s()  called begin",
                            __FUNCTION__);
    m_mutex = std::make_shared<std::mutex>();
    m_condition = std::make_shared<std::condition_variable>();
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,
                            "t_send_s_ack::%s()  called end",
                            __FUNCTION__);
}

t_send_s_ack::~t_send_s_ack()
{
    stop();
}

int t_send_s_ack::init()
{
    //std::unique_lock<std::mutex> lk(*m_mutex);
    std::function<int()> f1 = bind(&t_send_s_ack::run, this);
    m_thread = std::make_shared<std::thread>(f1);
    //std::thread t1(f1);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,
                            "t_send_s_ack::%s()  called end",
                            __FUNCTION__);
    return 0;
}

int t_send_s_ack::send_s_ack(t_send_struct send_struct)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                            "t_send_s_ack::%s()  called",
                            __FUNCTION__);
    auto vd_service = send_struct.vd_service;
                        
    asn_wrapper::RequestHeader *header = vd_service->header;
    if(header == NULL)
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                              "t_send_s_ack::%s()  header is null",
                              __FUNCTION__);
      return 1;
    }
    
    if (header->optional.getPresence(asn_wrapper::asn_ackRequired) != asnTRUE)
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                              "t_send_s_ack::%s()  asn_ackRequired is not present, not send s_ack",
                              __FUNCTION__);
      return 1;
    }
    if (header->ackRequired == false)
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                              "t_send_s_ack::%s()  ackRequired is false, not send s_ack",
                              __FUNCTION__);
      return 1;
    }
    
    if (header->optional.getPresence(asn_wrapper::asn_requestType) != asnTRUE)
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                              "t_send_s_ack::%s()  asn_requestType is not present, not send s_ack",
                              __FUNCTION__);
      return 1;
    }
    if (header->requestType != asn_wrapper::serviceRequest)
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                              "t_send_s_ack::%s()  requestType is not serviceRequest, not send s_ack",
                              __FUNCTION__);
      return 1;
    }
    
    {
      std::unique_lock<std::mutex> lk((*m_mutex));
      m_sack_queue.push(send_struct);
      (*m_condition).notify_all(); 
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                              "t_send_s_ack::%s()  notify signal",
                              __FUNCTION__);
    }
    
    return 0;
}


int t_send_s_ack::run()
{
  DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,
                          "t_send_s_ack::%s()  called begin",
                          __FUNCTION__);
  while(m_running)
  {
    t_send_struct send_struct;
    std::shared_ptr<asn_wrapper::VDServiceRequest> s_ack;
    {
      std::unique_lock<std::mutex> lk((*m_mutex));
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                              "t_send_s_ack::%s()  obtain lock",
                              __FUNCTION__);
      if (m_sack_queue.size() == 0)
      {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                                "t_send_s_ack::%s()  m_sack_queue is empty, wait for signal",
                                __FUNCTION__);
        t_send_s_ack * capture_this = this;
        bool wait_status = m_condition->wait_for(lk,
                                        std::chrono::seconds(60),
                                        [capture_this]{ 
                                            return capture_this->m_sack_queue.size()>0; 
                                        });
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                                "t_send_s_ack::%s() wait for signal to end, wait_status=%d",
                                __FUNCTION__, wait_status);
      }
      if (m_sack_queue.size() == 0)
      {
          DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                                  "t_send_s_ack::%s() m_sack_queue.size() == 0, continue",
                                  __FUNCTION__);
          continue;
      }
      send_struct = m_sack_queue.front();
      m_sack_queue.pop();
    }
    s_ack = send_struct.vd_service;
    if (s_ack == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                                "t_send_s_ack::%s()  s_ack is null, ignore.",
                                __FUNCTION__);
        continue;
    }
    if ( !s_ack->optional.getPresence(asn_wrapper::asn_body) )
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                                "t_send_s_ack::%s()  asn_body is not presence, ignore.",
                                __FUNCTION__);
        continue;
    }
    if ( !s_ack->body->optional.getPresence(asn_wrapper::asn_serviceId) )
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                                "t_send_s_ack::%s()  asn_serviceId is not presence, ignore.",
                                __FUNCTION__);
        continue;
    }
    
    auto response = std::make_shared<asn_wrapper::VDServiceRequest>();
    response->header = new asn_wrapper::RequestHeader();
    
    response->header->requestid = s_ack->header->requestid;
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG
                            , "t_send_s_ack::%s()  requestid:%d"
                            , __FUNCTION__
                            , response->header->requestid);
    
    Timestamp now;
    response->header->timeStamp = new asn_wrapper::TimeStamp();
    response->header->timeStamp->seconds = (now.epochMicroseconds()/1000000);
    response->header->timeStamp->optional.setPresence(asn_milliseconds);
    response->header->timeStamp->milliseconds= (now.epochMicroseconds()%1000000)/1000;
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG
                            , "t_send_s_ack::%s()  timeStamp->seconds:%d"
                            , __FUNCTION__
                            , response->header->timeStamp->seconds);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG
                            , "t_send_s_ack::%s()  timeStamp->milliseconds:%d"
                            , __FUNCTION__
                            , response->header->timeStamp->milliseconds);

    response->header->eventId = new asn_wrapper::TimeStamp();
    response->header->eventId->seconds = s_ack->header->eventId->seconds;
    response->header->eventId->optional.setPresence(asn_milliseconds);
    response->header->eventId->milliseconds= s_ack->header->eventId->milliseconds;
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG
                            , "t_send_s_ack::%s()  eventId->seconds:%d"
                            , __FUNCTION__
                            , response->header->eventId->seconds);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG
                            , "t_send_s_ack::%s()  eventId->milliseconds:%d"
                            , __FUNCTION__
                            , response->header->eventId->milliseconds);

    response->header->optional.setPresence(asn_requestType);
    response->header->requestType = asn_wrapper::ack;
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG
                            , "t_send_s_ack::%s()  requestType:%d"
                            , __FUNCTION__
                            , response->header->requestType);

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG
                            , "t_send_s_ack::%s()  serviceId:%d"
                            , __FUNCTION__
                            , s_ack->body->serviceId);
    
//    response->optional.setPresence(asn_wrapper::asn_body);
//    response->body = new asn_wrapper::RequestBody();
    
//    response->body->optional.setPresence(asn_wrapper::asn_serviceId);
//    response->body->serviceId = s_ack->body->serviceId;
    auto tmp_signal = std::make_shared<t_s_ack_signal_pack>(response, s_ack->body->serviceId);
    VocmoSignalSource vocmo_signal_source;
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                            "t_send_s_ack::%s()  begin send s_ack, is_sms_retry = %d.",
                            __FUNCTION__
                            , send_struct.is_sms_retry);

    int ret = vocmo_signal_source.GeelySendMessage(tmp_signal, send_struct.is_sms_retry);
    if (ret == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                                "t_send_s_ack::%s()  send s_ack failed",
                                __FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                                "t_send_s_ack::%s()  send s_ack success",
                                __FUNCTION__);
    }
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,
                            "t_send_s_ack::%s() one loop end",
                            __FUNCTION__);
  }
  return 0;
}

int t_send_s_ack::stop()
{
  {
    std::unique_lock<std::mutex> lk((*m_mutex));
    //std::shared_ptr<asn_wrapper::VDServiceRequest> s_ack = nullptr;
    m_sack_queue.push(t_send_struct());
    m_running = false;
    (*m_condition).notify_all(); 
  }
  m_thread->join();
  return 0;
}

int t_send_s_ack::test()
{
  DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,
                          "t_send_s_ack::%s()",
                          __FUNCTION__);

  return 0;
}


}// namespace fsm

/** \}    end of addtogroup */
