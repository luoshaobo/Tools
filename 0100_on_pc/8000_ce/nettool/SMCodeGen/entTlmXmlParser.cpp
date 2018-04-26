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

//==============================================================================
// INCLUDES
//==============================================================================
#include "entTlmXmlParser.h"
#include "entTlmXmlUtils.h"

//==============================================================================
// LOCAL DEFINITIONS
//==============================================================================
#define TLM_XML_COM_RET_TYPE                            LONG
#define TLM_XML_COM_RET_OK                              0L
#define TLM_XML_COM_RET_FALSE                           1L

#define TLM_XML_VARIANT_BOOL                            SHORT
#define TLM_XML_VARIANT_FALSE                           (static_cast<TLM_XML_VARIANT_BOOL>(0))
#define TLM_XML_VARIANT_TRUE                            (static_cast<TLM_XML_VARIANT_BOOL>(-1))

//==============================================================================
// IMPLEMENTATION
//==============================================================================

/**
 * @brief Test if a HRESULT value is succeeded.
 *
 *
 * @param hr     [in]    The HRESULT value.
 *
 * @return bool: 
 *     true:    succeeded.
 *     false:   failed.
 */
static bool TLM_XML_COM_RET_SUCCEEDED(TLM_XML_COM_RET_TYPE hr) {
    return ((hr) == TLM_XML_COM_RET_OK);
}

/**
* @brief Constructor entTlmXmlParser 
*
* @return N/A.
*
*/
entTlmXmlParser::entTlmXmlParser(const char * xmlContent)
    : m_xmlContent(new T_STRING(xmlContent))
    , m_domDoc(0)
    , m_isXmlLoaded(false)
{

}

/**
* @brief Destructor entTlmXmlParser 
*
* @return N/A.
*
*/
entTlmXmlParser::~entTlmXmlParser()
{
	if(m_xmlContent){
		delete m_xmlContent;
		m_xmlContent = 0;
	}
    if (m_domDoc) {
        IXMLDOMDocument* domDocT = m_domDoc;
        try {
            domDocT->Release();
        } catch (...) {
        }
        m_domDoc = 0;
    }
}

/**
* @brief Create a IXMLDOMDocument object. 
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlParser::E_ERR entTlmXmlParser::createDomDoc(void)
{
    E_ERR err = E_ERR_OK;
    TLM_XML_COM_RET_TYPE hr;

    if (err == E_ERR_OK) {
        hr = CoCreateInstance(
            __uuidof(DOMDocument30),
            0,
            CLSCTX_INPROC_SERVER,
            __uuidof(IXMLDOMDocument),
            reinterpret_cast<void**>(&m_domDoc)
        );
        if (!TLM_XML_COM_RET_SUCCEEDED(hr)) {
            err = E_ERR_CREATING_DOM_DOC_FAILED;
        }
    }
    
    IXMLDOMDocument* domDocT = m_domDoc;

    if (err == E_ERR_OK) {
        hr = domDocT->put_async(TLM_XML_VARIANT_FALSE);
        if (!TLM_XML_COM_RET_SUCCEEDED(hr)) {
            err = E_ERR_CREATING_DOM_DOC_FAILED;
        }
    }

    if (err == E_ERR_OK) {
        hr = domDocT->put_validateOnParse(TLM_XML_VARIANT_FALSE);
        if (!TLM_XML_COM_RET_SUCCEEDED(hr)) {
            err = E_ERR_CREATING_DOM_DOC_FAILED;
        }
    }

    if (err == E_ERR_OK) {
        hr = domDocT->put_resolveExternals(TLM_XML_VARIANT_FALSE);
        if (!TLM_XML_COM_RET_SUCCEEDED(hr)) {
            err = E_ERR_CREATING_DOM_DOC_FAILED;
        }
    }
    
    if (err != E_ERR_OK) {
        if (m_domDoc) {
            domDocT->Release();
            m_domDoc = 0;
        }
    }

    return err;
}

/**
* @brief Load a XML. 
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlParser::E_ERR entTlmXmlParser::loadXml(void)
{
    E_ERR err = E_ERR_OK;
    TLM_XML_COM_RET_TYPE hr;
    TLM_XML_VARIANT_BOOL isLoadOk = TLM_XML_VARIANT_FALSE;
    BSTR xmlContent = 0;
    
    IXMLDOMDocument* domDocT = m_domDoc;

    if (err == E_ERR_OK) {
        if (domDocT != 0) {
            xmlContent = entTlmXmlUtils::createBstrFromUtf8(*m_xmlContent);
            if (xmlContent != 0) {
                hr = domDocT->loadXML(xmlContent, &isLoadOk);
                if (TLM_XML_COM_RET_SUCCEEDED(hr) && isLoadOk == TLM_XML_VARIANT_TRUE) {
                    m_isXmlLoaded = true;
                    err = E_ERR_OK;
                } else {
                    err = E_ERR_LOADING_XML_FAILED;
                }
            } else {
                err = E_ERR_ALLOCATING_BSTR_FAILED;
            }
        } else {
            err = E_ERR_DOM_DOC_NOT_CREATED;
        }
    }
    
    if (xmlContent != 0) {
        entTlmXmlUtils::destroyBstr(xmlContent);
        xmlContent = 0;
    }

    return err;
}

/**
* @brief Query the matched item count for the given xpath. 
*
* @param xpath      [in]    The given xpath.
* @param count      [out]   The matched item count
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlParser::E_ERR entTlmXmlParser::queryNodeCountByXPath(const T_STRING &xpath, unsigned int &count)
{
    E_ERR err = E_ERR_OK;
    TLM_XML_COM_RET_TYPE hr;
    BSTR bsQuery = 0;
    IXMLDOMNodeList* pNodeList = 0;
    long nListLength = 0;
    
    IXMLDOMDocument* domDocT = m_domDoc;

    count = 0;

    if (err == E_ERR_OK) {
        if (domDocT == 0) {
            err = createDomDoc();
            if (err != E_ERR_OK) {
            }
        }
    }

    if (err == E_ERR_OK) {
        if (!m_isXmlLoaded) {
            err = loadXml();
            if (err != E_ERR_OK) {
            }
        }
    }

    if (err == E_ERR_OK) {
        bsQuery = entTlmXmlUtils::createBstrFromUtf8(xpath);
        if (bsQuery != 0) {
            IXMLDOMDocument* domDocT = m_domDoc;
            IXMLDOMNodeList* pNodeListT = pNodeList;
            BSTR bsQueryT = bsQuery;
            hr = domDocT->selectNodes(bsQueryT, &pNodeListT);
            pNodeList = pNodeListT;
            if (TLM_XML_COM_RET_SUCCEEDED(hr)) {
                hr = pNodeListT->get_length(&nListLength);
                if (TLM_XML_COM_RET_SUCCEEDED(hr)) {
                    count = static_cast<unsigned int>(nListLength);
                    err = E_ERR_OK;
                } else {
                    err = E_ERR_GETING_NODE_COUNT_FAILED;
                }
            } else {
                err = E_ERR_SELECTING_NODES_FAILED;
            }
        } else {
            err = E_ERR_ALLOCATING_BSTR_FAILED;
        }
    }
    
    if (bsQuery != 0) {
        entTlmXmlUtils::destroyBstr(bsQuery);
        bsQuery = 0;
    }
    if (pNodeList != 0) {
        pNodeList->Release();
        pNodeList = 0;
    }

    return err;
}

/**
* @brief Query the matched items for the given xpath. 
*
* @param xpath      [in]    The given xpath.
* @param retTexts   [out]   The matched items.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlParser::E_ERR entTlmXmlParser::getTextValuesByXPath(const T_STRING &xpath, T_STRINGS &retTexts)
{
    E_ERR err = E_ERR_OK;
    TLM_XML_COM_RET_TYPE hr;
    BSTR bsQuery = 0;
    IXMLDOMNodeList* pNodeList = 0;
    
    IXMLDOMDocument* domDocT = m_domDoc;

    //retTexts.clear();
    retTexts.erase( retTexts.begin(), retTexts.end() );

    if (err == E_ERR_OK) {
        if (domDocT == 0) {
            err = createDomDoc();
            if (err != E_ERR_OK) {
            }
        }
    }

    if (err == E_ERR_OK) {
        if (!m_isXmlLoaded) {
            err = loadXml();
            if (err != E_ERR_OK) {
            }
        }
    }

    if (err == E_ERR_OK) {
        bsQuery = entTlmXmlUtils::createBstrFromUtf8(xpath);
        if (bsQuery != 0) {
            IXMLDOMDocument* domDocT = m_domDoc;
            IXMLDOMNodeList* pNodeListT = pNodeList;
            BSTR bsQueryT = bsQuery;
            hr = domDocT->selectNodes(bsQueryT, &pNodeListT);
            pNodeList = pNodeListT;
            if (TLM_XML_COM_RET_SUCCEEDED(hr)) {
                // ok
            } else {
                err = E_ERR_SELECTING_NODES_FAILED;
            }
        } else {
            err = E_ERR_ALLOCATING_BSTR_FAILED;
        }
    }
    
    if (err == E_ERR_OK) {
        err = getTextValuesByXPath_GetAllNodes(pNodeList, retTexts);
    }
    
    if (bsQuery != 0) {
        entTlmXmlUtils::destroyBstr(bsQuery);
        bsQuery = 0;
    }
    if (pNodeList != 0) {
        pNodeList->Release();
        pNodeList = 0;
    }
    

    return err;
}

/**
* @brief Query the matched items for the given xpath. 
*
* @param xpath      [in]    The given xpath.
* @param retTexts   [out]   The matched items.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlParser::E_ERR entTlmXmlParser::getXmlValuesByXPath(const T_STRING &xpath, T_STRINGS &retTexts)
{
    E_ERR err = E_ERR_OK;
    TLM_XML_COM_RET_TYPE hr;
    BSTR bsQuery = 0;
    IXMLDOMNodeList* pNodeList = 0;
    
    IXMLDOMDocument* domDocT = m_domDoc;

    //retTexts.clear();
    retTexts.erase( retTexts.begin(), retTexts.end() );

    if (err == E_ERR_OK) {
        if (domDocT == 0) {
            err = createDomDoc();
            if (err != E_ERR_OK) {
            }
        }
    }

    if (err == E_ERR_OK) {
        if (!m_isXmlLoaded) {
            err = loadXml();
            if (err != E_ERR_OK) {
            }
        }
    }

    if (err == E_ERR_OK) {
        bsQuery = entTlmXmlUtils::createBstrFromUtf8(xpath);
        if (bsQuery != 0) {
            IXMLDOMDocument* domDocT = m_domDoc;
            IXMLDOMNodeList* pNodeListT = pNodeList;
            BSTR bsQueryT = bsQuery;
            hr = domDocT->selectNodes(bsQueryT, &pNodeListT);
            pNodeList = pNodeListT;
            if (TLM_XML_COM_RET_SUCCEEDED(hr)) {
                // ok
            } else {
                err = E_ERR_SELECTING_NODES_FAILED;
            }
        } else {
            err = E_ERR_ALLOCATING_BSTR_FAILED;
        }
    }
    
    if (err == E_ERR_OK) {
        err = getXmlValuesByXPath_GetAllNodes(pNodeList, retTexts);
    }
    
    if (bsQuery != 0) {
        entTlmXmlUtils::destroyBstr(bsQuery);
        bsQuery = 0;
    }
    if (pNodeList != 0) {
        pNodeList->Release();
        pNodeList = 0;
    }
    

    return err;
}

/**
* @brief Get all nodes for the given IXMLDOMNodeList object. 
*
* @param pNodeList  [in]    the given IXMLDOMNodeList object.
* @param retTexts   [out]   The matched items.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlParser::E_ERR entTlmXmlParser::getTextValuesByXPath_GetAllNodes(IXMLDOMNodeList* pNodeList, T_STRINGS &retTexts)
{
    E_ERR err = E_ERR_OK;
    TLM_XML_COM_RET_TYPE hr;
    IXMLDOMNode* pNode = 0;
    BSTR bsNodeText = 0;
    
    IXMLDOMNodeList* pNodeListT = pNodeList;
    
    while (err == E_ERR_OK) {
        hr = pNodeListT->nextNode(&pNode);
        if (hr == TLM_XML_COM_RET_FALSE && pNode == 0) {
            err = E_ERR_OK;
            break; // enumerating finished
        } else if (TLM_XML_COM_RET_SUCCEEDED(hr)) {
            err = getTextValuesByXPath_GetAllNodes_GetText(pNode, &bsNodeText);
            if (err == E_ERR_OK) {
                retTexts.push_back(entTlmXmlUtils::convertBstrToUtf8(bsNodeText));
                entTlmXmlUtils::destroyBstr(bsNodeText);
                bsNodeText = 0;
                IXMLDOMNode* pNodeT = pNode;
                pNodeT->Release();
                pNode = 0;
            } else {
            
            }
        } else {
            err = E_ERR_ENUMING_NODE_FAILED;
            break;
        }
    }
    
    if (pNode != 0) {
        pNode->Release();
        pNode = 0;
    }
    
    return err;
}

/**
* @brief Get all nodes for the given IXMLDOMNodeList object. 
*
* @param pNodeList  [in]    the given IXMLDOMNodeList object.
* @param retTexts   [out]   The matched items.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlParser::E_ERR entTlmXmlParser::getXmlValuesByXPath_GetAllNodes(IXMLDOMNodeList* pNodeList, T_STRINGS &retTexts)
{
    E_ERR err = E_ERR_OK;
    TLM_XML_COM_RET_TYPE hr;
    IXMLDOMNode* pNode = 0;
    BSTR bsNodeText = 0;
    
    IXMLDOMNodeList* pNodeListT = pNodeList;
    
    while (err == E_ERR_OK) {
        hr = pNodeListT->nextNode(&pNode);
        if (hr == TLM_XML_COM_RET_FALSE && pNode == 0) {
            err = E_ERR_OK;
            break; // enumerating finished
        } else if (TLM_XML_COM_RET_SUCCEEDED(hr)) {
            err = getXmlValuesByXPath_GetAllNodes_GetXml(pNode, &bsNodeText);
            if (err == E_ERR_OK) {
                retTexts.push_back(entTlmXmlUtils::convertBstrToUtf8(bsNodeText));
                entTlmXmlUtils::destroyBstr(bsNodeText);
                bsNodeText = 0;
                IXMLDOMNode* pNodeT = pNode;
                pNodeT->Release();
                pNode = 0;
            } else {
            
            }
        } else {
            err = E_ERR_ENUMING_NODE_FAILED;
            break;
        }
    }
    
    if (pNode != 0) {
        pNode->Release();
        pNode = 0;
    }
    
    return err;
}

/**
* @brief Get the text for the given IXMLDOMNode object. 
*
* @param pNode          [in]    the given IXMLDOMNode object.
* @param pbsNodeText    [out]   The text.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlParser::E_ERR entTlmXmlParser::getTextValuesByXPath_GetAllNodes_GetText(IXMLDOMNode* pNode, BSTR *pbsNodeText)
{
    E_ERR err = E_ERR_OK;
    TLM_XML_COM_RET_TYPE hr;
    
    if (pNode != 0) {
        hr = pNode->get_text(pbsNodeText);
        if (TLM_XML_COM_RET_SUCCEEDED(hr)) {
            
        } else {
            err = E_ERR_GETTING_NODE_VALUE_FAILED;
        }
    } else {
        err = E_ERR_GETTING_NODE_VALUE_FAILED;
    }
    
    return err;
}

/**
* @brief Get the text for the given IXMLDOMNode object. 
*
* @param pNode          [in]    the given IXMLDOMNode object.
* @param pbsNodeText    [out]   The text.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlParser::E_ERR entTlmXmlParser::getXmlValuesByXPath_GetAllNodes_GetXml(IXMLDOMNode* pNode, BSTR *pbsNodeText)
{
    E_ERR err = E_ERR_OK;
    TLM_XML_COM_RET_TYPE hr;
    
    if (pNode != 0) {
        hr = pNode->get_xml(pbsNodeText);
        if (TLM_XML_COM_RET_SUCCEEDED(hr)) {
            
        } else {
            err = E_ERR_GETTING_NODE_VALUE_FAILED;
        }
    } else {
        err = E_ERR_GETTING_NODE_VALUE_FAILED;
    }
    
    return err;
}
