/*
 * Copyright (C) 2016 Delphi
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20150901-1_IHU_SW-amendment_signed.pdf agreement
 * between Volvo Cars and Delphi.
 *
 * Sublicensed by Volvo Car Corporation to Continental Automotive GmbH
 * only to be used in Volvo Car Corporation projects.
 *
 * Changes to this file are to be categorized as Buyer Proprietary as
 * described in "Attachment IV 20160322-1_SW-amendment TCAM.pdf"
 * between Volvo Car Corporation and Continental Automotive GmbH.
 * Unless required by applicable law or agreed to in writing, the software
 * is sublicensed to Continental Automotive GmbH on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either expressed
 * or implied.
 */

#include "xml.hpp"

#include <cstdlib>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <dlt.h>

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace xmlif {


std::string _GetElementData(xmlDocPtr doc,
                            std::string nsPrefix,
                            std::string nsURI,
                            std::string xpathExpr)
{
    xmlXPathObjectPtr xpathObj;
    xmlNodePtr xmlNode;
    xmlXPathContextPtr xpathCtx;

    xpathCtx = xmlXPathNewContext(doc);

    //libxml2, xpath and default namespaces needs explicit handling
    if (!nsPrefix.empty() && !nsURI.empty())
    {
        if (xmlXPathRegisterNs(xpathCtx,
                               (xmlChar*)nsPrefix.c_str(),
                               (xmlChar*)nsURI.c_str())
            != 0)
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Unable to register namespace");
            xmlXPathFreeContext(xpathCtx);
            return "";
        }
    }

    xpathObj = xmlXPathEvalExpression((xmlChar*)xpathExpr.c_str(), xpathCtx);
    if (xpathObj == NULL)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Unable to evalute xpath");
        xmlXPathFreeContext(xpathCtx);
        return "";
    }

    if (xpathObj->nodesetval == NULL)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "no nodesetval");
        xmlXPathFreeObject(xpathObj);
        xmlXPathFreeContext(xpathCtx);
        return "";
    }

    if (xpathObj->nodesetval->nodeTab == NULL)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "no nodetab");
        xmlXPathFreeObject(xpathObj);
        xmlXPathFreeContext(xpathCtx);
        return "";
    }

    if (xpathObj->nodesetval->nodeNr != 1)
    {
        //TODO: is this good?
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Got != 1 number of nodes (%d)", xpathObj->nodesetval->nodeNr);
    }

    xmlNode = xpathObj->nodesetval->nodeTab[0];

    if (xmlNode == NULL)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Unable to find a Node");
        xmlXPathFreeObject(xpathObj);
        xmlXPathFreeContext(xpathCtx);
        return "";
    }

    xmlChar *content = xmlNodeListGetString(doc, xmlNode->xmlChildrenNode, 1);

    if (content == NULL)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "content is null");
        return "";
    }


    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "got content: %s for xpath: %s", (char*)content, xpathExpr.c_str());
    std::string ret((char*)content);

    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);

    return ret;
}


bool GetElementData(xmlDocPtr doc,
                    std::string nsPrefix,
                    std::string nsURI,
                    std::string xpathExpr,
                    std::int32_t* to_set)
{
    std::string content = _GetElementData(doc, nsPrefix, nsURI, xpathExpr);

    if (content.empty())
    {
        return false;
    }

    *to_set = (std::int32_t) std::stoi(content);

    return true;
}

bool GetElementData(xmlDocPtr doc,
                    std::string nsPrefix,
                    std::string nsURI,
                    std::string xpathExpr,
                    bool* to_set)
{
    std::string content = _GetElementData(doc, nsPrefix, nsURI, xpathExpr);

    if (content.empty())
    {
        return false;
    }

    if (content.compare("true") == 0)
    {
        *to_set = true;
    }
    else if (content.compare("false") == 0)
    {
        *to_set = false;
    }
    else
    {
        return false;
    }

    return true;
}


bool GetElementData(xmlDocPtr doc,
                    std::string nsPrefix,
                    std::string nsURI,
                    std::string xpathExpr,
                    std::string* to_set)
{
    std::string content = _GetElementData(doc, nsPrefix, nsURI, xpathExpr);

    if (content.empty())
    {
        return false;
    }

    *to_set = content;

    return true;
}


bool parseDocument(xmlDocPtr *doc, const std::vector<uint8_t>& data)
{
    *doc = xmlReadMemory(reinterpret_cast<const char*>(data.data()), data.size(), "foo.xml", NULL, 0);

    if (*doc == NULL)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Could not parse xml document");
        return false;
    }

    //TODO: make some debug guards
    xmlChar *xmlbuff;
    int buffersize;
    xmlDocDumpFormatMemory(*doc, &xmlbuff, &buffersize, 1);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "parsed xml document:\n%s", (char *) xmlbuff);
    xmlFree(xmlbuff);

    return true;
}

}  // namespace xmlif
