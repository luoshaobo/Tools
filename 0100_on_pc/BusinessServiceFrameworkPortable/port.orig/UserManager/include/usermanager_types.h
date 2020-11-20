/*
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
 *  \file     usermanager_types.h
 *  \brief    FSM UserManager Types
 *  \author   Imran Siddique
 *
 *  \addtogroup fsm_usermanagertypes
 *  \{
*/

#ifndef FSM_USERMANAGERTYPES_H
#define FSM_USERMANAGERTYPES_H

namespace user_manager
{

typedef int UserId;
const UserId kUndefinedUserId = -1;

enum class ReturnCode : int
{
    kUndefined,                 ///< Uninitialized value.

    kSuccess,                   ///< Operation was executed with success.
    kError,                     ///< Operation failed for unspecified reason.
    kNotFound,                  ///< The requested certificate or key was not found.
    kInvalidArgument,           ///< Invalid argument supplied.
    kNotImplemented,            ///< Operation is not implemented.

    kMax                        ///< Maximum value type should hold. Keep this last.
};

/*
 * \brief Enumerates the list of roles for which the Usermanager stores users.
 */
enum class UserRole : int
{
    kUndefined = 0, ///< Indicates a unknown or undefined user role.
    // Below actual user roles for paired devices
    kAdmin = 1,
    kUser = 2,
    kDelegate = 3,
    // Below backend users. This exact list is subject to change as requirements are clarified.
    kCarAccess = 4,
    kAssistanceCall = 5,

    kEndMarker = 6 // Mark the maximum valid value + 1
};

} // namespace user_manager

#endif // FSM_USERMANAGERTYPES_H

namespace fsm
{
    using namespace user_manager;
}


/** \}    end of addtogroup */
