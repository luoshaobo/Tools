///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file vds_marben_adapt_cus.h
//    marben adapt functions: designed by Dev, if generated code not provided

// @project		GLY_TCAM
// @subsystem	FSM
// @author		All Dev
// @Init date	22-Feb-2019
///////////////////////////////////////////////////////////////////

#ifndef VDS_MARBEN_ADAPT_CUS_H
#define VDS_MARBEN_ADAPT_CUS_H

#include <string>
#include <vector>

class VdsMarbenAdaptCus:virtual protected VdsMarbenAdaptBase
{
public:
    VdsMarbenAdaptCus(){};
    
    VdsMarbenAdaptCus(asn_wrapper::VDServiceRequest* vdsObj) : VdsMarbenAdaptBase(vdsObj){};

};

#endif

