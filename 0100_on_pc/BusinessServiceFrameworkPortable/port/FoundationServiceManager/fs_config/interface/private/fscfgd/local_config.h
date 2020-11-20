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
 *  \file     local_config.h
 *  \brief    Foundation Services Service File-based local-config interface.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_LOCAL_CONFIG_H_INC_
#define FSM_LOCAL_CONFIG_H_INC_

#include <string>
#include <map>

#include <fscfg/fscfg_types.h>

namespace fsm
{

/**
 * \brief Foundation Services Service File-based local-config file parser.
 */
class LocalConfig
{
public:
    /**
     * \brief Constructor.
     */
    LocalConfig();

    /**
     * \brief Retrieves the entry point URL.
     *
     * \param[out] entry_point_url entry point URL value from config file.
     *
     * \return fscfg_kRcBadState if parsing of config failed,\n
     *         fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetEntryPointUrl(std::string& entry_point_url);

    /**
     * \brief Retrieves the SSL tunnel configuration.The decision is based on the prefix
     *        of the entry point, prefix https means secure connection
     *
     * \param[out] run_unsecure true if allowed running without SSL tunnel, false otherwise.
     *
     * \return fscfg_kRcBadState if parsing of config failed,\n
     *         fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetRunUnsecure(bool& run_unsecure);

private:
    /**
     * \brief Copy constructor.
     */
    LocalConfig(const LocalConfig& other);
    /**
     * \brief Assignment operator.
     */
    LocalConfig& operator=(const LocalConfig& other);

};

} // namespace fsm

#endif // FSM_LOCAL_CONFIG_H_INC_

/** \}    end of addtogroup */
