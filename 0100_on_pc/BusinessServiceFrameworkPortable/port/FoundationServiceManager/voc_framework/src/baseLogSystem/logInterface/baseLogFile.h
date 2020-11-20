/////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file baseLogFile.h
//  This file is about log function header

// @project     GLY_TCAM
// @subsystem   Application
// @author      Zhou You
// @Init date   26-Oct-2018
///////////////////////////////////////////////////////////////////



#ifndef _BASE_LOG_FILE_H_
#define _BASE_LOG_FILE_H_


#ifdef __cplusplus
extern "C"
{
#endif


#include <pthread.h>

typedef struct 
{
	char fileName[50];
	char filePath[20];
	int maxSize;
	int keepNum;
	pthread_mutex_t	 mutexRW;
}LOGDEFINE;



#ifdef __cplusplus
}
#endif

#endif
