///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file vds_marben_adapt_base.h
//    marben adapt functions: designed by Dev, if generated code not provided

// @project		GLY_TCAM
// @subsystem	FSM
// @author		Lin Haixia
// @Init date	22-Feb-2019
///////////////////////////////////////////////////////////////////

#ifndef VDS_MARBEN_ADAPT_BASE_H
#define VDS_MARBEN_ADAPT_BASE_H

#include <string>
#include <vector>
#include "VDSasn.h"

class VdsMarbenAdaptBase
{
public:
    VdsMarbenAdaptBase();
    
    VdsMarbenAdaptBase(asn_wrapper::VDServiceRequest* vdsObj);
    
     ~VdsMarbenAdaptBase();
    
    ////////////////////////////////////////////////////////////
    // @brief : verify scope.
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 22-Feb-2019
    ////////////////////////////////////////////////////////////
    bool verifyScope(const std::string &fieldName, int64_t value, int64_t minValue, int64_t maxValue) ;
    
    ////////////////////////////////////////////////////////////
    // @brief : verify PrintableString.
    // @return     True if successfully set, false otherwise
    // @author     lijing, 22-Feb-2019
    ////////////////////////////////////////////////////////////
    bool verifyPrintableString(const std::string &fieldName, const std::string  &value) ;
        
    ////////////////////////////////////////////////////////////
    // @brief : verify PrintableString and scope.
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 22-Feb-2019
    ////////////////////////////////////////////////////////////
    bool verifyPrintableString(const std::string &fieldName, const std::string  &value, int64_t minSize, int64_t maxSize) ;

    ////////////////////////////////////////////////////////////
    // @brief : split string.
    // @author     linhaixia, 22-Feb-2019
    ////////////////////////////////////////////////////////////
    void splitString(const std::string& s,  std::vector< std::string>& v, const  std::string& c);

    ////////////////////////////////////////////////////////////
    // @brief : octet string to string.
    // @author     linhaixia, 25-Mar-2019
    ////////////////////////////////////////////////////////////
    void OctetToString(unsigned char * bArray, char *&data_out,  int bArray_len);
	
    ////////////////////////////////////////////////////////////
    // @brief : string to octet string.
    // @author     linhaixia, 25-Mar-2019
    ////////////////////////////////////////////////////////////
    void StringToOctet(std::string data, unsigned char *&data_out,  int &size);
	
public:
    asn_wrapper::VDServiceRequest* m_vdsObj;
	
public:
    bool m_isCheck = true;
};

#endif

