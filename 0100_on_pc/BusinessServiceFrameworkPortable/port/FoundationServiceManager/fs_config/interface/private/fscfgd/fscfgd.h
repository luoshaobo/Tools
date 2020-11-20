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
 *  \file     fscfgd.h
 *  \brief    Foundation Services Daemon-side top-level interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_FSCFGD_H_INC_
#define FSM_FSCFGD_H_INC_

#include <fscfgd/daemon.h>

namespace fsm
{
    /**
     * \brief retrieves the process's Daemon instance.
     *
     * \return Daemon instance.
     */
    Daemon* GetDaemon();
} // namespace fsm

#endif // FSM_FSCFGD_H_INC_

/** \}    end of addtogroup */
