/**
 * \file    
 *
 * \brief The XML parser for Telematics HMI.
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

#ifndef ENT_TLM_XML_PARSER_H
#define ENT_TLM_XML_PARSER_H

//==============================================================================
// INCLUDES
//==============================================================================
#include "entTlmXmlBase.h"
#include <msxml2.h>
#include <vector>

//==============================================================================
// MACROS
//==============================================================================
//#ifdef WIN32
//    #ifdef libtlmxml_EXPORTS
//        #define libxml_EXPORT  __declspec(dllexport)
//    #else
//        #define libxml_EXPORT  __declspec(dllimport)
//    #endif
//#else
    #define libxml_EXPORT
//#endif

//==============================================================================
// PROTOTYPES
//==============================================================================
/*
 * Examples for XPath:
 *     /bookstore                                   # all bookstores
 *     /bookstore[0]                                # the first bookstore
 *     /bookstore/book                              # all books in all bookstores
 *     /bookstore[0]/book                           # all books in the first bookstore
 *     /bookstore[0]/book[0]                        # the first book in the first bookstore
 *     /bookstore[0]/book[0]/title/@lang            # the language attribute of the title in the first book in the first bookstore
 */
class libxml_EXPORT entTlmXmlParser
{
public:
    enum E_ERR
    {
        E_ERR_OK = 0,
        E_ERR_DOM_DOC_NOT_CREATED,
        E_ERR_ALLOCATING_BSTR_FAILED,
        E_ERR_CREATING_DOM_DOC_FAILED,
        E_ERR_LOADING_XML_FAILED,
        E_ERR_ENUMING_NODE_FAILED, 
        E_ERR_GETTING_NODE_VALUE_FAILED, 
        E_ERR_SELECTING_NODES_FAILED,
        E_ERR_GETING_NODE_COUNT_FAILED,
        E_ERR_FAILED = 0xFFFFFFFF
    };

    typedef std::string T_STRING;
    typedef std::vector<T_STRING> T_STRINGS;

public:
    entTlmXmlParser(const char * xmlContent);
    ~entTlmXmlParser();
    
public:
    E_ERR queryNodeCountByXPath(const T_STRING& xpath, unsigned int& count);
    E_ERR getTextValuesByXPath(const T_STRING& xpath, T_STRINGS& retTexts);
    
private:
    E_ERR createDomDoc(void);
    E_ERR loadXml(void);
    
private:
    E_ERR getTextValuesByXPath_GetAllNodes(IXMLDOMNodeList* pNodeList, T_STRINGS &retTexts);
    E_ERR getTextValuesByXPath_GetAllNodes_GetText(IXMLDOMNode* pNode, BSTR *pbsNodeText);
    
private:
    entTlmXmlParser(const entTlmXmlParser&);
    entTlmXmlParser& operator=(const entTlmXmlParser&);

private:
    T_STRING * m_xmlContent;
    IXMLDOMDocument* m_domDoc;
    bool m_isXmlLoaded;
};

#endif // #ifndef ENT_TLM_XML_PARSER_H
