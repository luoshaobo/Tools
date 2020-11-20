/**
 * Copyright (C) 2017-2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
% * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     persistence_stub.cpp
 *  \brief    OTP Persistence API stub implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup unittest
 *  \{
 */

// ** INCLUDES *****************************************************************

#include "persistence_stub.h"

#include <sstream>
#include <string>
#include <stack>
#include <stdlib.h>
#include <cstdint>
#include <cstring>

#include <unistd.h>

// global persistance api stub state.
namespace
{
    /** used to externally control the tpPCL API return code */
    tpPCL_Error_t g_persistence_err_code = E_PCL_ERROR_NONE;
    /** tracks the number of calls performed in the tpPCL API */
    uint32_t g_tpPCL_calls = 0;
}

void tpPCL_stub_setErrorCode(tpPCL_Error_t err_code)
{
    g_persistence_err_code = err_code;
}

uint32_t tpPCL_stub_getCallCount()
{
    return g_tpPCL_calls;
}

void tpPCL_stub_setCallCount(uint32_t call_count)
{
    g_tpPCL_calls = call_count;
}

tpPCL_Error_t tpPCL_init(const gchar* application_name, GArray* plugin_ldbids, gboolean receive_change_notification)
{
    ++g_tpPCL_calls;

    return g_persistence_err_code;
}

tpPCL_Error_t tpPCL_deinit()
{
    ++g_tpPCL_calls;

    return g_persistence_err_code;
}

tpPCL_Error_t tpPCL_getWriteablePath(const guint ldbid, gchar** path, gint* size)
{
    static char cwd_buff[PATH_MAX];
    ++g_tpPCL_calls;

    if (g_persistence_err_code == E_PCL_ERROR_NONE)
    {
        getcwd(cwd_buff, PATH_MAX);
        *path = cwd_buff;

        strncat(cwd_buff, "/tmp/", PATH_MAX);

        *size = strlen(cwd_buff);

        system("mkdir -p tmp");
    }

    return g_persistence_err_code;
}

void tpPCL_stub_clean()
{
    system("rm -rf tmp");
}

/** \}    end of addtogroup */
