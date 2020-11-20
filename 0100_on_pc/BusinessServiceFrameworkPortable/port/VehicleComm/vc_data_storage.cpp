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
*/

/** @file vc_data_storage.cpp
 * This file handles persistant data storage for VehicleComm
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        25-Jan-2017
 ***************************************************************************/

#include <cstring>

#include "vehicle_comm_messages.hpp"
#include "vc_utility.hpp"

#include "vc_data_storage.hpp"

namespace vc {

MessageQueue *DataStorage::mq_;

// Private
/**
    @brief Add a new collection in Datastorage

    @param[in]  name            Collection name
    @return     A pointer to a ParameterCollection
*/
ParameterCollection *DataStorage::AddCollection(const std::string& name)
{
    LOG(LOG_INFO, "DataStorage: %s.", __FUNCTION__);

    ParameterCollection *pc = new ParameterCollection(name);
    pcl_.push_back(pc);

    return pc;
}

/**
    @brief Find a collection in Datastorage

    @param[in]  name            Collection name
    @return     A pointer to a ParameterCollection
*/
ParameterCollection *DataStorage::FindCollection(const std::string &name)
{
    LOG(LOG_INFO, "DataStorage: %s.", __FUNCTION__);

    for (std::list<ParameterCollection*>::iterator it = pcl_.begin(); it != pcl_.end(); ++it) {
        if ((*it)->name_ == name)
            return *it;
    }
    
    return NULL;
}

/**
    @brief Send an event triggered by an updated parameter with string value

    @param[in]  coll_name          Collection name
    @param[in]  par_name           Parameter name
    @param[in]  par_val            Parameter value
    @return     A VC return value
*/
ReturnValue DataStorage::SendEventMessage(const std::string &coll_name, const std::string &par_name,
                                            const std::string par_val)
{
    LOG(LOG_INFO, "DataStorage: %s.", __FUNCTION__);

    EventDSParameterStr par;
    STRNCPY(par.collection, coll_name.c_str(), sizeof(par.collection));
    STRNCPY(par.name, par_name.c_str(), sizeof(par.name));
    STRNCPY(par.value, par_val.c_str(), sizeof(par.value));

    MessageBase *m;

    // Send event to MP
    m = InitMessage((MessageID)EVENT_DSPARAMETER_STR, MESSAGE_EVENT, -1, -1, ENDPOINT_DS,
                                ENDPOINT_MP, CREATE_DATA(par));
    if (m == NULL) {
        LOG(LOG_WARN, "DataStorage: %s InitMessage returned NULL not adding to queue.", __FUNCTION__);
        return RET_ERR_INTERNAL;
    }
    (void)mq_->AddMessage(m);

    return RET_OK;
}

/**
    @brief Send an event triggered by an updated parameter with vector values

    @param[in]  coll_name          Collection name
    @param[in]  par_name           Parameter name
    @param[in]  par_val            Parameter value
    @return     A VC return value
*/
ReturnValue DataStorage::SendEventMessage(const std::string &coll_name, const std::string &par_name,
                                            const std::vector<unsigned char> par_val)
{
    LOG(LOG_INFO, "DataStorage: %s.", __FUNCTION__);

    EventDSParameterVect par;
    
    STRNCPY(par.collection, coll_name.c_str(), sizeof(par.collection));
    STRNCPY(par.name, par_name.c_str(), sizeof(par.name));
    par.size = par_val.size();
    memcpy(par.value, (void*)&par_val[0], par_val.size());

    MessageBase *m;

    // Send event to MP
    m = InitMessage((MessageID)EVENT_DSPARAMETER_VECT, MESSAGE_EVENT, -1, -1, ENDPOINT_DS,
                                ENDPOINT_MP, CREATE_DATA(par));
    if (m == NULL) {
        LOG(LOG_WARN, "DataStorage: %s InitMessage returned NULL not adding to queue.", __FUNCTION__);
        return RET_ERR_INTERNAL;
    }
    (void)mq_->AddMessage(m);

    return RET_OK;
}

/**
    @brief Send an event triggered by an updated parameter with int value

    @param[in]  coll_name          Collection name
    @param[in]  par_name           Parameter name
    @param[in]  par_val            Parameter value int
    @return     A VC return value
*/
ReturnValue DataStorage::SendEventMessage(const std::string &coll_name, const std::string &par_name, int par_val)
{
    LOG(LOG_INFO, "DataStorage: %s.", __FUNCTION__);

    EventDSParameterInt par;
    STRNCPY(par.collection, coll_name.c_str(), sizeof(par.collection));
    STRNCPY(par.name, par_name.c_str(), sizeof(par.name));
    par.value = par_val;
    LOG(LOG_DEBUG, "DataStorage: %s collection: %s  name: %s  value: %d.", __FUNCTION__, par.collection, par.name, par.value);

    MessageBase *m;

    // Send event to MP
    m = InitMessage((MessageID)EVENT_DSPARAMETER_INT, MESSAGE_EVENT, -1, -1, ENDPOINT_DS,
                        ENDPOINT_MP, CREATE_DATA(par));
    if (m == NULL) {
        LOG(LOG_WARN, "DataStorage: %s InitMessage returned NULL not adding to queue.", __FUNCTION__);
        return RET_ERR_INTERNAL;
    }
    (void)mq_->AddMessage(m);

    return RET_OK;
}


/**
    @brief Print all parameters
*/
void DataStorage::PrintParameters()
{
    for (std::list<ParameterCollection*>::iterator it = pcl_.begin(); it != pcl_.end(); ++it)
        (*it)->PrintParameters();
}

/**
    @brief Copy a string parameter into a VCResponse message

    @param[in]  msg                Message containing request parameter
    @param[in]  par                String parameter top copy from
    @return     A VC return value
*/
ReturnValue DataStorage::HandleGetDSParameter_Str(Message<ReqGetDSParameter> *msg, Parameter<std::string>* par)
{
    MessageBase *m_res;
    ReqGetDSParameter *req = &msg->message;
    if (par->value_.length() >= N_STR_MAX) {
        LOG(LOG_WARN, "DataStorage: %s: Parameter-value to long!", __FUNCTION__);
        return RET_ERR_INTERNAL;
    }

    ResGetDSParameterStr res_str;
    STRNCPY(res_str.collection, req->collection, sizeof(res_str.collection));
    STRNCPY(res_str.name, par->name_.c_str(), sizeof(res_str.name));
    STRNCPY(res_str.value, par->value_.c_str(), sizeof(res_str.value));
    m_res = InitMessage((MessageID)RES_GETDSPARAMETER_STR, MESSAGE_RESPONSE, msg->client_id_,
                        msg->session_id_, ENDPOINT_DS, ENDPOINT_TS, CREATE_DATA(res_str));
    if (m_res == NULL) {
        LOG(LOG_WARN, "DataStorage: %s: InitMessage failed for RES_GETDSPARAMETER_STR!", __FUNCTION__);
        return RET_ERR_INTERNAL;
    }

    mq_->AddMessage(m_res);
    return RET_OK;
}

/**
    @brief Copy a int parameter into a VCResponse message

    @param[in]  msg                Message containing request parameter
    @param[in]  par                Int parameter to copy from
    @return     A VC return value
*/
ReturnValue DataStorage::HandleGetDSParameter_Int(Message<ReqGetDSParameter> *msg, Parameter<int>* par)
{
    MessageBase *m_res;
    ReqGetDSParameter *req = &msg->message;
    ResGetDSParameterInt res_int;

    STRNCPY(res_int.collection, req->collection, sizeof(res_int.collection));
    STRNCPY(res_int.name, par->name_.c_str(), sizeof(res_int.name));
    res_int.value =  par->value_;
    m_res = InitMessage((MessageID)RES_GETDSPARAMETER_INT, MESSAGE_RESPONSE, msg->client_id_,
                        msg->session_id_, ENDPOINT_DS, ENDPOINT_TS, CREATE_DATA(res_int));
    if (m_res == NULL) {
        LOG(LOG_WARN, "DataStorage: %s: InitMessage failed for RES_GETDSPARAMETER_INT!", __FUNCTION__);
        return RET_ERR_INTERNAL;
    }

    mq_->AddMessage(m_res);
    return RET_OK;
}

/**
    @brief Copy a vector parameter into a VCResponse message

    @param[in]  msg                Message containing request parameter
    @param[in]  par                Vector parameter to copy from
    @return     A VC return value
*/
ReturnValue DataStorage::HandleGetDSParameter_CharArr(Message<ReqGetDSParameter> *msg, Parameter<std::vector<unsigned char>>* par)
{
    MessageBase *m_res;
    ReqGetDSParameter *req = &msg->message;
    ResGetDSParameterVect res_vect;

    STRNCPY(res_vect.collection, req->collection, sizeof(res_vect.collection));
    STRNCPY(res_vect.name, par->name_.c_str(), sizeof(res_vect.name));
    if (par->value_.size() > LENGTH(res_vect.value)) {
        LOG(LOG_WARN, "DataStorage: %s: vector size to big to fit in vc message", __FUNCTION__);
        return RET_ERR_INTERNAL;
    }
    res_vect.size =  par->value_.size();
    memcpy(res_vect.value, (void*)&par->value_[0], par->value_.size());
    m_res = InitMessage((MessageID)RES_GETDSPARAMETER_VECT, MESSAGE_RESPONSE, msg->client_id_,
                        msg->session_id_, ENDPOINT_DS, ENDPOINT_TS, CREATE_DATA(res_vect));
    if (m_res == NULL) {
        LOG(LOG_WARN, "DataStorage: %s: InitMessage failed for RES_GETDSPARAMETER_INT!", __FUNCTION__);
        return RET_ERR_INTERNAL;
    }

    mq_->AddMessage(m_res);
    return RET_OK;
}

/**
    @brief Handle incoming request for parameter

    @param[in]  msg                Message containing request parameter
    @return     A VC return value
*/
ReturnValue DataStorage::HandleGetDSParameter(Message<ReqGetDSParameter> *msg)
{
    ReqGetDSParameter *req = &msg->message;
    ParameterBase *par = FindParameter(std::string(req->collection), std::string(req->name));
    if (!par) {
        LOG(LOG_WARN, "DataStorage: %s: Parameter %s (collection = %s) not found in DataStorage!",
                __FUNCTION__, &req->name, &req->collection);
        return RET_ERR_VALUE_NOT_AVAILABLE;
    }
    switch (par->type_) {
    case VCPARAM_TYPE_STRING: {
        return HandleGetDSParameter_Str(msg, (Parameter<std::string>*)par);
        break;
    }
    case VCPARAM_TYPE_INT: {
        return HandleGetDSParameter_Int(msg, (Parameter<int>*)par);
        break;
    }
    case VCPARAM_TYPE_CHARARRAY: {
        return HandleGetDSParameter_CharArr(msg, (Parameter<std::vector<unsigned char>>*)par);
        break;
    }
    default:
        LOG(LOG_WARN, "DataStorage: %s: Unknown parameter-type (type = %d)!", __FUNCTION__, (int)par->type_);
        return RET_ERR_INTERNAL;
        break;
    }
    return RET_OK;
}


// Public

/**
    @brief Constructor for the DataStorage-class.

    Constructor for Datastorage initiated with vehiclecomms message queue.
    @param[in]  mq      VehicleComms message queue.
*/
DataStorage::DataStorage(MessageQueue *mq)
{
    LOG(LOG_INFO, "DataStorage: %s.", __FUNCTION__);
    mq_ = mq;
}

/**
    @brief Destructor for the DataStorage-class.
*/
DataStorage::~DataStorage()
{
    LOG(LOG_INFO, "DataStorage: %s.", __FUNCTION__);
}

/**
    @brief Lock when writing or reading value that has dependencies
*/
void DataStorage::StartReadWriteDepValues()
{
    LOG(LOG_DEBUG, "DataStorage: %s.", __FUNCTION__);
    dep_mtx_.lock();
}

/**
    @brief UnLock when stop writing or reading value that has dependencies
*/
void DataStorage::StopReadWriteDepValues()
{
    LOG(LOG_DEBUG, "DataStorage: %s.", __FUNCTION__);
    dep_mtx_.unlock();
}

/**
    @brief Find a parameter in Datastorage

    @param[in]  coll_name            Collection name
    @param[in]  par_name             Parameter name
    @return     A pointer to a ParameterBase
*/
ParameterBase *DataStorage::FindParameter(const std::string &coll_name, const std::string &par_name)
{
    ParameterCollection *pc = FindCollection(coll_name);
    if (!pc) {           
        LOG(LOG_WARN, "DataStorage: %s: Collection '%s' not found!", __FUNCTION__, coll_name.c_str());
        return NULL;
    }

    return pc->FindParameter(par_name);
}

/**
    @brief Find a parameter in Datastorage

    @param[in]  par_name             Parameter name
    @return     A pointer to a ParameterBase
*/
ParameterBase *DataStorage::FindParameter(const std::string &name)
{
    for (std::list<ParameterCollection*>::iterator it = pcl_.begin(); it != pcl_.end(); ++it) {
        ParameterBase *par = (*it)->FindParameter(name);
        if (par)
            return par;
    }
    
    return NULL;
}

/**
    @brief Add or change an int parameter in Datastorage

    @param[in]  coll_name            Collection name
    @param[in]  par_name             Parameter name
    @param[in]  par_val              Parameter value
    @param[in]  event_on_change      Send event on change
    @return     A VC return value
*/
ReturnValue DataStorage::ChangeIntParameter(const std::string &coll_name, const std::string &par_name,
                                                    int par_val, bool event_on_change)
{
    LOG(LOG_DEBUG, "DataStorage: %s.", __FUNCTION__);

    ParameterCollection *pc = FindCollection(coll_name);
    if (!pc) {
        pc = AddCollection(coll_name);
        if (!pc) {
            LOG(LOG_WARN, "DataStorage: %s: Failed to add collection '%s'!", __FUNCTION__, coll_name.c_str());
            return RET_ERR_INTERNAL;
        }
        LOG(LOG_DEBUG, "DataStorage: %s: Collection '%s' added!", __FUNCTION__, coll_name.c_str());
    }

    bool par_changed = false;
    Parameter<int> *p = (Parameter<int>*)pc->FindParameter(par_name);
    if (!p) {
        if (pc->AddParameter(pc->CreateIntParameter(par_name, par_val)) != RET_OK) {
            LOG(LOG_WARN, "DataStorage: %s: Failed to add parameter '%s' to collection '%s'!",
                __FUNCTION__, par_name.c_str(), coll_name.c_str());
            return RET_ERR_INTERNAL;
        }
        LOG(LOG_DEBUG, "DataStorage: %s: Parameter '%s' added (val = %d) to collection '%s'",
                __FUNCTION__, par_name.c_str(), par_val, coll_name.c_str());

        p = (Parameter<int>*)pc->FindParameter(par_name);

        par_changed = true;
    } else {
        if (p->value_ != par_val) {
            p->value_ = par_val;
            par_changed = true;
        }
    }

    if (!par_changed)
        return RET_OK;

    if (event_on_change) {
        if (SendEventMessage(coll_name, par_name, par_val) != RET_OK) {
            LOG(LOG_WARN, "DataStorage: %s: Failed to send event on change (collection = %s, param = %s, value = %d)!",
                    __FUNCTION__, par_name.c_str(), coll_name.c_str(), par_val);
            return RET_ERR_INTERNAL;
        }      
    }

    LOG(LOG_DEBUG, "DataStorage: %s: Parameter '%s' set to (val = %d) in collection '%s'",
            __FUNCTION__, par_name.c_str(), par_val, coll_name.c_str());

    return RET_OK;
}

/**
    @brief Add or change a string parameter in Datastorage

    @param[in]  coll_name            Collection name
    @param[in]  par_name             Parameter name
    @param[in]  par_val              Parameter value
    @param[in]  event_on_change      Send event on change
    @return     A VC return value
*/
ReturnValue DataStorage::ChangeStringParameter(const std::string &coll_name, const std::string &par_name, 
                                                    const std::string &par_val, bool event_on_change)
{
    LOG(LOG_INFO, "DataStorage: %s.", __FUNCTION__);

    ParameterCollection *pc = FindCollection(coll_name);
    if (!pc) {
        pc = AddCollection(coll_name);
        if (!pc) {
            LOG(LOG_WARN, "DataStorage: %s: Failed to add collection '%s'!", __FUNCTION__, coll_name.c_str());
            return RET_ERR_INTERNAL;
        }
        LOG(LOG_DEBUG, "DataStorage: %s: Collection '%s' added!", __FUNCTION__, coll_name.c_str());
    }

    bool par_changed = false;
    Parameter<std::string> *p = (Parameter<std::string>*)pc->FindParameter(par_name);
    if (!p) {
        if (pc->AddParameter(pc->CreateStringParameter(par_name, par_val)) != RET_OK) {
            LOG(LOG_WARN, "DataStorage: %s: Failed to add parameter '%s' to collection '%s'!",
                __FUNCTION__, par_name.c_str(), coll_name.c_str());
            return RET_ERR_INTERNAL;
        }
        LOG(LOG_DEBUG, "DataStorage: %s: Parameter '%s' added (val = %s) to collection '%s'",
                __FUNCTION__, par_name.c_str(), par_val.c_str(), coll_name.c_str());

        p = (Parameter<std::string>*)pc->FindParameter(par_name);

        par_changed = true;
    } else {
        if (p->value_.compare(par_val) != 0) {
            p->value_ = par_val;
            par_changed = true;
        }
    }

    if (!par_changed)
        return RET_OK;

    if (event_on_change) {
        if (SendEventMessage(coll_name, par_name, par_val) != RET_OK) {
            LOG(LOG_WARN, "DataStorage: %s: Failed to send event on change (collection = %s, param = %s, value = %s)!",
                    __FUNCTION__, par_name.c_str(), coll_name.c_str(), par_val.c_str());
            return RET_ERR_INTERNAL;
        }      
    }

    LOG(LOG_DEBUG, "DataStorage: %s: Parameter '%s' set to (val = %s) in collection '%s'",
            __FUNCTION__, par_name.c_str(), par_val.c_str(), coll_name.c_str());

    return RET_OK;
}

/**
    @brief Add or change a char array parameter in Datastorage

    @param[in]  coll_name            Collection name
    @param[in]  par_name             Parameter name
    @param[in]  par_val              Parameter value
    @param[in]  event_on_change      Send event on change
    @return     A VC return value
*/
ReturnValue DataStorage::ChangeCharArrayParameter(const std::string &coll_name, const std::string &par_name, 
                                                    const std::vector<unsigned char> &par_val, bool event_on_change)
{
    LOG(LOG_INFO, "DataStorage: %s.", __FUNCTION__);

    if (par_val.size() > N_VECT_MAX) {
        LOG(LOG_WARN, "DataStorage: %s: Input vector size (%d) is bigger than maximum allowed (%d) vector size",
            __FUNCTION__, par_val.size(), N_VECT_MAX);
        return RET_ERR_INTERNAL;
    }

    ParameterCollection *pc = FindCollection(coll_name);
    if (!pc) {
        pc = AddCollection(coll_name);
        if (!pc) {
            LOG(LOG_WARN, "DataStorage: %s: Failed to add collection '%s'!", __FUNCTION__, coll_name.c_str());
            return RET_ERR_INTERNAL;
        }
        LOG(LOG_DEBUG, "DataStorage: %s: Collection '%s' added!", __FUNCTION__, coll_name.c_str());
    }

    bool par_changed = false;
    Parameter<std::vector<unsigned char>> *p = (Parameter<std::vector<unsigned char>>*)pc->FindParameter(par_name);
    if (!p) {
        if (pc->AddParameter(pc->CreateCharArrayParameter(par_name, par_val)) != RET_OK) {
            LOG(LOG_WARN, "DataStorage: %s: Failed to add parameter '%s' to collection '%s'!",
                __FUNCTION__, par_name.c_str(), coll_name.c_str());
            return RET_ERR_INTERNAL;
        }
        LOG(LOG_DEBUG, "DataStorage: %s: Parameter '%s' added to collection '%s'",
                __FUNCTION__, par_name.c_str(), coll_name.c_str());

        p = (Parameter<std::vector<unsigned char>>*)pc->FindParameter(par_name);

        par_changed = true;
    } else {
        if (p->value_ != par_val) {
            p->value_ = par_val;
            par_changed = true;
        }
    }

    if (!par_changed)
        return RET_OK;

    if (event_on_change) {
        if (SendEventMessage(coll_name, par_name, par_val) != RET_OK) {
            LOG(LOG_WARN, "DataStorage: %s: Failed to send event on change (collection = %s, param = %s)!",
                    __FUNCTION__, par_name.c_str(), coll_name.c_str());
            return RET_ERR_INTERNAL;
        }      
    }

    LOG(LOG_DEBUG, "DataStorage: %s: Parameter '%s' set in collection '%s'",
            __FUNCTION__, par_name.c_str(), coll_name.c_str());

    return RET_OK;
}

/**
    @brief Method to retrieve or set information to DataStorage
    
    Method to retrieve or set information to DataStorage by events
    or request/response.

    @param[in]  m      Input message from Vehiclecomm.
    @return     A VC return value
*/

ReturnValue DataStorage::SendMessage(MessageBase *m)
{
    LOG(LOG_INFO, "DataStorage: %s.", __FUNCTION__);
    ReturnValue ret;

    if (m == NULL) {
        LOG(LOG_WARN, "DataStorage: %s: Invalid input message (NULL).", __FUNCTION__);
        return RET_ERR_INVALID_ARG;
    }

    switch(MessageType(m->type_)) {
    case MESSAGE_REQUEST: {
        switch ((RequestID)m->id_) {
        case REQ_GETDSPARAMETER: {
            ret = HandleGetDSParameter((Message<ReqGetDSParameter>*)m);
            break;
        }
        default:
            LOG(LOG_WARN, "DataStorage: %s Invalid request (id = %d)!", __FUNCTION__, m->id_);
            ret = RET_ERR_INTERNAL;
            break;
        }
        if (ret != RET_OK)
            SendErrorMessage(mq_, m, ret);
        break;
    }
    default:
        LOG(LOG_WARN, "DataStorage: %s: Invalid message type %d.", __FUNCTION__, m->type_);
        ret = RET_ERR_INTERNAL;
        break;
    }

    return ret;
}

/**
    @brief Init method for DataStorage class
    
    Initialize the DataStorage instance.
*/
ReturnValue DataStorage::Init()
{
    LOG(LOG_INFO, "DataStorage: %s.", __FUNCTION__);

    return RET_OK;
}

// uia93888 remote start engine
////////////////////////////////////////////////////////////
// @brief : vuc request - get car's status 
//
// @return     A ResRMTStatus
// @author     uia93888, Sep 18, 2018
////////////////////////////////////////////////////////////
ResRMTStatus DataStorage::GetRMTEngineStatus()
{
    LOG(LOG_INFO, "DataStorage: %s.", __FUNCTION__);
    ResRMTStatus res;
    ParameterCollection* _door_collection = FindCollection("DoorState");
    ParameterCollection* _win_collection = FindCollection("WinState");
    ParameterCollection* _air_collection = FindCollection("CmpmtAirState");
    if(_door_collection){
        //doorDrvrSts
        Parameter<int> *p = (Parameter<int>*)_door_collection->FindParameter("driverDoorState");
        if(!p){
            res.doorDrvrSts = 0;
        } else {
            res.doorDrvrSts = p->value_;
            p = NULL;
        }
        //doorLeReSts
        p = (Parameter<int>*)_door_collection->FindParameter("driverSideRearDoorState");
        if(!p){
            res.doorLeReSts = 0;
        } else {
            res.doorLeReSts = p->value_;
            p = NULL;
        }
        //doorDrvrLockSts
        p = (Parameter<int>*)_door_collection->FindParameter("driverDoorLockState");
        if(!p){
            res.doorDrvrLockSts = 0;
        } else {
            res.doorDrvrLockSts = p->value_;
            p = NULL;
        }
        //doorLeReLockSrs
        p = (Parameter<int>*)_door_collection->FindParameter("driverSideRearDoorLockState");
        if(!p){
            res.doorLeReLockSrs = 0;
        } else {
            res.doorLeReLockSrs = p->value_;
            p = NULL;
        }
        //doorPassSts
        p = (Parameter<int>*)_door_collection->FindParameter("passengerDoorState");
        if(!p){
            res.doorPassSts = 0;
        } else {
            res.doorPassSts = p->value_;
            p = NULL;
        }
        //doorRiReSts
        p = (Parameter<int>*)_door_collection->FindParameter("passengerSideRearDoorState");
        if(!p){
            res.doorRiReSts = 0;
        } else {
            res.doorRiReSts = p->value_;
            p = NULL;
        }
        //doorPassLockSts
        p = (Parameter<int>*)_door_collection->FindParameter("passengerDoorLockState");
        if(!p){
            res.doorPassLockSts = 0;
        } else {
            res.doorPassLockSts = p->value_;
            p = NULL;
        }
        //doorRiReLockSts
        p = (Parameter<int>*)_door_collection->FindParameter("passengerSideRearDoorLockState");
        if(!p){
            res.doorRiReLockSts = 0;
        } else {
            res.doorRiReLockSts = p->value_;
            p = NULL;
        }
        //trSts
        p = (Parameter<int>*)_door_collection->FindParameter("tailgateOrBootlidState");
        if(!p){
            res.trSts = 0;
        } else {
            res.trSts = p->value_;
            p = NULL;
        }
        //trLockSts
        p = (Parameter<int>*)_door_collection->FindParameter("tailgateOrBootlidLockState");
        if(!p){
            res.trLockSts = 0;
        } else {
            res.trLockSts = p->value_;
            p = NULL;
        }
        //hoodSts
        p = (Parameter<int>*)_door_collection->FindParameter("hoodState");
        if(!p){
            res.hoodSts = 0;
        } else {
            res.hoodSts = p->value_;
            p = NULL;
        }
        //lockgCenStsForUsrFb
        p = (Parameter<int>*)_door_collection->FindParameter("centralLockingStateForUserFeedback");
        if(!p){
            res.lockgCenStsForUsrFb = 0;
        } else {
            res.lockgCenStsForUsrFb = p->value_;
            p = NULL;
        }
        //lockgCenSts
        p = (Parameter<int>*)_door_collection->FindParameter("centralLockState");
        if(!p){
            res.lockgCenSts = 0;
        } else {
            res.lockgCenSts = p->value_;
            p = NULL;
        }
        LOG(LOG_ERR, "VuCComm: %s: successed to get collection DoorState.", __FUNCTION__);
        //TODO: get door status
    } else {
        res.lockgCenSts = 0;
        res.doorPassLockSts = 0;
        res.doorPassSts = 0;
        res.doorLeReLockSrs = 0;
        res.doorLeReSts = 0;
        res.doorDrvrSts = 0;
        res.doorDrvrLockSts = 0;
        res.doorRiReLockSts = 0;
        res.doorRiReSts = 0;
        res.trLockSts = 0;
        res.trSts = 0;
        res.hoodSts = 0;
        res.lockgCenStsForUsrFb = 0;
        LOG(LOG_ERR, "VuCComm: %s: failed to get collection DoorState.", __FUNCTION__);
    }

    if(_win_collection){
        //sunroofPosnSts
        Parameter<int> *p = (Parameter<int>*)_win_collection->FindParameter("sunroofPosnSts");
        if(!p){
            res.sunroofPosnSts = 0;
        } else {
            res.sunroofPosnSts = p->value_;
            p = NULL;
        }
        //winPosnStsAtDrv
        p = (Parameter<int>*)_win_collection->FindParameter("winPosnStsAtDrv");
        if(!p){
            res.winPosnStsAtDrv = 0;
        } else {
            res.winPosnStsAtDrv = p->value_;
            p = NULL;
        }
        //winPosnStsAtPass
        p = (Parameter<int>*)_win_collection->FindParameter("winPosnStsAtPass");
        if(!p){
            res.winPosnStsAtPass = 0;
        } else {
            res.winPosnStsAtPass = p->value_;
            p = NULL;
        }
        //winPosnStsReLe
        p = (Parameter<int>*)_win_collection->FindParameter("winPosnStsReLe");
        if(!p){
            res.winPosnStsReLe = 0;
        } else {
            res.winPosnStsReLe = p->value_;
            p = NULL;
        }
        //winPosnStsReRi
        p = (Parameter<int>*)_win_collection->FindParameter("winPosnStsReRi");
        if(!p){
            res.winPosnStsReRi = 0;
        } else {
            res.winPosnStsReRi = p->value_;
            p = NULL;
        }

        LOG(LOG_INFO, "VuCComm: %s: successed to get collection WinState.", __FUNCTION__);
        //TODO: get door status
    } else {
        res.sunroofPosnSts = 0;
        res.winPosnStsAtDrv = 0;
        res.winPosnStsAtPass = 0;
        res.winPosnStsReLe = 0;
        res.winPosnStsReRi = 0;
        LOG(LOG_ERR, "VuCComm: %s: failed to get collection DoorState.", __FUNCTION__);
    }

    if(_air_collection){
        //interiorPM25
        Parameter<int> *p = (Parameter<int>*)_air_collection->FindParameter("cmpmtInAirPMLvl");
        if(!p){
            res.interiorPM25Lvl = 0;
        } else {
            res.interiorPM25Lvl = p->value_;
            p = NULL;
        }
        //exteriorPM25
        p = (Parameter<int>*)_air_collection->FindParameter("cmpmtOutAirPMLvl");
        if(!p){
            res.exteriorPM25Lvl = 0;
        } else {
            res.exteriorPM25Lvl = p->value_;
            p = NULL;
        }
        p = (Parameter<int>*)_air_collection->FindParameter("intPm25Vlu");
        if(!p){
            res.interiorPM25Vlu = 0;
        } else {
            res.interiorPM25Vlu = p->value_;
            p = NULL;
        }
        p = (Parameter<int>*)_air_collection->FindParameter("outdPm25Vlu");
        if(!p){
            res.exteriorPM25Vlu = 0;
        } else {
            res.exteriorPM25Vlu = p->value_;
            p = NULL;
        }
        LOG(LOG_INFO, "VuCComm: %s: successed to get collection AirState.", __FUNCTION__);
        //TODO: get door status
    } else {
        res.interiorPM25Lvl = 0;
        res.exteriorPM25Lvl = 0;
        res.interiorPM25Vlu = 0;
        res.exteriorPM25Vlu = 0;
        LOG(LOG_ERR, "VuCComm: %s: failed to get collection AirState.", __FUNCTION__);
    }
        
    res.ventilateStatus = 0;
    res.vehSpdIndcd =0;
    res.vehSpdIndcdQly = 0;
    res.engOilLvlSts = 0;
    res.epbSts = 0;
    res.fuLvlIndcd = 0;
    res.fuLvlLoIndcn = 0;
    res.fulLvlLoWarn = 0;

    LOG(LOG_INFO, "GetRMTEngineStatus: doorPassLockSts = %d", res.doorPassLockSts);
    LOG(LOG_INFO, "GetRMTEngineStatus: doorPassSts = %d", res.doorPassSts);
    LOG(LOG_INFO, "GetRMTEngineStatus: doorLeReLockSrs = %d", res.doorLeReLockSrs);
    LOG(LOG_INFO, "GetRMTEngineStatus: doorLeReSts = %d", res.doorLeReSts);
    LOG(LOG_INFO, "GetRMTEngineStatus: doorDrvrSts = %d", res.doorDrvrSts);
    LOG(LOG_INFO, "GetRMTEngineStatus: doorDrvrLockSts = %d", res.doorDrvrLockSts);
    LOG(LOG_INFO, "GetRMTEngineStatus: doorRiReLockSts = %d", res.doorRiReLockSts);
    LOG(LOG_INFO, "GetRMTEngineStatus: doorRiReSts = %d", res.doorRiReSts);
    LOG(LOG_INFO, "GetRMTEngineStatus: lockgCenSts = %d", res.lockgCenSts);
    LOG(LOG_INFO, "GetRMTEngineStatus: trLockSts = %d", res.trLockSts);
    LOG(LOG_INFO, "GetRMTEngineStatus: trSts = %d", res.trSts);
    LOG(LOG_INFO, "GetRMTEngineStatus: hoodSts = %d", res.hoodSts);
    LOG(LOG_INFO, "GetRMTEngineStatus: lockgCenStsForUsrFb = %d", res.lockgCenStsForUsrFb);
    LOG(LOG_INFO, "GetRMTEngineStatus: sunroofPosnSts = %d", res.sunroofPosnSts);
    LOG(LOG_INFO, "GetRMTEngineStatus: winPosnStsAtDrv = %d", res.winPosnStsAtDrv);
    LOG(LOG_INFO, "GetRMTEngineStatus: winPosnStsAtPass = %d", res.winPosnStsAtPass);
    LOG(LOG_INFO, "GetRMTEngineStatus: winPosnStsReLe = %d", res.winPosnStsReLe);
    LOG(LOG_INFO, "GetRMTEngineStatus: winPosnStsReRi = %d", res.winPosnStsReRi);
    LOG(LOG_INFO, "GetRMTEngineStatus: ventilateStatus = %d", res.ventilateStatus);
    LOG(LOG_INFO, "GetRMTEngineStatus: interiorPM25Lvl = %d", res.interiorPM25Lvl);
    LOG(LOG_INFO, "GetRMTEngineStatus: exteriorPM25Lvl = %d", res.exteriorPM25Lvl);
    LOG(LOG_INFO, "GetRMTEngineStatus: interiorPM25Vlu = %d", res.interiorPM25Vlu);
    LOG(LOG_INFO, "GetRMTEngineStatus: exteriorPM25Vlu = %d", res.exteriorPM25Vlu);
    
    return res;
}

////////////////////////////////////////////////////////////
// @brief : vuc request - get engien states 
//
// @return     A ResRMTStatus
// @author     uia93888, Oct 23, 2018
////////////////////////////////////////////////////////////
Res_RMTEngine DataStorage::GetEngineStates()
{
    LOG(LOG_INFO, "DataStorage: %s.", __FUNCTION__);
    Res_RMTEngine res;
    ParameterCollection* _engine_collection = FindCollection("RMT_EngineStates");
    if(_engine_collection){
        //doorDrvrSts
        Parameter<int> *p = (Parameter<int>*)_engine_collection->FindParameter("engSt1WdSts");
        if(!p){
            res.engSt1WdSts = EngStl_Ini;
        } else {
            res.engSt1WdSts = (vc::EngStlWdSts)p->value_;
            p = NULL;
        }
        p = (Parameter<int>*)_engine_collection->FindParameter("ersStrtApplSts");
        if(!p){
            res.ersStrtApplSts = ErsStrtApplSts_ErsStsOff;
        } else {
            res.ersStrtApplSts = (vc::ErsStrtApplSts)p->value_;
            p = NULL;
        }
        p = (Parameter<int>*)_engine_collection->FindParameter("ersStrtRes");
        if(!p){
            res.ersStrtRes = ErsStrtRes_ErsStrtNotSet;
        } else {
            res.ersStrtRes = (vc::ErsStrtRes)p->value_;
            p = NULL;
        }
    }else{
        res.engSt1WdSts = EngStl_Ini;
        res.ersStrtApplSts = ErsStrtApplSts_ErsStsOff;
        res.ersStrtRes = ErsStrtRes_ErsStrtNotSet;
    }
    return res;
}
}
