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

#ifndef XML_FSM_HPP
#define XML_FSM_HPP

#include <cstdint>
#include <string>
#include <vector>

#include <libxml/tree.h>
#include <libxml/parser.h>


namespace xmlif {

bool GetElementData(xmlDocPtr doc,
                    std::string nsPrefix,
                    std::string nsURI,
                    std::string xpathExpr,
                    std::int32_t* to_set);

bool GetElementData(xmlDocPtr doc,
                    std::string nsPrefix,
                    std::string nsURI,
                    std::string xpathExpr,
                    bool* to_set);

bool GetElementData(xmlDocPtr doc,
                    std::string nsPrefix,
                    std::string nsURI,
                    std::string xpathExpr,
                    std::string* to_set);

bool parseDocument(xmlDocPtr *doc, const std::vector<uint8_t>& data);


}  // namespace xmlif

#endif  // XML_FSM_HPP
