/**
 * \file    
 *
 * \brief The XML utils for Telematics HMI.
 *
 * \par Author:
 *      - Shaobo Luo.
 *      - Phone +8621-60804724 
 *      - Shaobo.Luo@continental-corporation.com
 *
\verbatim
Date            Author              Reason
20.12.2012      Shaobo Luo          Creation of the first version
\endverbatim
 *
 * \par Copyright Notice:
 * Copyright (C) 2011 Continental Automotive Singapore Pte Ltd. 
 * Alle Rechte vorbehalten. All Rights Reserved.\n
 * The reproduction, transmission or use of this document or its contents is
 * not permitted without express written authority.\n
 * Offenders will be liable for damages. All rights, including rights created
 * by patent grant or registration of a utility model or design, are reserved.
 *
 */

#ifndef ENT_TLM_XML_UTILS_H
#define ENT_TLM_XML_UTILS_H

//==============================================================================
// INCLUDES
//==============================================================================
#include "entTlmXmlBase.h"

//==============================================================================
// PROTOTYPES
//==============================================================================
class entTlmXmlUtils
{
public:
    typedef std::string T_STRING;
    
public:
    static BSTR createBstrFromUtf8(const T_STRING& utf8Str);
    static void destroyBstr(BSTR bstr);
    static T_STRING convertBstrToUtf8(const BSTR& bstr);
};

#endif // #ifndef ENT_TLM_XML_UTILS_H
