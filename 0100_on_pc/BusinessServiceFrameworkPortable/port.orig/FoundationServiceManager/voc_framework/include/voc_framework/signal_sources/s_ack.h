#ifndef __VOC_FRAMEWORK_S_ACK_H__
#define __VOC_FRAMEWORK_S_ACK_H__

#include "VDSasn.h"

#include <queue>

#include "voc_framework/signals/signal_pack.h"


namespace fsm
{
struct t_send_struct
{
  std::shared_ptr<asn_wrapper::VDServiceRequest> vd_service = nullptr;
  bool is_sms_retry = false;
};

class t_s_ack_signal : public fsm::Signal
{
public:
    t_s_ack_signal(int signal_type);
    virtual std::string ToString() {     return "t_s_ack_signal"; };
    
private:
    int m_signal_type = kTypeUndefined;
};

class t_s_ack_signal_pack : public fsm::SignalPack
{
public:
    t_s_ack_signal_pack(std::shared_ptr<asn_wrapper::VDServiceRequest> vd_service, int signal_type);
    
    virtual void *GetPackPayload();
    virtual void * PackGeelyAsn();

    virtual fsm::Signal *GetSignal();
    virtual int get_service_id();

private:
    std::shared_ptr<asn_wrapper::VDServiceRequest> m_vd_service;
    std::shared_ptr<t_s_ack_signal> m_signal;

    int m_service_id = -1;
};

class t_send_s_ack
{
public:
    static t_send_s_ack& instance();
public:
    t_send_s_ack();
    int init();
    int send_s_ack(t_send_struct send_struct);
    
    virtual ~t_send_s_ack();
    
    int run();
    int stop();
    int test();
        
private:
    std::queue<t_send_struct > m_sack_queue;
    bool m_running = true;
    
    std::shared_ptr<std::mutex> m_mutex;
    std::shared_ptr<std::condition_variable> m_condition;
    std::shared_ptr<std::thread> m_thread;

    bool m_init = false;

    static std::mutex m_instance_mutex;
};

}

#endif

