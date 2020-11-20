/////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file logInterface.h
//  This file is about the log Interface

// @project     GLY_TCAM
// @subsystem   Application
// @author      Zhou You
// @Init date   26-Oct-2018
///////////////////////////////////////////////////////////////////

#ifndef _LOG_INTERFACE_H_
#define _LOG_INTERFACE_H_
typedef void* logCtlHandle;


#ifdef __cplusplus
extern "C"
{
#endif


/****************************************************************
#define   FILE_NAME       "vocInfo.log"
#define   ERROR_FILE        "vocError.log"
#define   FILE_PATH   "/home/uia75973/zhouy"
#define   MAX_SIZE	  512
#define   KEEP_NUM    8
logCtlHandle ErrLogHandle;
logCtlHandle InfoLogHandle;
***************************************************************/



extern logCtlHandle logInitHandle(const char *fileNm,const char *filePh,int maxSz,int bkNum);
extern void BASE_ERROR_LOG(logCtlHandle lgHandle,const char *format,...);
extern void BASE_INFO_LOG(logCtlHandle lgHandle,const char *format,...);

#ifdef __cplusplus
}
#endif

#endif
