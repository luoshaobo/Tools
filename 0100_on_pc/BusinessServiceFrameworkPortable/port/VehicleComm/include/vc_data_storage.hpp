#ifndef VC_DATA_STORAGE_HPP
#define VC_DATA_STORAGE_HPP

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

/** @file vc_data_storage.hpp
 * This file handles persistant data storage for VehicleComm
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Johan Bohlin & Ola Lilja
 * @date        25-Jan-2017
 ***************************************************************************/

#include <string.h>
#include <stdio.h>
#include <mutex>

#include "vc_common.hpp"
#include "vc_message_queue.hpp"
#include "vc_parameter_collection.hpp"

namespace vc {

/**
    @brief A class that stores locally stores data to be accessed by clients

    This class locally stores data that can be retrieved by e.g events or be parsed out
    from OTPs persistant data. This enables clients to request for this data at any time.
*/
class DataStorage { 
    private:
    static MessageQueue *mq_; /**< The main message-queue, belonging to the VehicleComm-instance */
    std::mutex dep_mtx_;

    ParameterCollection *AddCollection(const std::string& name);
    ParameterCollection *FindCollection(const std::string &name);
    ReturnValue SendEventMessage(const std::string &coll_name, const std::string &par_name, int par_val);
    ReturnValue SendEventMessage(const std::string &coll_name, const std::string &par_name, const std::string par_val);
    ReturnValue SendEventMessage(const std::string &coll_name, const std::string &par_name, const std::vector<unsigned char> par_val);

    ReturnValue HandleGetDSParameter_Str(Message<ReqGetDSParameter> *msg, Parameter<std::string>* par);
    ReturnValue HandleGetDSParameter_Int(Message<ReqGetDSParameter> *msg, Parameter<int>* par);
    ReturnValue HandleGetDSParameter_CharArr(Message<ReqGetDSParameter> *msg, Parameter<std::vector<unsigned char>>* par);
    ReturnValue HandleGetDSParameter(Message<ReqGetDSParameter> *msg);

    void PrintParameters();
    std::list<ParameterCollection*> pcl_; /**< The list of ParameterCollection, storing all parameters */

    public:
    DataStorage(MessageQueue *mq);
    ~DataStorage();

    void StartReadWriteDepValues();
    void StopReadWriteDepValues();
    ParameterBase *FindParameter(const std::string &coll_name, const std::string &par_name);
    ParameterBase *FindParameter(const std::string &name);
    ReturnValue ChangeIntParameter(const std::string &coll_name, const std::string &par_name,
                                                    int par_val, bool event_on_change);
    ReturnValue ChangeStringParameter(const std::string &coll_name, const std::string &par_name, 
                                                    const std::string &par_val, bool event_on_change);
    ReturnValue ChangeCharArrayParameter(const std::string &coll_name, const std::string &par_name,
                                                    const std::vector<unsigned char> &par_val, bool event_on_change);

    ReturnValue SendMessage(MessageBase *m);
    ReturnValue Init();
    //uia93888 remote start engine
    ResRMTStatus GetRMTEngineStatus();
    Res_RMTEngine GetEngineStates();
    /*ReturnValue Start();
    ReturnValue Stop();
    ReturnValue Suspend();
    ReturnValue Resume(); */
};

} // namespace vc

#endif // VC_DATA_STORAGE_HPP
