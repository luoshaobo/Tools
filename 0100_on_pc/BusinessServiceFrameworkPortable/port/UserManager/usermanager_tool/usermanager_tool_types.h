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
 *  \file     usermanager_tool_types.h
 *  \brief    User Manager CLI tool types
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm_user_manager
 *  \{
*/

#ifndef USERMANAGER_TOOL_TYPES_H_
#define USERMANAGER_TOOL_TYPES_H_

#include <tuple>
#include <map>
#include <utility>

namespace fsm
{

namespace usermanager_tool
{

enum class Operation
{
    kUndefined,
    kAdd,
    kRemove,
    kUpdate,
    kList,
    kEndMarker
};

enum class EntryType
{
    kUndefined,
    kKey,
    kCertificate,
    kUser,
    kEndMarker
};

enum class ArgumentType
{
    kUndefined,
    kId,
    kFile,
    kRole,
    kEndMarker
};

typedef std::map<ArgumentType, std::string> ArgValPairs;

typedef std::tuple<Operation,
                   EntryType,
                   ArgValPairs> CommandLineArguments;

} // namespace usermanager_tool

} // namespace fsm

#endif // USERMANAGER_TOOL_TYPES_H_

/** \}    end of addtogroup */
