#ifndef VC_PARAMETER_COLLECTION_HPP
#define VC_PARAMETER_COLLECTION_HPP

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

/** @file vc_parameter_collection.hpp
 * This file handles a collection of parameters in the form of
 * parameter/value pair.
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        1-Feb-2017
 ***************************************************************************/

#include <list>
#include <memory>
#include <vector>

namespace vc {

/**
    @brief Enum for the different parameter-types.
*/
typedef enum {
    VCPARAM_TYPE_UNKNOWN,
    VCPARAM_TYPE_INT,
    VCPARAM_TYPE_STRING,
    VCPARAM_TYPE_CHARARRAY
} VCParamType;


/**
    @brief The base-class for different parameter-types.

    This class is the base for all objects of the template-class Parameter.
*/
class ParameterBase {
  public:
    std::string name_;
    VCParamType type_;
    ParameterBase(const std::string &name, const VCParamType type) : name_(name), type_(type) {};
};

/**
    @brief Template-class for the Parameter-classes.

    This class is the template-class for the Parameter-classes.
*/
template<typename S>
class Parameter : public ParameterBase {
  public:
    Parameter(const std::string &name, const S& value) : ParameterBase(name, VCPARAM_TYPE_UNKNOWN), value_(value) {};
    S value_;
};

/**
    @brief Integer Parameter-classes.

    This class is the integer-variant of the Parameter template-class.
*/
template<>
class Parameter<int> : public ParameterBase {
  public:
    Parameter(const std::string &name, const int &value) : ParameterBase(name, VCPARAM_TYPE_INT), value_(value) {};
    int value_;
};

/**
    @brief String Parameter-classes.

    This class is the string-variant of the Parameter template-class.
*/
template<>
class Parameter<std::string> : public ParameterBase {
  public:
    Parameter(const std::string &name, const std::string &value) : ParameterBase(name, VCPARAM_TYPE_STRING), value_(value) {};
    std::string value_;
};

/**
    @brief Vector Parameter-classes.

    This class is the vector-variant of the Parameter template-class.
*/
template<>
class Parameter<std::vector<unsigned char>> : public ParameterBase {
  public:
    Parameter(const std::string &name, const std::vector<unsigned char> &value) : ParameterBase(name, VCPARAM_TYPE_CHARARRAY), value_(value) {};
    std::vector<unsigned char> value_;
};

/**
    @brief A collection (list) of Parameter-objects.

    This class is used to store one or more objects of the type Parameter.
*/
class ParameterCollection {
  private:
    std::list<ParameterBase*> params_;        /**< The list of parameters */

  public:
    std::string name_;

    ParameterCollection(const std::string &name);
    ~ParameterCollection();
    void PrintParameters();
    ParameterBase *FindParameter(const std::string &name);
    ParameterBase *CreateIntParameter(const std::string &name, int value);
    ParameterBase *CreateStringParameter(const std::string &name, const std::string &value);
    ParameterBase *CreateCharArrayParameter(const std::string &name, const std::vector<unsigned char> &value);
    ReturnValue AddParameter(ParameterBase *p);
    ReturnValue DeleteParameter(std::string &name);
    void Clear();
};

} // namespace vc

#endif // VC_PARAMETER_COLLECTION_HPP
