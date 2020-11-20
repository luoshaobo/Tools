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
 *  \file     fscfg_types.h
 *  \brief    Foundation Services C-types
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fscfg
 *  \{
 */

#ifndef FSM_FSCFG_TYPES_H_INC_
#define FSM_FSCFG_TYPES_H_INC_

#ifdef __cplusplus
#include <functional>

extern "C" {
#endif /* __cplusplus */

/**
 * \brief Full error-code space fscfg API calls can generate.
 */
typedef enum
{
    fscfg_kRcUndefined,         /**< Uninitialized value */

    fscfg_kRcNotImplemented,    /**< Operation not implemented */
    fscfg_kRcNotSupported,      /**< Operation not supported */
    fscfg_kRcNotPermitted,      /**< Operation not permitted */
    fscfg_kRcNotFound,          /**< Requested resource does not exist */
    fscfg_kRcError,             /**< Generic error. Failing reason was not deduced. */
    fscfg_kRcSuccess,           /**< Operation was executed with success */
    fscfg_kRcKeyExists,         /**< Key already exists */
    fscfg_kRcBadState,          /**< subject is in a bad state and can't satisfy operation */
    fscfg_kRcBadParam,          /**< Unexpected call parameter */
    fscfg_kRcBadResource,       /**< Resource does not exist or is invalid */
    fscfg_kRcUnavailable,       /**< Resource unavailable */

    fscfg_kRcMax                /**< Maximum value type should hold. Keep this last. */
} fscfg_ReturnCode;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#ifdef __cplusplus
template <typename T> using Callback = std::function<fscfg_ReturnCode (T)>;
#endif /* __cplusplus */

#endif /* FSM_FSCFG_TYPES_H_INC_ */

/** \}    end of addtogroup */

