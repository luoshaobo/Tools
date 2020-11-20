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

#include "dlt/dlt.h"
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <list>
#include <cstring>
#include <stdio.h> 
#include <sstream>

#include "vds_marben_adapt_base.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);


VdsMarbenAdaptBase::VdsMarbenAdaptBase()
{
	DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "VdsMarbenAdaptBase::%s\n", __FUNCTION__);
        m_vdsObj = new asn_wrapper::VDServiceRequest(); 
}

VdsMarbenAdaptBase::VdsMarbenAdaptBase(asn_wrapper::VDServiceRequest* vdsObj)
{
	DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "VdsMarbenAdaptBase::%s\n", __FUNCTION__);
	m_vdsObj = vdsObj;
}

 VdsMarbenAdaptBase::~VdsMarbenAdaptBase()
 {
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "VdsMarbenAdaptBase::%s\n", __FUNCTION__);
 }

bool  VdsMarbenAdaptBase::verifyScope(const std::string &fieldName, int64_t value, int64_t minValue, int64_t maxValue) 
{
    if(!m_isCheck)
        return true;

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "VdsMarbenAdaptBase::%s fieldName:%s,value:%lld, minValue:%lld, maxValue:%lld.\n",
    __FUNCTION__, fieldName.c_str(), value, minValue, maxValue);

    if( value < minValue || value > maxValue)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "VdsMarbenAdaptBase::%s failed.\n", __FUNCTION__);
        return false;
    }
    
    return true;
}

bool  VdsMarbenAdaptBase::verifyPrintableString(const std::string &fieldName, const std::string  &value)
{
    if(!m_isCheck)
        return true;

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "VdsMarbenAdaptBase::%s fieldName:%s, value:%s.\n", __FUNCTION__,fieldName.c_str(), value.c_str());

    /*
     * Check the alphabet of the PrintableString.
     * ASN.1:1984 (X.409)
     */
    static int _PrintableString_alphabet[256] = {
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    /*                  */
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    /*                  */
     1, 0, 0, 0, 0, 0, 0, 2, 3, 4, 0, 5, 6, 7, 8, 9,    /* .      '() +,-./ */
    10,11,12,13,14,15,16,17,18,19,20, 0, 0,21, 0,22,    /* 0123456789:  = ? */
     0,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,    /*  ABCDEFGHIJKLMNO */
    38,39,40,41,42,43,44,45,46,47,48, 0, 0, 0, 0, 0,    /* PQRSTUVWXYZ      */
     0,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,    /*  abcdefghijklmno */
    64,65,66,67,68,69,70,71,72,73,74, 0, 0, 0, 0, 0,    /* pqrstuvwxyz      */
    };

    size_t i = 0;    
    for(i = 0; i < value.length(); i++) {
        if(_PrintableString_alphabet[(int)value.at(i)] == 0) {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "VdsMarbenAdaptBase::%s alphabet: %s not allowed.\n", __FUNCTION__, value.at(i));
            return false;
        }
    }

    return true;
}


bool  VdsMarbenAdaptBase::verifyPrintableString(const std::string &fieldName, const std::string  &value, int64_t minSize, int64_t maxSize ) 
{
    if(!m_isCheck)
        return true;

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "VdsMarbenAdaptBase::%s fieldName:%s, value:%s, minSize:%lld, maxSize:%lld\n",
    __FUNCTION__, fieldName.c_str(), value.c_str(), minSize, maxSize);

    if( value.size() < minSize || value.size() > maxSize)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "VdsMarbenAdaptBase::%s failed.\n", __FUNCTION__);
        return false;
    }

   return verifyPrintableString(fieldName, value);
}

void  VdsMarbenAdaptBase::splitString(const std::string& s,  std::vector< std::string>& v, const  std::string& c)
{
    std::string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;

    while(std::string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2-pos1));
        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }

    if(pos1 != s.length())
        v.push_back(s.substr(pos1));
}


void VdsMarbenAdaptBase::OctetToString(unsigned char * bArray, char *&data_out,  int bArray_len)
{
    int nIndex = 0;
    for(int i=0; i<bArray_len; i++)
    {
    	int high = bArray[i]/16, low = bArray[i]%16;
    	data_out[nIndex] = (high<10) ? ('0' + high) : ('A' + high - 10);
    	data_out[nIndex + 1] = (low<10) ? ('0' + low) : ('A' + low - 10);
        if(i < bArray_len - 1)
	{
            data_out[nIndex + 2] = ' ';
	}
    	nIndex += 3;
    }
}

void VdsMarbenAdaptBase::StringToOctet(std::string data, unsigned char *&data_out,  int &size)
{
    int hexOctetValue;
    const char *sep = ":. ";
    char *p;
    
    char tmp[ data.length() + 1 ] = {0};
    for(unsigned int i = 0; i < data.length(); i++)
    {
       tmp[i] = data[i];
    }
    
    int j = 0;
    p = strtok(tmp, sep);

    while(p)
    {
        std::istringstream iss(p);
        iss >> std::hex >> hexOctetValue;
        data_out[j] = (unsigned char)hexOctetValue;
        
        p = strtok(NULL, sep);
        j++;
    }
    
    data_out[j] = '\0';
    size = j;
}


