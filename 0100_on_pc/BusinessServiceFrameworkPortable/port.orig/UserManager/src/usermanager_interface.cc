/*
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
 *  \file     usermanager_interface.cpp
 *  \brief    FSM UserManager Interface
 *  \author   Imran Siddique
 *
 *  \addtogroup fsm_usermanagerinterface
 *  \{
*/

#include "usermanager_interface.h"
#include "usermanager.h"

#include "dlt/dlt.h"


namespace user_manager
{

std::shared_ptr<UsermanagerInterface> UsermanagerInterface::Create(std::shared_ptr<Keystore> keystore)
{
  if (!keystore)
  {
    keystore = std::make_shared<Keystore>();
  }

  return std::make_shared<Usermanager>(keystore);
}

UsermanagerInterface::~UsermanagerInterface()
{
  // Clean up
}

} // namespace user_manager

/** \}    end of addtogroup */
