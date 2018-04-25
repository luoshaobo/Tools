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

//==============================================================================
// INCLUDES
//==============================================================================
#include "entTlmXmlUtils.h"

//==============================================================================
// IMPLEMENTATION
//==============================================================================

/*
 * \brief Create a BSTR from a UTF8 string.
 *
 *
 * \param utf8Str     [in]    The UTF8 string.
 *
 * \return BSTR: the created BSTR.
 */
BSTR entTlmXmlUtils::createBstrFromUtf8(const T_STRING &utf8Str)
{
    int err = 0;
    int bsLen = 0;
    int bsBufLen = 0;
    wchar_t *bsBuf = 0;
    BSTR bsRet = 0;

    if (err == 0) {
        bsBufLen = MultiByteToWideChar(
            CP_UTF8,
            0,
            utf8Str.c_str(),
            -1,
            0,
            0
        );
        if (bsBufLen > 0) {
            // OK
        } else {
            err = -1;
        }
    }
    
    if (err == 0) {
        bsBuf = new wchar_t[bsBufLen];
        if (bsBuf != 0) {
            bsLen = MultiByteToWideChar(
                CP_UTF8,
                0,
                utf8Str.c_str(),
                -1,
                bsBuf,
                bsBufLen
            );
            if (bsLen > 0) {
                bsRet = SysAllocString(bsBuf);
                if (bsRet > 0) {
                    // OK
                } else {
                    err = -1;
                }
            } else {
                err = -1;
            }
        } else {
            err = -1;
        }
    }

    if (err != 0) {
        if (bsRet != 0) {
            SysFreeString(bsRet);
            bsRet = 0;
        }
    }
    
    if (bsBuf != 0) {
        delete[] bsBuf;
        bsBuf = 0;
    }
    return bsRet;
}

/*
 * \brief Destroy a BSTR.
 *
 *
 * \return N/A.
 */
void entTlmXmlUtils::destroyBstr(BSTR bstr)
{
    if (bstr != 0) {
        SysFreeString(bstr);
    }
}

/*
 * \brief Convert a BSTR to a UTF8 string.
 *
 *
 * \param bstr     [in]    The BSTR.
 *
 * \return entTlmXmlUtils::T_STRING: the UTF8 string.
 */
entTlmXmlUtils::T_STRING entTlmXmlUtils::convertBstrToUtf8(const BSTR& bstr)
{
    int err = 0;
    int sLen = 0;
    int sBufLen = 0;
    char *sBuf = 0;
    T_STRING strRet("");

    if (err == 0) {
        if (bstr == 0) {
            err = -1;
        }
    }

    if (err == 0) {
        sBufLen = WideCharToMultiByte(
            CP_UTF8,
            0,
            bstr,
            SysStringLen(bstr),
            0,
            0,
            0,
            0
        );
        if (sBufLen > 0) {
            // OK
        } else {
            err = -1;
        }
    }
    
    if (err == 0) {
        sBuf = new char[sBufLen + 1];
        if (sBuf != 0) {
            sLen = WideCharToMultiByte(
                CP_UTF8,
                0,
                bstr,
                SysStringLen(bstr),
                sBuf,
                sBufLen,
                0,
                0
            );
            if (sLen > 0) {
                sBuf[sBufLen] = '\0';
                strRet = T_STRING(sBuf);
            } else {
                err = -1;
            }
        } else {
            err = -1;
        }
    }
    
    if (sBuf != 0) {
        delete[] sBuf;
        sBuf = 0;
    }

    return strRet;
}
