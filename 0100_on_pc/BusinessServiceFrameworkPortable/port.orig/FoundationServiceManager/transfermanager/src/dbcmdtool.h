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
 *  \file     dbcmdtool.h
 *  \brief    dbcmdtool header
 *  \author   Maksym Mozok
 *
 *  \addtogroup transfermanager
 *  \{
 */
#ifndef FSM_TRANSFERMANAGER_DBCMDTOOL_H
#define FSM_TRANSFERMANAGER_DBCMDTOOL_H

// ** TYPES ****************************************************************

typedef enum class Command
{
    kCmdUnknown = -1,
    kCmdAdd = 0,
    kCmdDel = 1,
    kCmdGet = 2,
    kCmdBackup = 3,
    kCmdRestore = 4
} CommandCmd;

#endif // FSM_TRANSFERMANAGER_DBCMDTOOL_H
/** \}    end of addtogroup */
