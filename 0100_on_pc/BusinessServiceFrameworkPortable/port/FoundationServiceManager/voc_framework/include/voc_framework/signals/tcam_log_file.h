/////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file tcam_log_file.h
//  This file is about the log  C++ interface

// @project     GLY_TCAM
// @subsystem   Application
// @author      Zhou You
// @Init date   26-Oct-2018
///////////////////////////////////////////////////////////////////

#ifndef _TCAM_LOG_FILE_H
#define _TCAM_LOG_FILE_H



#include <string>
#include "logInterface.h"
using namespace std;


#define   VOC_INFO_LOG       vocInfo.log
#define   VOC_ERR_LOG        vocError.log
#define   FILE_PATH   /data/Voc
#define   MAX_SIZE    128
#define   KEEP_NUM    8
logCtlHandle ErrLogHandle;
logCtlHandle InfoLogHandle;

#define STRING(x)  #x


#define TCAM_INIT_LOGHANDLE(fileNmStr,filePhStr,maxSz,bkNum) {\
	logInitHandle(STRING(fileNmStr),STRING(filePhStr),maxSz,bkNum)}

#define TCAM_INFO_LOG(logHandle,formatStr,args...) \
	(BASE_INFO_LOG(logHandle,STRING(formatStr),##args))

#define TCAM_ERROR_LOG(logHandle,formatStr,args...) \
	(BASE_ERROR_LOG(logHandle,STRING(formatStr),##args))


#endif
