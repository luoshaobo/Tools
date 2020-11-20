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
 *  \file     transaction_id.cc
 *  \brief    VOC Service transaction id class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/transactions/transaction_id.h"

#include <ctime>
#include <random>
#include <string>
#include <cstring>
#include <mutex>

#include "dlt/dlt.h"

// #include "fsm_ccm.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

const std::string CCMTransactionId::kUndefinedCCMTransactionId = "\0";
uint64_t IpCommandBrokerTransactionId::counter_ = 0;
uint64_t VpomTransactionId::counter_ = 0;
uint64_t TimeoutTransactionId::counter_ = 0;
uint64_t VdServiceTransactionId::counter_ = 0;

// Class TransactionId

TransactionId::TransactionIdType TransactionId::GetType() const
{
    return my_type;
}

bool TransactionId::IsDefined () const
{
    return false;
}

bool TransactionId::operator== (const TransactionId& other) const
{
    return this->Compare(other);
}

bool TransactionId::operator!= (const TransactionId& other) const
{
    return !this->Compare(other);
}

TransactionId::TransactionId ()
{
    this->my_type = kTransactionIdTypeUndefined;
}


// Class VdServiceTransactionId

VdServiceTransactionId::VdServiceTransactionId ()
{
    my_id = VdServiceTransactionId::GetNextId();
    my_type = kTransactionIdTypeVdService;
}

VdServiceTransactionId::VdServiceTransactionId (uint64_t id)
{
    my_id = id;
    my_type = kTransactionIdTypeVdService;
}

VdServiceTransactionId::VdServiceTransactionId(const std::string transaction_id_str)
{
    my_id = std::stoull(transaction_id_str);
}

uint64_t VdServiceTransactionId::GetId()
{
    return my_id;
}

bool VdServiceTransactionId::IsDefined() const
{
    //TODO: do we actaully use/need this?
    return my_id != kUndefinedTransactionId;
}

std::shared_ptr<TransactionId> VdServiceTransactionId::GetSharedCopy() const
{
    return std::make_shared<VdServiceTransactionId>(*this);
}

bool VdServiceTransactionId::Compare (const TransactionId& other) const
{
    bool result = false;

    if (other.GetType() == kTransactionIdTypeVdService)
    {
        const VdServiceTransactionId& ihu_other =
            static_cast<const VdServiceTransactionId&>(other);
        result = this->my_id == ihu_other.my_id;
    }
    else
    {
        result = false;
    }

    return result;
}

uint64_t VdServiceTransactionId::GetNextId()
{
    return ++counter_;
}



// Class CCMTransactionId

CCMTransactionId::CCMTransactionId ()
{
    //my_id = FsmGenerateId(CCM_TRANSACTION_ID_LEN);
    my_type = kTransactionIdTypeCCM;
}

CCMTransactionId::CCMTransactionId (const std::string& id /*TODO: should be some ccm uuid type from FSM*/)
{
    my_id = id;
    my_type = kTransactionIdTypeCCM;
}

CCMTransactionId::CCMTransactionId (char* id /*TODO: should be some ccm uuid type from FSM*/)
{
    //my_id = std::string(id, CCM_TRANSACTION_ID_LEN);
    my_type = kTransactionIdTypeCCM;
}

/**
 * \brief Writes this CCMTransactionId
 *        into a c style character sequence.
 * \param[in/out] c_string The character sequence to write to.
 *                must be atleast CCM_LONG_TRANSACTION_ID_LEN
 *                bytes large.
 */
void CCMTransactionId::WriteToCString(char* c_string) const
{
    // if (c_string && my_id.c_str() && (my_id.size() >= CCM_TRANSACTION_ID_LEN))
    // {
        // std::memcpy(c_string, my_id.data(), CCM_TRANSACTION_ID_LEN);
    // }
    //else do nothing
}

bool CCMTransactionId::IsDefined() const
{
    return my_id.compare(kUndefinedCCMTransactionId) != 0;
}

std::shared_ptr<TransactionId> CCMTransactionId::GetSharedCopy() const
{
    return std::make_shared<CCMTransactionId>(*this);
}

bool CCMTransactionId::Compare (const TransactionId& other) const
{
    if (other.GetType() == kTransactionIdTypeCCM)
    {
        const CCMTransactionId& ccm_other = static_cast<const CCMTransactionId&>(other);

        return this->my_id.compare(ccm_other.my_id) == 0;
    }
    else
    {
        return false;
    }
}


// Class VehicleCommTransactionId

long  VehicleCommTransactionId::last_used_id_ = 0;  // by coincidence same value as kUndefinedVehicleCommTransactionId_
    // but not necessarily
std::mutex VehicleCommTransactionId::vc_transaction_id_mutex_;

VehicleCommTransactionId::VehicleCommTransactionId ()
{
    my_type = kTransactionIdTypeVC;
    std::lock_guard<std::mutex> lock(vc_transaction_id_mutex_);  // perhaps a bit of overkill, but better safe than sorry
    my_id_ = ++last_used_id_;
    if (my_id_ == kUndefinedVehicleCommTransactionId_)  // just in case we have an overflow and reach an invalid value ...
    {
        my_id_ = ++last_used_id_;
    }
}

VehicleCommTransactionId::VehicleCommTransactionId (long id)
{
    my_id_ = id;
    my_type = kTransactionIdTypeVC;
}

bool VehicleCommTransactionId::IsDefined() const
{
    return my_id_ != kUndefinedVehicleCommTransactionId_;
}

long VehicleCommTransactionId::GetId() const
{
    return my_id_;
}

std::shared_ptr<TransactionId> VehicleCommTransactionId::GetSharedCopy() const
{
    return std::make_shared<VehicleCommTransactionId>(*this);
}

bool VehicleCommTransactionId::Compare (const TransactionId& other) const
{
    bool result;
    if (other.GetType() == kTransactionIdTypeVC)
    {
        const VehicleCommTransactionId& vc_other = static_cast<const VehicleCommTransactionId&>(other);
        result = this->my_id_ == vc_other.my_id_;
    }
    else
    {
        result = false;
    }
    return result;
}

// Class IpCommandBrokerTransactionId

IpCommandBrokerTransactionId::IpCommandBrokerTransactionId ()
{
    my_id = IpCommandBrokerTransactionId::GetNextId();
    my_type = kTransactionIdTypeIpCommandBroker;
}

IpCommandBrokerTransactionId::IpCommandBrokerTransactionId (uint64_t id)
{
    my_id = id;
    my_type = kTransactionIdTypeIpCommandBroker;
}

IpCommandBrokerTransactionId::IpCommandBrokerTransactionId(const std::string transaction_id_str)
{
    my_id = std::stoull(transaction_id_str);
}

uint64_t IpCommandBrokerTransactionId::GetId()
{
    return my_id;
}

bool IpCommandBrokerTransactionId::IsDefined() const
{
    //TODO: do we actaully use/need this?
    return my_id != kUndefinedTransactionId;
}

std::shared_ptr<TransactionId> IpCommandBrokerTransactionId::GetSharedCopy() const
{
    return std::make_shared<IpCommandBrokerTransactionId>(*this);
}

bool IpCommandBrokerTransactionId::Compare (const TransactionId& other) const
{
    bool result = false;

    if (other.GetType() == kTransactionIdTypeIpCommandBroker)
    {
        const IpCommandBrokerTransactionId& ihu_other =
            static_cast<const IpCommandBrokerTransactionId&>(other);
        result = this->my_id == ihu_other.my_id;
    }
    else
    {
        result = false;
    }

    return result;
}

uint64_t IpCommandBrokerTransactionId::GetNextId()
{
    return ++counter_;
}


// Class VpomTransactionId

VpomTransactionId::VpomTransactionId ()
{
    my_id = VpomTransactionId::GetNextId();
    my_type = kTransactionIdTypeVpom;
}

VpomTransactionId::VpomTransactionId (uint64_t id)
{
    my_id = id;
    my_type = kTransactionIdTypeVpom;
}

VpomTransactionId::VpomTransactionId(const std::string transaction_id_str)
{
    my_id = std::stoull(transaction_id_str);
}

uint64_t VpomTransactionId::GetId()
{
    return my_id;
}

bool VpomTransactionId::IsDefined() const
{
    //TODO: do we actaully use/need this?
    return my_id != kUndefinedTransactionId;
}

std::shared_ptr<TransactionId> VpomTransactionId::GetSharedCopy() const
{
    return std::make_shared<VpomTransactionId>(*this);
}

bool VpomTransactionId::Compare (const TransactionId& other) const
{
    bool result = false;

    if (other.GetType() == kTransactionIdTypeVpom)
    {
        const VpomTransactionId& ihu_other =
            static_cast<const VpomTransactionId&>(other);
        result = this->my_id == ihu_other.my_id;
    }
    else
    {
        result = false;
    }

    return result;
}

uint64_t VpomTransactionId::GetNextId()
{
    return ++counter_;
}

// Class TimeoutTransactionId
TimeoutTransactionId::TimeoutTransactionId ():my_id(GetNextId())
{
    my_type = kTransactionIdTypeTimeout;
}

uint64_t TimeoutTransactionId::GetId()
{
    return my_id;
}

bool TimeoutTransactionId::IsDefined() const
{
    //always defined as we autoinitialise
    return true;
}

std::shared_ptr<TransactionId> TimeoutTransactionId::GetSharedCopy() const
{
    return std::make_shared<TimeoutTransactionId>(*this);
}

bool TimeoutTransactionId::Compare (const TransactionId& other) const
{
    bool result = false;

    if (other.GetType() == kTransactionIdTypeTimeout)
    {
        const TimeoutTransactionId& timeout_other =
            static_cast<const TimeoutTransactionId&>(other);
        result = this->my_id == timeout_other.my_id;
    }
    else
    {
        result = false;
    }

    return result;
}

uint64_t TimeoutTransactionId::GetNextId()
{
    return ++counter_;
}

std::atomic<uint64_t> InternalSignalTransactionId::counter_(0);

InternalSignalTransactionId::InternalSignalTransactionId()
: id_(++counter_)
{
    my_type = kTransactionIdTypeInternalSignal;
}

uint64_t InternalSignalTransactionId::GetId() const
{
    return id_;
}

std::shared_ptr<TransactionId> InternalSignalTransactionId::GetSharedCopy() const
{
    return std::make_shared<InternalSignalTransactionId>(*this);
}

bool InternalSignalTransactionId::Compare(const TransactionId& other) const
{
    bool result = false;

    if (other.GetType() == kTransactionIdTypeInternalSignal)
    {
        const InternalSignalTransactionId& internal_signal_other =
            static_cast<const InternalSignalTransactionId&>(other);
        result = this->id_ == internal_signal_other.id_;
    }
    else
    {
        result = false;
    }

    return result;
}


} // namespace fsm

/** \}    end of addtogroup */
