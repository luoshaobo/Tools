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
 *  \file     manual_test.h
 *  \brief    Foundation Services Daemon manual testing interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfgd/config.h>
#include <fscfgd/provisioning.h>
#include <fscfgd/discovery.h>

namespace fsm
{

class ManualTest
{
public:
    ManualTest(std::shared_ptr<Config> config,
               std::shared_ptr<Provisioning> provisioning,
               std::shared_ptr<Discovery> discovery);
    ~ManualTest();

    void Start();

private:
    ManualTest(const ManualTest& other);
    ManualTest& operator=(const ManualTest& other);

    std::shared_ptr<Config> config_;
    std::shared_ptr<Provisioning> provisioning_;
    std::shared_ptr<Discovery> discovery_;
};

} // namespace fsm

/** \}    end of addtogroup */
