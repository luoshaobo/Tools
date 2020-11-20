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
 *  \file     main.cpp
 *  \brief    Foundation Services ConfigDaemon execution entry point
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <cstdlib>

#include <fscfgd/daemon.h>

#include <fscfgd/config.h>
#include <fscfgd/provisioning.h>
#include <fscfgd/discovery.h>

#include <fscfgd/fscfgd.h>

namespace fsm
{
    Daemon* daemon_global_instance_ = nullptr;
}

int main(int argc, const char* argv[])
{
    fsm::Daemon daemon;

    fsm::daemon_global_instance_ = &daemon;

    fscfg_ReturnCode rc = daemon.Start();

    return rc == fscfg_kRcSuccess ? EXIT_SUCCESS : EXIT_FAILURE; // as it's not implemented.
}

/** \}    end of addtogroup */
