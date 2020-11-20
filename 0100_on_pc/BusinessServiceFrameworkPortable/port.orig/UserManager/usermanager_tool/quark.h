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
 *  \file     quark.h
 *  \brief    Quark data structure interface
 *            association between string and int
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm_usermanager
 *  \{
*/

#include <usermanager_types.h>

#include <map>

namespace fsm
{

namespace usermanager_tool
{

class Quark
{
public:
    Quark(const std::map<std::string, int>& values);

    ReturnCode AtKey(const std::string& key, int& value);
    ReturnCode AtValue(int value, std::string& key);
    const std::map<std::string, int>& GetMap() const;

    Quark() = default;
    Quark(const Quark& other) = default;
    Quark& operator=(const Quark& other) = default;
    ~Quark() = default;

protected:
    std::map<std::string, int> values_;
};

} // namespace usermanager_tool

} // namespace fsm

/** \}    end of addtogroup */
