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
 *  \file     fscfgd.cpp
 *  \brief    Foundation Services Daemon-side top-level implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfgd/fscfgd.h>

namespace fsm
{

extern Daemon* daemon_global_instance_;


Daemon* GetDaemon()
{
    return daemon_global_instance_;
}

} // namespace fsm

/** \}    end of addtogroup */
