/**
 * \file    
 *
 * \brief The XML maker for Telematics HMI.
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

#ifndef ENT_TLM_XML_MAKER_H
#define ENT_TLM_XML_MAKER_H

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
class libxml_EXPORT entTlmXmlMaker
{
public:
    enum E_ERR {
        E_ERR_OK = 0,
        E_ERR_CREATING_COM_FAILED,
        E_ERR_ARGUMENT_INVALID,
        E_ERR_CREATING_DOM_DOC_FAILED, 
        E_ERR_QUERYING_COM_FAILED, 
        E_ERR_SETTING_MXREADER_FAILED,
        E_ERR_APPENDING_CHILD_TO_PARENT_FAILED,
        E_ERR_CREATING_INSTRUCTION_FAILED,
        E_ERR_DOM_DOC_NOT_CREATED,
        E_ERR_LOADING_XML_FAILED, 
        E_ERR_ALLOCATING_BSTR_FAILED,
        E_ERR_APPENDING_CHILD_FAILED,
        E_ERR_CREATING_NODE_FAILED,
        E_ERR_CREATING_TEXT_NODE_FAILED,
        E_ERR_CREATING_ATTRIBUTE_NODE_FAILED,
        E_ERR_SETTING_TEXT_NODE_VALUE_FAILED,
        E_ERR_SETTING_ELEMENT_ATTRIBUTE_FAILED,
        E_ERR_GETTING_XML_FAILED,
        E_ERR_GETTING_FORMATED_XML_FAILED,
        E_ERR_SETTING_MXWRITER_FAILED,
        E_ERR_PARSING_XML_FAILED,
        E_ERR_FAILED = 0xFFFFFFFF
    };

    typedef std::string T_STRING;
    typedef std::vector<T_STRING> T_STRINGS;
    typedef void* T_NODE;

public:
    entTlmXmlMaker();
    ~entTlmXmlMaker();
    
public:
    E_ERR addRootNode(const T_STRING& rootNodeName, T_NODE* rootNode);
    E_ERR addRootNode(const T_STRING& rootNodeName, const T_STRING& rootNodeValue, T_NODE* rootNode);
    E_ERR addChildNode(const T_NODE& parentNode, const T_STRING& childNodeName, T_NODE* childNode);
    E_ERR addChildNode(const T_NODE& parentNode, const T_STRING& childNodeName, const T_STRING& childNodeValue, T_NODE* childNode);
    E_ERR setNodeValue(const T_NODE& node, const T_STRING& value);
    E_ERR setNodeAtrribute(const T_NODE& node, const T_STRING& attrName, const T_STRING& attrValude);
    E_ERR makeXmlContent(T_STRING& xmlContent);
    E_ERR makeFormatedXmlContent(T_STRING& xmlContent);
    E_ERR releaseNode(const T_NODE& node);

private:
    E_ERR createDomDoc(void);
    E_ERR createXmlElement(const T_STRING& elementName, const T_STRING& elementValue, IXMLDOMElement** ppElement);
    E_ERR createXmlAtributeElement(const T_STRING& attrElementName, const T_STRING& attrElementValue, IXMLDOMAttribute** ppAttrElement);
    E_ERR appendChildToParent(IXMLDOMNode* pChild, IXMLDOMNode* pParent);
    E_ERR appendAtrributeToElement(IXMLDOMAttribute* pAtrribute, IXMLDOMElement* pElement);
    void fixEncodingInXmlContent(T_STRING& xmlContent);

private:
    entTlmXmlMaker(const entTlmXmlMaker&);
    entTlmXmlMaker& operator=(const entTlmXmlMaker&);
    
private:
    E_ERR createDomDoc_SetDocumentProperties1(void) const;
    E_ERR createDomDoc_SetDocumentProperties2(void);
    E_ERR createXmlElement_Part2(const T_STRING& elementName, const T_STRING& elementValue, IXMLDOMElement *pElementNode, IXMLDOMElement **ppElement) const;
    E_ERR createXmlAtributeElement_Part2(const T_STRING& attrElementValue, IXMLDOMAttribute *pAttrElementNode, IXMLDOMAttribute **ppAttrElement) const;
    
    void makeFormatedXmlContent_FreeResources(IMXWriter*& pMXWriter, VARIANT &vXmlConent, BSTR& bsEncoding, ISAXContentHandler*& pSAXContentHandler);
    E_ERR makeFormatedXmlContent_SetWriterProperties(IMXWriter* pMXWriter, BSTR bsEncoding, ISAXContentHandler** ppSAXContentHandler);
    E_ERR makeFormatedXmlContent_SetReaderProperties(ISAXContentHandler* pSAXContentHandler) const;

private:
    IXMLDOMDocument* m_domDoc;
};

#endif // #ifndef ENT_TLM_XML_MAKER_H
