///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file vds_marben_adapt.h
//    API for gly signal adapt

// @project		GLY_TCAM
// @subsystem	FSM
// @author		Lin Haixia
// @Init date	22-Feb-2019
///////////////////////////////////////////////////////////////////

#ifndef VDS_MARBEN_ADAPT_H
#define VDS_MARBEN_ADAPT_H

#include <string>
#include <vector>
#include "vds_marben_adapt_gen.h"
#include "vds_marben_adapt_cus.h"
#include "vds_marben_adapt_base.h"

class VdsMarbenAdapt: public VdsMarbenAdaptGen, public VdsMarbenAdaptCus
{
public:
	VdsMarbenAdapt(){};
    
	VdsMarbenAdapt(asn_wrapper::VDServiceRequest* vdsObj) : VdsMarbenAdaptBase(vdsObj){};
};

#endif

