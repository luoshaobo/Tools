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

//==============================================================================
// INCLUDES
//==============================================================================
#include "entTlmXmlMaker.h"
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
* @brief The DLL entrance.
*
* @param hinstDLL       [IN]    the HRESULT value
* @param dwReason       [IN]    the HRESULT value
* @param lpvReserved    [IN]    the HRESULT value
*
* @return  bool.
*     true:     is OK.
*     false:    is not OK.
*
* @note
*/
BOOL WINAPI DllMain(HANDLE hinstDLL, 
                      DWORD dwReason, 
                      LPVOID lpvReserved)
{
    switch(dwReason){
        case DLL_PROCESS_ATTACH:
            {
            }
            break;
        case DLL_THREAD_ATTACH:
            {
                CoInitializeEx(0, COINIT_MULTITHREADED);
            }
            break;
        case DLL_THREAD_DETACH:
            {
                CoUninitialize();
            }
            break;
        case DLL_PROCESS_DETACH:
        default:
            {
            }
            break;
        }
    return TRUE;
}

/**
* @brief Constructor entTlmXmlMaker 
*
* @return N/A.
*
*/
entTlmXmlMaker::entTlmXmlMaker() : 
    m_domDoc(0)
{

}

/**
* @brief Destructor entTlmXmlMaker 
*
* @return N/A.
*
*/
entTlmXmlMaker::~entTlmXmlMaker()
{
    if (m_domDoc) {
        try {
            m_domDoc->Release();
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
entTlmXmlMaker::E_ERR entTlmXmlMaker::createDomDoc(void)
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
        err = createDomDoc_SetDocumentProperties1();
    }
    
    if (err == E_ERR_OK) {
        err = createDomDoc_SetDocumentProperties2();
    }

    if (err != E_ERR_OK) {
        if (m_domDoc) {
            m_domDoc->Release();
            m_domDoc = 0;
        }
    }
    
    return err;
}

/**
* @brief Set the properties for the IXMLDOMDocument object (part 1). 
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlMaker::E_ERR entTlmXmlMaker::createDomDoc_SetDocumentProperties1(void) const
{
    E_ERR err = E_ERR_OK;
    TLM_XML_COM_RET_TYPE hr;
    
    IXMLDOMDocument* domDocT = m_domDoc;
    
    if (err == E_ERR_OK) {
        hr = static_cast<TLM_XML_COM_RET_TYPE>(domDocT->put_async(TLM_XML_VARIANT_FALSE));
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
    
    return err;
}

/**
* @brief Set the properties for the IXMLDOMDocument object (part 2). 
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlMaker::E_ERR entTlmXmlMaker::createDomDoc_SetDocumentProperties2(void)
{
    E_ERR err = E_ERR_OK;
    TLM_XML_COM_RET_TYPE hr;
    
    BSTR bsXml = 0;
    BSTR bsXmlVersion = 0;
    IXMLDOMProcessingInstruction* pi=0;
    
    IXMLDOMDocument* domDocT = m_domDoc;
    
    if (err == E_ERR_OK) {
        bsXml = entTlmXmlUtils::createBstrFromUtf8("xml");
        bsXmlVersion = entTlmXmlUtils::createBstrFromUtf8("version=\'1.0\'");
        if (bsXml != 0 && bsXmlVersion != 0) {
            hr = domDocT->createProcessingInstruction(bsXml, bsXmlVersion, &pi);
            if (TLM_XML_COM_RET_SUCCEEDED(hr)) {
                err = appendChildToParent(pi, domDocT);
                if (err == E_ERR_OK) {
                } else {
                }
            } else {
                err = E_ERR_CREATING_INSTRUCTION_FAILED;
            }
        } else {
            err = E_ERR_ALLOCATING_BSTR_FAILED;
        }
    }
    
    if (bsXml != 0) {
        entTlmXmlUtils::destroyBstr(bsXml);
        bsXml = 0;
    }
    if (bsXmlVersion != 0) {
        entTlmXmlUtils::destroyBstr(bsXmlVersion);
        bsXml = 0;
    }
    if (pi != 0) {
        pi->Release();
        pi = 0;
    }
    
    return err;
}

/**
* @brief Append a child node to the given parent node. 
*
* @param pChild     [in]    The child node.
* @param pParent    [in]    The parent node.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlMaker::E_ERR entTlmXmlMaker::appendChildToParent(IXMLDOMNode *pChild, IXMLDOMNode *pParent)
{
    E_ERR err = E_ERR_OK;
    TLM_XML_COM_RET_TYPE hr;
    IXMLDOMNode* pNode = 0;
    
    IXMLDOMNode* pParentT = pParent;

    if (err == E_ERR_OK) {
        if (pChild == 0 || pParent == 0) {
            err = E_ERR_ARGUMENT_INVALID;
        }
    }

    if (err == E_ERR_OK) {
        hr = pParentT->appendChild(pChild, &pNode);
        if (TLM_XML_COM_RET_SUCCEEDED(hr)) {
            err = E_ERR_OK;
        } else {
            err = E_ERR_APPENDING_CHILD_TO_PARENT_FAILED;
        }
    }
    
    if (pNode != 0) {
        pNode->Release();
        pNode=0;
    }

    return err;
}

/**
* @brief Append a attribute to the given element. 
*
* @param pAtrribute [in]    The attribue.
* @param pElement   [in]    The element.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlMaker::E_ERR entTlmXmlMaker::appendAtrributeToElement(IXMLDOMAttribute* pAtrribute, IXMLDOMElement* pElement)
{
    E_ERR err = E_ERR_OK;
    TLM_XML_COM_RET_TYPE hr;
    IXMLDOMAttribute* pAttributeRet = 0;
    
    IXMLDOMElement* pElementT = pElement;

    if (err == E_ERR_OK) {
        if (pAtrribute == 0 || pElement == 0) {
            err = E_ERR_ARGUMENT_INVALID;
        }
    }

    if (err == E_ERR_OK) {
        if (m_domDoc == 0) {
            err = createDomDoc();
            if (err != E_ERR_OK) {
            }
        }
    }

    if (err == E_ERR_OK) {
        hr = pElementT->setAttributeNode(pAtrribute, &pAttributeRet);
        if (TLM_XML_COM_RET_SUCCEEDED(hr)) {
            err = E_ERR_OK;
        } else {
            err = E_ERR_SETTING_ELEMENT_ATTRIBUTE_FAILED;
        }
    }
    
    if (pAttributeRet != 0) {
        pAttributeRet->Release();
        pAttributeRet = 0;
    }

    return err;
}

/**
* @brief Create a IXMLDOMElement object. 
*
* @param elementName    [in]    The name of the element.
* @param elementValue   [in]    The text of the element.
* @param ppElement      [out]   The returned element.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlMaker::E_ERR entTlmXmlMaker::createXmlElement(const T_STRING& elementName, const T_STRING& elementValue, IXMLDOMElement **ppElement)
{
    E_ERR err = E_ERR_OK;
    TLM_XML_COM_RET_TYPE hr;
    IXMLDOMElement *pElementNode = 0;
    
    
    BSTR bsElementName = 0;
    
    IXMLDOMElement **ppElementT = ppElement;
    IXMLDOMDocument* domDocT = m_domDoc;

    if (err == E_ERR_OK) {
        if (ppElement == 0) {
            err = E_ERR_ARGUMENT_INVALID;
        }
    }

    if (err == E_ERR_OK) {
        *ppElementT = 0;

        if (m_domDoc == 0) {
            err = createDomDoc();
            if (err != E_ERR_OK) {
            }
        }
    }

    if (err == E_ERR_OK) {
        bsElementName = entTlmXmlUtils::createBstrFromUtf8(elementName);
        if (bsElementName != 0) {
            hr = domDocT->createElement(bsElementName, &pElementNode);
            if (TLM_XML_COM_RET_SUCCEEDED(hr)) {
                err = createXmlElement_Part2(elementName, elementValue, pElementNode, ppElement);
                if (err == E_ERR_OK) {
                } else {
                }
            } else {
                err = E_ERR_CREATING_NODE_FAILED;
            }
        } else {
            err = E_ERR_ALLOCATING_BSTR_FAILED;
        }
    }
    
    if (ppElement != 0) {
        *ppElement = pElementNode;
    }

    if (err != E_ERR_OK) {
        if (pElementNode != 0) {
            pElementNode->Release();
            pElementNode = 0;
        }
    }
    
    if (bsElementName != 0) {
        entTlmXmlUtils::destroyBstr(bsElementName);
        bsElementName = 0;
    }
    
    return err;
}

/**
* @brief Create a IXMLDOMElement object (part 2). 
*
* @param elementName    [in]    The name of the element.
* @param elementValue   [in]    The text of the element.
* @param pElementNode   [in]    The element object.
* @param ppElement      [out]   The returned element.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlMaker::E_ERR entTlmXmlMaker::createXmlElement_Part2(const T_STRING& elementName, const T_STRING& elementValue, IXMLDOMElement *pElementNode, IXMLDOMElement **ppElement) const
{
    E_ERR err = E_ERR_OK;
    TLM_XML_COM_RET_TYPE hr;
    BSTR bsElementValue = 0;
    IXMLDOMText *pTextEelement = 0;
    IXMLDOMElement *pElement = 0;
    IXMLDOMNode *pTextEelementRet = 0;
    
    IXMLDOMDocument* domDocT = m_domDoc;
    
    if (elementValue.length() > 0) {
        bsElementValue = entTlmXmlUtils::createBstrFromUtf8(elementValue);
        if (bsElementValue != 0) {
            hr = domDocT->createTextNode(bsElementValue, &pTextEelement);
            if (TLM_XML_COM_RET_SUCCEEDED(hr)) {
                IXMLDOMElement *pElementNodeT = pElementNode;
                hr = pElementNodeT->appendChild(pTextEelement, &pTextEelementRet);
                if (TLM_XML_COM_RET_SUCCEEDED(hr)) {
                    pElement = pElementNode;
                    err = E_ERR_OK;
                } else {
                    err = E_ERR_APPENDING_CHILD_FAILED;
                }
            } else {
                err = E_ERR_CREATING_TEXT_NODE_FAILED;
            }
        } else {
            err = E_ERR_ALLOCATING_BSTR_FAILED;
        }
    } else {
        pElement = pElementNode;
        err = E_ERR_OK;
    }
    
    if (ppElement != 0) {
        *ppElement = pElement;
    }
    
    if (bsElementValue != 0) {
        entTlmXmlUtils::destroyBstr(bsElementValue);
        bsElementValue = 0;
    }
    if (pTextEelement != 0) {
        pTextEelement->Release();
        pTextEelement = 0;
    }
    if (pTextEelementRet != 0) {
        pTextEelementRet->Release();
        pTextEelementRet = 0;
    }
    
    return err;
}

/**
* @brief Create a IXMLDOMAttribute object. 
*
* @param attrElementName        [in]    The name of the attribute.
* @param attrElementValue       [in]    The text of the attribute.
* @param ppAttrElement          [out]   The returned attribue.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlMaker::E_ERR entTlmXmlMaker::createXmlAtributeElement(const T_STRING& attrElementName, const T_STRING& attrElementValue, IXMLDOMAttribute **ppAttrElement)
{
    E_ERR err = E_ERR_OK;
    TLM_XML_COM_RET_TYPE hr;
    IXMLDOMAttribute *pAttrElementNode = 0;
    BSTR bsAttrElementName = 0;
    
    IXMLDOMAttribute **ppAttrElementT = ppAttrElement;
    IXMLDOMDocument* domDocT = m_domDoc;

    if (err == E_ERR_OK) {
        if (ppAttrElement == 0) {
            err = E_ERR_ARGUMENT_INVALID;
        }
    }

    if (err == E_ERR_OK) {
        *ppAttrElementT = 0;

        if (m_domDoc == 0) {
            err = createDomDoc();
            if (err != E_ERR_OK) {
            }
        }
    }

    if (err == E_ERR_OK) {
        bsAttrElementName = entTlmXmlUtils::createBstrFromUtf8(attrElementName);
        if (bsAttrElementName != 0) {
            hr = domDocT->createAttribute(bsAttrElementName, &pAttrElementNode);
            if (TLM_XML_COM_RET_SUCCEEDED(hr)) {
                err = createXmlAtributeElement_Part2(attrElementValue, pAttrElementNode, ppAttrElement);
                if (err == E_ERR_OK) {
                
                } else {
                
                }
            } else {
                err = E_ERR_CREATING_ATTRIBUTE_NODE_FAILED;
            }
        } else {
            err = E_ERR_ALLOCATING_BSTR_FAILED;
        }
    }
    
    if (ppAttrElement != 0) {
        *ppAttrElementT = pAttrElementNode;
    }

    if (err != E_ERR_OK) {
        if (pAttrElementNode != 0) {
            pAttrElementNode->Release();
            pAttrElementNode = 0;
        }
    }
    
    if (bsAttrElementName != 0) {
        entTlmXmlUtils::destroyBstr(bsAttrElementName);
        bsAttrElementName = 0;
    }
    

    return err;
}

/**
* @brief Create a IXMLDOMAttribute object (part 2). 
*
* @param attrElementName        [in]    The name of the attribute.
* @param attrElementValue       [in]    The text of the attribute.
* @param ppAttrElement          [out]   The returned attribue.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlMaker::E_ERR entTlmXmlMaker::createXmlAtributeElement_Part2(const T_STRING& attrElementValue, IXMLDOMAttribute *pAttrElementNode, IXMLDOMAttribute **ppAttrElement) const
{
    E_ERR err = E_ERR_OK;
    TLM_XML_COM_RET_TYPE hr;
    BSTR bsAttrElementValue = 0;
    IXMLDOMAttribute *pAttrElement = 0;
    
    IXMLDOMDocument* domDocT = m_domDoc;
    
    bsAttrElementValue = entTlmXmlUtils::createBstrFromUtf8(attrElementValue);
    if (bsAttrElementValue != 0) {
        VARIANT varAttrElementValue = {0};
        VariantInit(&varAttrElementValue);
        V_BSTR(&varAttrElementValue) = bsAttrElementValue;
        V_VT(&varAttrElementValue) = VT_BSTR;
        IXMLDOMAttribute *pAttrElementNodeT = pAttrElementNode;
        hr = pAttrElementNodeT->put_value(varAttrElementValue);
        if (TLM_XML_COM_RET_SUCCEEDED(hr)) {
            pAttrElement = pAttrElementNode;
            err = E_ERR_OK;
        } else {
            err = E_ERR_SETTING_TEXT_NODE_VALUE_FAILED;
        }
    } else {
        err = E_ERR_ALLOCATING_BSTR_FAILED;
    }
    
    if (ppAttrElement != 0) {
        IXMLDOMAttribute **ppAttrElementT = ppAttrElement;
        *ppAttrElementT = pAttrElement;
    }
    
    
    if (bsAttrElementValue != 0) {
        entTlmXmlUtils::destroyBstr(bsAttrElementValue);
        bsAttrElementValue = 0;
    }
    
    return err;
}

/**
* @brief Add a root node to the document. 
*
* @param rootNodeName   [in]    The name of the root node.
* @param rootNode       [out]   The returned root node.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlMaker::E_ERR entTlmXmlMaker::addRootNode(const T_STRING& rootNodeName, T_NODE* rootNode)
{
    E_ERR err = E_ERR_OK;
    IXMLDOMElement* pRootElement = 0;
    
    T_NODE* rootNodeT = rootNode;

    if (err == E_ERR_OK) {
        if (rootNode == 0) {
            err = E_ERR_ARGUMENT_INVALID;
        }
    }

    if (err == E_ERR_OK) {
        *rootNodeT = 0;

        if (m_domDoc == 0) {
            err = createDomDoc();
            if (err != E_ERR_OK) {
            }
        }
    }

    if (err == E_ERR_OK) {
        err = createXmlElement(rootNodeName, "", &pRootElement);
        if (err == E_ERR_OK) {
            err = appendChildToParent(pRootElement, m_domDoc);
            if (err == E_ERR_OK) {
                *rootNodeT = pRootElement;
            } else {
            }
        } else {
        }
    }

    if (err != E_ERR_OK) {
        if (pRootElement != 0) {
            IXMLDOMElement* pRootElementT = pRootElement;
            pRootElementT->Release();
            pRootElement = 0;
        }
    }
    
    return err;
}

/**
* @brief Add a root node to the document. 
*
* @param rootNodeName   [in]    The name of the root node.
* @param rootNodeValue  [in]    The value of the root node.
* @param rootNode       [out]   The returned root node.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlMaker::E_ERR entTlmXmlMaker::addRootNode(const T_STRING& rootNodeName, const T_STRING& rootNodeValue, T_NODE* rootNode)
{
    E_ERR err = E_ERR_OK;
    IXMLDOMElement* pRootElement = 0;
    
    T_NODE* rootNodeT = rootNode;

    if (err == E_ERR_OK) {
        if (rootNode == 0) {
            err = E_ERR_ARGUMENT_INVALID;
        }
    }

    if (err == E_ERR_OK) {
        *rootNodeT = 0;

        if (m_domDoc == 0) {
            err = createDomDoc();
            if (err != E_ERR_OK) {
            }
        }
    }

    if (err == E_ERR_OK) {
        err = createXmlElement(rootNodeName, rootNodeValue, &pRootElement);
        if (err == E_ERR_OK) {
            err = appendChildToParent(pRootElement, m_domDoc);
            if (err == E_ERR_OK) {
                *rootNodeT = pRootElement;
            } else {
            }
        } else {
        }
    }

    if (err != E_ERR_OK) {
        if (pRootElement != 0) {
            IXMLDOMElement* pRootElementT = pRootElement;
            pRootElementT->Release();
            pRootElement = 0;
        }
    }
    
    return err;
}

/**
* @brief Add a child node to the given parent node. 
*
* @param parentNode     [in]    The parent node.
* @param childNodeName  [in]    The name of the child node.
* @param childNode      [out]   The returned child node.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlMaker::E_ERR entTlmXmlMaker::addChildNode(const T_NODE& parentNode, const T_STRING& childNodeName, T_NODE* childNode)
{
    E_ERR err = E_ERR_OK;
    IXMLDOMElement* pChildElement = 0;
    
    T_NODE* childNodeT = childNode;

    if (err == E_ERR_OK) {
        if (childNode == 0) {
            err = E_ERR_ARGUMENT_INVALID;
        }
    }

    if (err == E_ERR_OK) {
        *childNodeT = 0;

        if (m_domDoc == 0) {
            err = createDomDoc();
            if (err != E_ERR_OK) {
            }
        }
    }

    if (err == E_ERR_OK) {
        err = createXmlElement(childNodeName, "", &pChildElement);
        if (err == E_ERR_OK) {
            err = appendChildToParent(pChildElement, static_cast<IXMLDOMNode*>(parentNode));
            if (err == E_ERR_OK) {
                *childNodeT = pChildElement;
            } else {
            }
        } else {
        }
    }

    if (err != E_ERR_OK) {
        if (pChildElement != 0) {
            IXMLDOMElement* pChildElementT = pChildElement;
            pChildElementT->Release();
            pChildElement = 0;
        }
    }
    
    return err;
}

/**
* @brief Add a child node to the given parent node. 
*
* @param parentNode     [in]    The parent node.
* @param childNodeName  [in]    The name of the child node.
* @param childNodeValue [in]    The value of the child node.
* @param childNode      [out]   The returned child node.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlMaker::E_ERR entTlmXmlMaker::addChildNode(const T_NODE& parentNode, const T_STRING& childNodeName, const T_STRING& childNodeValue, T_NODE* childNode)
{
    E_ERR err = E_ERR_OK;
    IXMLDOMElement* pChildElement = 0;
    
    T_NODE* childNodeT = childNode;

    if (err == E_ERR_OK) {
        if (childNode == 0) {
            err = E_ERR_ARGUMENT_INVALID;
        }
    }

    if (err == E_ERR_OK) {
        *childNodeT = 0;

        if (m_domDoc == 0) {
            err = createDomDoc();
            if (err != E_ERR_OK) {
            }
        }
    }

    if (err == E_ERR_OK) {
        err = createXmlElement(childNodeName, childNodeValue, &pChildElement);
        if (err == E_ERR_OK) {
            err = appendChildToParent(pChildElement, static_cast<IXMLDOMNode*>(parentNode));
            if (err == E_ERR_OK) {
                *childNodeT = pChildElement;
            } else {
            }
        } else {
        }
    }

    if (err != E_ERR_OK) {
        if (pChildElement != 0) {
            IXMLDOMElement* pChildElementT = pChildElement;
            pChildElementT->Release();
            pChildElement = 0;
        }
    }
    
    return err;
}

/**
* @brief Set a text value to a node. 
*
* @param node       [in]    The node.
* @param value      [in]    The text value.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlMaker::E_ERR entTlmXmlMaker::setNodeValue(const T_NODE& node, const T_STRING& value)
{
    E_ERR err = E_ERR_OK;
    TLM_XML_COM_RET_TYPE hr;
    IXMLDOMElement* pElementNode = static_cast<IXMLDOMElement* >(node);
    IXMLDOMText* pTextEelement = 0;
    IXMLDOMNode* pTextEelementRet = 0;
    BSTR bsElementValue = 0;
    
    IXMLDOMDocument* domDocT = m_domDoc;

    if (err == E_ERR_OK) {
        if (node == 0) {
            err = E_ERR_ARGUMENT_INVALID;
        }
    }

    if (err == E_ERR_OK) {
        if (m_domDoc == 0) {
            err = createDomDoc();
            if (err != E_ERR_OK) {
            }
        }
    }
    
    if (err == E_ERR_OK) {
        bsElementValue = entTlmXmlUtils::createBstrFromUtf8(value);
        if (bsElementValue != 0) {
            hr = domDocT->createTextNode(bsElementValue, &pTextEelement);
            if (TLM_XML_COM_RET_SUCCEEDED(hr)) {
                hr = pElementNode->appendChild(pTextEelement, &pTextEelementRet);
                if (TLM_XML_COM_RET_SUCCEEDED(hr)) {
                    err = E_ERR_OK;
                } else {
                    err = E_ERR_APPENDING_CHILD_FAILED;
                }
            } else {
                err = E_ERR_CREATING_TEXT_NODE_FAILED;
            }
        } else {
            err = E_ERR_ALLOCATING_BSTR_FAILED;
        }
    }
    
    if (bsElementValue != 0) {
        entTlmXmlUtils::destroyBstr(bsElementValue);
        bsElementValue = 0;
    }
    if (pTextEelement != 0) {
        IXMLDOMText* pTextEelementT = pTextEelement;
        pTextEelementT->Release();
        pTextEelement = 0;
    }
    if (pTextEelementRet != 0) {
        IXMLDOMNode* pTextEelementRetT = pTextEelementRet;
        pTextEelementRetT->Release();
        pTextEelementRet = 0;
    }

    return err;
}

/**
* @brief Set an attribute to a node. 
*
* @param node       [in]    The node.
* @param attrName   [in]    The attribute name.
* @param attrValude [in]    The attribute value.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlMaker::E_ERR entTlmXmlMaker::setNodeAtrribute(const T_NODE& node, const T_STRING& attrName, const T_STRING& attrValude)
{
    E_ERR err = E_ERR_OK;
    IXMLDOMElement* pElementNode = static_cast<IXMLDOMElement* >(node);
    IXMLDOMAttribute* pAttrElement = 0;

    if (err == E_ERR_OK) {
        if (node == 0) {
            err = E_ERR_ARGUMENT_INVALID;
        }
    }

    if (err == E_ERR_OK) {
        if (m_domDoc == 0) {
            err = createDomDoc();
            if (err != E_ERR_OK) {
            }
        }
    }

    if (err == E_ERR_OK) {
        err = createXmlAtributeElement(attrName, attrValude, &pAttrElement);
        if (err == E_ERR_OK) {
            err = appendAtrributeToElement(pAttrElement, pElementNode);
            if (err == E_ERR_OK) {
            } else {
            }
        } else {
        }
    }
    
    if (pAttrElement != 0) {
        IXMLDOMAttribute* pAttrElementT = pAttrElement;
        pAttrElementT->Release();
        pAttrElement = 0;
    }

    return err;
}

/**
* @brief Make the XML content from the document. 
*
* @param xmlContent     [out]    The XML content.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlMaker::E_ERR entTlmXmlMaker::makeXmlContent(T_STRING& xmlContent)
{
    E_ERR err = E_ERR_OK;
    TLM_XML_COM_RET_TYPE hr;
    BSTR bsXml = 0;
    
    IXMLDOMDocument* domDocT = m_domDoc;

    //xmlContent.clear();
    xmlContent.erase( xmlContent.begin(), xmlContent.end() );
    
    if (err == E_ERR_OK) {
        if (domDocT == 0) {
            err = createDomDoc();
            if (err != E_ERR_OK) {
            }
        }
    }

    if (err == E_ERR_OK) {
        IXMLDOMDocument* domDocT = m_domDoc;
        BSTR bsXmlT;
        hr = domDocT->get_xml(&bsXmlT);
        if (TLM_XML_COM_RET_SUCCEEDED(hr)) {
            bsXml = bsXmlT;
            xmlContent = entTlmXmlUtils::convertBstrToUtf8(bsXml);
            fixEncodingInXmlContent(xmlContent);
            err = E_ERR_OK;
        } else {
            err = E_ERR_GETTING_XML_FAILED;
        }
    }
    
    if (bsXml != 0) {
        entTlmXmlUtils::destroyBstr(bsXml);
        bsXml = 0;
    }

    return err;
}

/**
* @brief Make the formated XML content from the document. 
*
* @param xmlContent     [out]    The formated XML content.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlMaker::E_ERR entTlmXmlMaker::makeFormatedXmlContent(T_STRING& xmlContent)
{
    E_ERR err = E_ERR_OK;
    TLM_XML_COM_RET_TYPE hr;
    VARIANT vXmlConent = {0};
    BSTR bsEncoding = 0;
    ISAXContentHandler* pSAXContentHandler = 0;
    IMXWriter* pMXWriter = 0;

    //xmlContent.clear();
	
	xmlContent.erase( xmlContent.begin(), xmlContent.end() );

    if (err == E_ERR_OK) {
        if (m_domDoc == 0) {
            err = createDomDoc();
            if (err != E_ERR_OK) {
            }
        }
    }
    
    if (err == E_ERR_OK) {
        hr = CoCreateInstance(
            __uuidof(MXXMLWriter),
            0,
            CLSCTX_INPROC_SERVER,
            __uuidof(IMXWriter),
            reinterpret_cast<void**>(&pMXWriter)
        );
        if (!TLM_XML_COM_RET_SUCCEEDED(hr)) {
            err = E_ERR_CREATING_COM_FAILED;
        }
    }
    
    IMXWriter* pMXWriterT = pMXWriter;

    if (err == E_ERR_OK) {
        bsEncoding = entTlmXmlUtils::createBstrFromUtf8("UTF-8");
        if (bsEncoding == 0) {
            err = E_ERR_ALLOCATING_BSTR_FAILED;
        }
    }

    if (err == E_ERR_OK) {
        err = makeFormatedXmlContent_SetWriterProperties(pMXWriter, bsEncoding, &pSAXContentHandler);
    }
    
    if (err == E_ERR_OK) {
        err = makeFormatedXmlContent_SetReaderProperties(pSAXContentHandler);
    }

    if (err == E_ERR_OK) {
        VariantInit(&vXmlConent);
        hr = pMXWriterT->get_output(&vXmlConent);
        if (!TLM_XML_COM_RET_SUCCEEDED(hr)) {
            err = E_ERR_PARSING_XML_FAILED;
        }
    }
    
    if (err == E_ERR_OK) {
        xmlContent = entTlmXmlUtils::convertBstrToUtf8(vXmlConent.bstrVal);

        fixEncodingInXmlContent(xmlContent);
    }
    
    makeFormatedXmlContent_FreeResources(pMXWriter, vXmlConent, bsEncoding, pSAXContentHandler);
    
    return err;
}

/**
* @brief Free resources. 
*
* @param pMXWriter              [inout]    The IMXWriter object.
* @param vXmlConent             [inout]    The XML content.
* @param bsEncoding             [inout]    The encoding.
* @param pSAXContentHandler     [inout]    The ISAXContentHandler object.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
void entTlmXmlMaker::makeFormatedXmlContent_FreeResources(IMXWriter*& pMXWriter, VARIANT &vXmlConent, BSTR& bsEncoding, ISAXContentHandler*& pSAXContentHandler)
{
    if (pMXWriter != 0) {
        pMXWriter->Release();
        pMXWriter = 0;
    }
    if (vXmlConent.bstrVal != 0) {
        entTlmXmlUtils::destroyBstr(vXmlConent.bstrVal);
        vXmlConent.bstrVal = 0;
    }
    if (bsEncoding != 0) {
        entTlmXmlUtils::destroyBstr(bsEncoding);
        bsEncoding = 0;
    }
    if (pSAXContentHandler != 0) {
        pSAXContentHandler->Release();
        pSAXContentHandler = 0;
    }
}

/**
* @brief Set properties for the IMXWriter object. 
*
* @param pMXWriter              [in]    The IMXWriter object.
* @param bsEncoding             [in]    The encoding.
* @param pSAXContentHandler     [in]    The ISAXContentHandler object.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlMaker::E_ERR entTlmXmlMaker::makeFormatedXmlContent_SetWriterProperties(IMXWriter* pMXWriter, BSTR bsEncoding, ISAXContentHandler** ppSAXContentHandler)
{
    E_ERR err = E_ERR_OK;
    TLM_XML_COM_RET_TYPE hr;
    ISAXContentHandler* pSAXContentHandler = 0;
    
    IMXWriter* pMXWriterT = pMXWriter;
    
    if (err == E_ERR_OK) {
        hr = pMXWriterT->put_indent(TLM_XML_VARIANT_TRUE);
        if (!TLM_XML_COM_RET_SUCCEEDED(hr)) {
            err = E_ERR_SETTING_MXWRITER_FAILED;
        }
    }
    if (err == E_ERR_OK) {
        hr = pMXWriterT->put_encoding(bsEncoding);
        if (!TLM_XML_COM_RET_SUCCEEDED(hr)) {
            err = E_ERR_SETTING_MXWRITER_FAILED;
        }
    }
    if (err == E_ERR_OK) {
        hr = pMXWriterT->put_standalone(TLM_XML_VARIANT_TRUE);
        if (!TLM_XML_COM_RET_SUCCEEDED(hr)) {
            err = E_ERR_SETTING_MXWRITER_FAILED;
        }
    }
    if (err == E_ERR_OK) {
        hr = pMXWriterT->QueryInterface(__uuidof(ISAXContentHandler), reinterpret_cast<void **>(&pSAXContentHandler));
        if (!TLM_XML_COM_RET_SUCCEEDED(hr)) {
            err = E_ERR_QUERYING_COM_FAILED;
        }
    }
    
    if (pSAXContentHandler != 0) {
        *ppSAXContentHandler = pSAXContentHandler;
    }
    
    return err;
}

/**
* @brief Set properties for the ISAXXMLReader object. 
*
* @param pSAXContentHandler     [in]    The ISAXContentHandler object.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlMaker::E_ERR entTlmXmlMaker::makeFormatedXmlContent_SetReaderProperties(ISAXContentHandler* pSAXContentHandler) const
{
    E_ERR err = E_ERR_OK;
    TLM_XML_COM_RET_TYPE hr;
    VARIANT vTemp;
    ISAXXMLReader* pMXReader = 0;
    
    if (err == E_ERR_OK) {
        hr = CoCreateInstance(
            __uuidof(SAXXMLReader),
            0,
            CLSCTX_INPROC_SERVER,
            __uuidof(ISAXXMLReader),
            reinterpret_cast<void**>(&pMXReader)
        );
        if (!TLM_XML_COM_RET_SUCCEEDED(hr)) {
            err = E_ERR_CREATING_COM_FAILED;
        }
    }
    
    ISAXXMLReader* pMXReaderT = pMXReader;
    
    if (err == E_ERR_OK) {
        hr = pMXReaderT->putContentHandler(pSAXContentHandler);
        if (!TLM_XML_COM_RET_SUCCEEDED(hr)) {
            err = E_ERR_SETTING_MXREADER_FAILED;
        }
    }

    if (err == E_ERR_OK) {
        VariantInit(&vTemp);
        vTemp.vt = VT_DISPATCH;
        vTemp.pdispVal = m_domDoc;
        hr = pMXReaderT->parse(vTemp);
        if (!TLM_XML_COM_RET_SUCCEEDED(hr)) {
            err = E_ERR_PARSING_XML_FAILED;
        }
    }
    
    if (pMXReader != 0) {
        pMXReaderT->Release();
        pMXReader = 0;
    }
    
    return err;
}

/**
* @brief Fix some bug for the generated XML string. 
*
* @param xmlContent     [inout]    The XML string.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
void entTlmXmlMaker::fixEncodingInXmlContent(T_STRING& xmlContent)
{
    std::string::size_type nPos;
    std::string sSpecWithoutEncoding = "<?xml version=\"1.0\"?>";
    std::string sSpecWithoutEncodingFixed = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    std::string sSpecWithEncodingUtf16 = "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>";
    std::string sSpecWithEncodingUtf16Fixed = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

    nPos = xmlContent.find(sSpecWithoutEncoding);
    if (nPos == 0) {
        xmlContent.replace(xmlContent.begin(), xmlContent.begin() + sSpecWithoutEncoding.length(), 
            sSpecWithoutEncodingFixed.begin(), sSpecWithoutEncodingFixed.end());
    }

    nPos = xmlContent.find(sSpecWithEncodingUtf16);
    if (nPos == 0) {
        xmlContent.replace(xmlContent.begin(), xmlContent.begin() + sSpecWithEncodingUtf16.length(), 
            sSpecWithEncodingUtf16Fixed.begin(), sSpecWithEncodingUtf16Fixed.end());
    }
}

/**
* @brief Release a node. 
*
* @param node     [in]    The node.
*
* @return  entTlmXmlParser::E_ERR.
*
* @note
*/
entTlmXmlMaker::E_ERR entTlmXmlMaker::releaseNode(const T_NODE& node)
{
    E_ERR err = E_ERR_OK;
    IXMLDOMElement* pElementNode = static_cast<IXMLDOMElement* >(node);

    if (err == E_ERR_OK) {
        if (node == 0) {
            err = E_ERR_ARGUMENT_INVALID;
        }
    }

    if (err == E_ERR_OK) {
        if (m_domDoc == 0) {
            err = createDomDoc();
            if (err != E_ERR_OK) {
            }
        }
    }

    if (err == E_ERR_OK) {
        pElementNode->Release();
    }

    return err;
}
