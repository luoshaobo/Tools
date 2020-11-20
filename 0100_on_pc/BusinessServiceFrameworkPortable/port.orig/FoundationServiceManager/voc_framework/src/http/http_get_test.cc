/**
 * Copyright (C) 2017-2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     default_cache.cpp
 *  \brief    Transfer Manager Default Cache fallback implementation.
 *  \author   Florian Schindler
 *
 *  \addtogroup transfermanager
 *  \{
 */

// ** INCLUDES *****************************************************************
#include "http_get_test.h"

#include <iostream>
#include <chrono>
#include <thread>

#include "dlt/dlt.h"
#include "icommunicatorproxy.h"
#include "icommunicationmanagerproxyevents.h"
#include "icommunicationmanagerproxy.h"
#include "thread.h"


using namespace fsm;

class t_call_back_test
{
public:
  void call_back(ResponseCommunicatorDataHttpsGet &callback)
  {
    printf("---------%s(), response body=%s\n", 
         __FUNCTION__,
        callback.payload().c_str());
    return;
  }
};

int main()
{
  std::cout << "start..." <<std::endl;

  t_call_back_test test_callback;
  t_http_get::t_callback_fun fun = std::bind(&t_call_back_test::call_back, &test_callback, _1);
  
  t_http_get http_get(fun);
  http_get.init();

  std::string url = "http://issuecdn.baidupcs.com/issue/netdisk/yunguanjia/6.7.1.9.exe";
  std::vector<std::string> headers;
  headers.push_back("Range:bytes=83-89");
  http_get.get(url, headers);

  std::chrono::milliseconds dura(200 * 1000);
  std::this_thread::sleep_for(dura);
  return 0;
}

/** \}    end of addtogroup */
