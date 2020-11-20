////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file utils.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Jiaojiao Shen
// @Init date   12-Sep-2018
////////////////////////////////////////////////////////////////////////////

#include <ctype.h>
#include <stdio.h>
#include <vector>
#include <time.h>
#include <stdbool.h>

#include <rms/util/utils.h>
#include <rms/config/settings.h>
#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

void printDumpLine(const uint8_t* const data, const uint32_t size) {
    if (!size) {
        return;
    }
    const uint32_t sizePrint = size * 3 + 1;
    std::vector<char> buf(sizePrint, 0);

    for (uint32_t i = 0, j = 0; (i < size) && (j < sizePrint); ++i) {
        j += sprintf(&buf[j], "%02X ", data[i]);
    }
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", buf.data());
}

void printDumpTable(const uint8_t* const data, const uint32_t size) {
    uint32_t i, j;

    if (!size) {
        return;
    }

    const uint8_t* p = NULL;
    const uint32_t step = 16;

    for (i = 0; i < 76; ++i) {
        printf("-");
    }
    printf("\n");

    for (i = 0; i < size; i += step) {
        printf("%08X: ", i);

        p = &data[i];
        for (j = 0; j < step; ++j) {
            if (i + j < size) {
                printf("%02X ", p[j]);
            } else {
                printf("   ");
            }

        }
        printf("| ");
        for (j = 0; j < step; ++j) {
            if (i + j < size) {
                printf("%c", isprint(p[j]) ? p[j] : '.');

            } else {
                printf(" ");
            }
        }
        printf("\n");
    }

}

//------------------------------------------------------------------------------
void printDump(const uint8_t* const data, const uint32_t size) {
#if !defined(DBPRINTF)
    printDumpTable(data, size);
#else
    printDumpLine(data, size);
#endif
}

//------------------------------------------------------------------------------
uint8_t calcCrc(const uint8_t* data, uint32_t size) {
    uint8_t crc = 0;
    for (uint32_t i = 0; i < size; ++i) {
        crc ^= data[i];
    }
    return crc;
}

DBG_TICK_TYPE DBG_getTicks(bool myMiliTicks)
{
    struct timespec timeInSeconds;
    DBG_TICK_TYPE ticks;
    clock_gettime((clockid_t)CLOCK_MONOTONIC, &timeInSeconds);
    if(myMiliTicks == true)
    {
        ticks = ((((DBG_TICK_TYPE) timeInSeconds.tv_sec)
                * DBG_CONVERT_MILLI_2_MICRO)
                + (((DBG_TICK_TYPE) timeInSeconds.tv_nsec)
                        / DBG_CONVERT_MILLI_2_NANO));
    }
    else
    {
        ticks = timeInSeconds.tv_sec;
    }
    return ticks;
}

