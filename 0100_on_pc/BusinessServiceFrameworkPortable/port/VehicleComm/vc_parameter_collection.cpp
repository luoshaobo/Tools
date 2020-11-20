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

/** @file vc_parameter_collection.cpp
 * This file handles a collection of parameters in the form of
 * parameter/value pair.
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        1-Feb-2017
 ***************************************************************************/

#include "vc_utility.hpp"
#include "vc_parameter_collection.hpp"

namespace vc {

ParameterCollection::ParameterCollection(const std::string &name)
        : name_(name)
{
    LOG(LOG_DEBUG, "ParameterCollection: %s.", __FUNCTION__);
}

ParameterCollection::~ParameterCollection()
{
}

std::string GetValueString(ParameterBase *p)
{
    switch (p->type_) {
    case VCPARAM_TYPE_INT:
        return std::string("");
    case VCPARAM_TYPE_STRING:
        return ((Parameter<std::string>*)p)->value_;
    case VCPARAM_TYPE_CHARARRAY:
        return std::string("");
    default:
        return std::string(""); 
    }
}

void ParameterCollection::PrintParameters()
{
    LOG(LOG_DEBUG, "ParameterCollection: %s: %s:", __FUNCTION__, name_.c_str());

    for (std::list<ParameterBase*>::iterator it = params_.begin(); it != params_.end(); ++it) {
        switch ((*it)->type_) {
        case VCPARAM_TYPE_INT:
            LOG(LOG_DEBUG, "ParameterCollection: %s:     name = %s, int-value = %d",
                    __FUNCTION__, (*it)->name_.c_str(), ((Parameter<int>*)*it)->value_);
            break;
        case VCPARAM_TYPE_STRING:
            LOG(LOG_DEBUG, "ParameterCollection: %s:     name = %s, string-value = %s",
                    __FUNCTION__, (*it)->name_.c_str(), ((Parameter<std::string>*)*it)->value_.c_str());
            break;
        case VCPARAM_TYPE_CHARARRAY:
            LOG(LOG_DEBUG, "ParameterCollection: %s:     name = %s, vector-size = %d",
                    __FUNCTION__, (*it)->name_.c_str(), ((Parameter<std::vector<unsigned char>>*)*it)->value_.size());
            break;
        default:
            LOG(LOG_DEBUG, "ParameterCollection: %s:     illegal parameter!", __FUNCTION__);
            break;
        }
    }
}

ParameterBase *ParameterCollection::FindParameter(const std::string &name)
{
    for (std::list<ParameterBase*>::iterator it = params_.begin(); it != params_.end(); ++it)
        if ((*it)->name_ == name)
            return *it;

    return NULL;
}

ParameterBase *ParameterCollection::CreateIntParameter(const std::string &name, int value)
{
    return new Parameter<int>(name, value);
}

ParameterBase *ParameterCollection::CreateStringParameter(const std::string &name, const std::string &value)
{
    return new Parameter<std::string>(name, value);
}

ParameterBase *ParameterCollection::CreateCharArrayParameter(const std::string &name, const std::vector<unsigned char> &value)
{
    return new Parameter<std::vector<unsigned char>>(name, value);
}

ReturnValue ParameterCollection::AddParameter(ParameterBase *p)
{
    //LOG(LOG_DEBUG, "ParameterCollection: %s: name = %s, value = %s", __FUNCTION__, p->name_.c_str(), GetValueString(p).c_str());
    
    params_.push_back(p);
    return RET_OK;
}

ReturnValue ParameterCollection::DeleteParameter(std::string &name)
{
    UNUSED(name);
    return RET_OK;
}

void ParameterCollection::Clear()
{
    LOG(LOG_DEBUG, "ParameterCollection: %s.", __FUNCTION__);

    for (std::list<ParameterBase*>::iterator it = params_.begin(); it != params_.end(); ++it)
        delete *it;
    params_.clear();
}

} // namespace vc
