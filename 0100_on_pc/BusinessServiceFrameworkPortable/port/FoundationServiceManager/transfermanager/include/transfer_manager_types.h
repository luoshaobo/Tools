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
 *  \file     transfer_manager_types.h
 *  \brief    Transfer Manager common types.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup transfermanager
 *  \{
 */

#ifndef FSM_TRANSFERMANAGER_TYPES_H_
#define FSM_TRANSFERMANAGER_TYPES_H_

namespace fsm
{
namespace transfermanager
{

/**
 * \brief Full error-code space API calls can generate.
 */
enum class ErrorCode
{
    kUndefined,                 ///< Uninitialized value.

    kNone,                      ///< Operation was executed with success.
    kNotImplemented,            ///< Operation is not implemented.
    kBadState,                  ///< Object is in bad state and cannot satifsy operation.
    kBadKey,                    ///< Unexpected key or key not found.
    kNotPermitted,              ///< Operation not permitted.
    kBadValue,                  ///< Unexpected or disallowed value.
    kInvalidatedResource,       ///< Resource can be operated on but should be renewed.
    kBadResource,               ///< Resource was not identified or in a bad state.
    kDuplicateKey,              ///< Key is already present.
    kOutOfBounds,               ///< Parameter is out of bounds.
    kDestinationOutOfBounds,    ///< Destination parameter is out of bounds.
    kSourceOutOfBounds,         ///< Source parameter is out of bounds.
    kTimeout,                   ///< Operation wait-time expired.
    kInternalError,             ///< An internal error happened

    kMax                        ///< Maximum value type should hold. Keep this last.
};

} // namespace transfermanager
} // namespace fsm

#endif // FSM_TRANSFERMANAGER_TYPES_H_

/** \}    end of addtogroup */
