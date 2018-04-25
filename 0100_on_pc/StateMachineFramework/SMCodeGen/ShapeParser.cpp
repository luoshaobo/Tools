#include "stdafx.h"
#include "ShapeParser.h"

#define NEW_LINE                    "\r\n"
#define CONDITION_ELSE              "else"

#define FLOAT_EQUAL(f1,f2)          (-0.0001f <= ((f1) - (f2)) && ((f1) - (f2)) <= 0.0001f)
#define SP_ESI_ITEM(err_name)       { SP_EC_ ## err_name, "SP_EC_" #err_name }

static const struct SP_ErrorStringItem {
    SP_ErrorCode nErrorCode;
    const char *pErrorStr;
} SP_ErrorStringMap[] = {
    SP_ESI_ITEM(OK),
    SP_ESI_ITEM(FAILED),
    SP_ESI_ITEM(NULL_PTR),
    SP_ESI_ITEM(OUT_OF_ARRAY_SCOPE),
    SP_ESI_ITEM(FAILED_TO_PARSE_XML),
    SP_ESI_ITEM(NO_SHAPE_TYPE),
    SP_ESI_ITEM(UNKNOWN_SHAPE_TYPE),
    SP_ESI_ITEM(NO_PINX),
    SP_ESI_ITEM(WRONG_FORMAT_FOR_PINX),
    SP_ESI_ITEM(NO_PINY),
    SP_ESI_ITEM(WRONG_FORMAT_FOR_PINY),
    SP_ESI_ITEM(NO_WIDTH),
    SP_ESI_ITEM(WRONG_FORMAT_FOR_WIDTH),
    SP_ESI_ITEM(NO_HEIGHT),
    SP_ESI_ITEM(WRONG_FORMAT_FOR_HEIGHT),
    SP_ESI_ITEM(NO_BEGINX),
    SP_ESI_ITEM(WRONG_FORMAT_FOR_BEGINX),
    SP_ESI_ITEM(NO_BEGINY),
    SP_ESI_ITEM(WRONG_FORMAT_FOR_BEGINY),
    SP_ESI_ITEM(NO_ENDX),
    SP_ESI_ITEM(WRONG_FORMAT_FOR_ENDX),
    SP_ESI_ITEM(NO_ENDY),
    SP_ESI_ITEM(WRONG_FORMAT_FOR_ENDY),
    SP_ESI_ITEM(NO_TEXT),
    SP_ESI_ITEM(NO_STATE_NAME),
    SP_ESI_ITEM(INVALID_STATE_NAME),
    SP_ESI_ITEM(NO_BEGIN_SHAPE_STATE),
    SP_ESI_ITEM(NO_END_SHAPE_STATE),
    SP_ESI_ITEM(WRONG_TRANS_ITEM_FORMAT),
    SP_ESI_ITEM(INVALID_DEF_TRANSITION),
    SP_ESI_ITEM(INVALID_EVENT_NAME),
    SP_ESI_ITEM(INVALID_EVENT_CONDITION),
    SP_ESI_ITEM(INVALID_EVENT_OPERATION),
    SP_ESI_ITEM(SAME_STATE_NANE),
    SP_ESI_ITEM(SAME_OR_INCLUSION_EVENTS_IN_STATE),
    SP_ESI_ITEM(NO_ROOT_STATE),
};

ShapeParser::ShapeParser(const std::string &sShapesXml)
    : m_sShapesXml(sShapesXml)
    , m_arrShapeStates()
    , m_arrShapeTranss()
    , m_pShapeTree(NULL)
    , m_sLastErrorXml()    
{

}

ShapeParser::~ShapeParser()
{

}

SP_ErrorCode ShapeParser::DoParsing()
{
    SP_ErrorCode nErrorCode = SP_EC_FAILED;
    entTlmXmlParser::E_ERR txpErr;
    entTlmXmlParser::T_STRINGS sShapeItems;
    unsigned int i;

    m_sLastErrorXml = m_sShapesXml;
    entTlmXmlParser allContentParser(m_sShapesXml.c_str());
    txpErr = allContentParser.getXmlValuesByXPath("/AllShapes/Shape", sShapeItems);
    if (txpErr != entTlmXmlParser::E_ERR_OK) {
        nErrorCode = SP_EC_FAILED_TO_PARSE_XML;
        goto ERR1;
    }

    for (i = 0; i < sShapeItems.size(); i++) {
        entTlmXmlParser::T_STRINGS sTypes;
        entTlmXmlParser::T_STRINGS sTempStrs;

        m_sLastErrorXml = sShapeItems[i];
        entTlmXmlParser shapeParser(sShapeItems[i].c_str());
        txpErr = shapeParser.getTextValuesByXPath("/Shape/@Type", sTypes);
        if (txpErr != entTlmXmlParser::E_ERR_OK) {
            nErrorCode = SP_EC_FAILED_TO_PARSE_XML;
            goto ERR1;
        }
        if (sTypes.size() != 1) {
            nErrorCode = SP_EC_NO_SHAPE_TYPE;
            goto ERR1;
        }

        if (sTypes[0] == "State") {
            ShapeState oShapeState;
            oShapeState.sXmlContent = sShapeItems[i];
            
            //
            // PinX
            //
            txpErr = shapeParser.getTextValuesByXPath("/Shape/PinX", sTempStrs);
            if (txpErr != entTlmXmlParser::E_ERR_OK) {
                nErrorCode = SP_EC_FAILED_TO_PARSE_XML;
                goto ERR1;
            }
            if (sTempStrs.size() != 1) {
                nErrorCode = SP_EC_NO_PINX;
                goto ERR1;
            }
            nErrorCode = ParseDoubleData(sTempStrs[0], oShapeState.dPinX);
            if (nErrorCode != SP_EC_OK) {
                nErrorCode = SP_EC_WRONG_FORMAT_FOR_PINX;
                goto ERR1;
            }

            //
            // PinY
            //
            txpErr = shapeParser.getTextValuesByXPath("/Shape/PinY", sTempStrs);
            if (txpErr != entTlmXmlParser::E_ERR_OK) {
                nErrorCode = SP_EC_FAILED_TO_PARSE_XML;
                goto ERR1;
            }
            if (sTempStrs.size() != 1) {
                nErrorCode = SP_EC_NO_PINY;
                goto ERR1;
            }
            nErrorCode = ParseDoubleData(sTempStrs[0], oShapeState.dPinY);
            if (nErrorCode != SP_EC_OK) {
                nErrorCode = SP_EC_WRONG_FORMAT_FOR_PINY;
                goto ERR1;
            }

            //
            // Width
            //
            txpErr = shapeParser.getTextValuesByXPath("/Shape/Width", sTempStrs);
            if (txpErr != entTlmXmlParser::E_ERR_OK) {
                nErrorCode = SP_EC_FAILED_TO_PARSE_XML;
                goto ERR1;
            }
            if (sTempStrs.size() != 1) {
                nErrorCode = SP_EC_NO_WIDTH;
                goto ERR1;
            }
            nErrorCode = ParseDoubleData(sTempStrs[0], oShapeState.dWidth);
            if (nErrorCode != SP_EC_OK) {
                nErrorCode = SP_EC_WRONG_FORMAT_FOR_WIDTH;
                goto ERR1;
            }

            //
            // Height
            //
            txpErr = shapeParser.getTextValuesByXPath("/Shape/Height", sTempStrs);
            if (txpErr != entTlmXmlParser::E_ERR_OK) {
                nErrorCode = SP_EC_FAILED_TO_PARSE_XML;
                goto ERR1;
            }
            if (sTempStrs.size() != 1) {
                nErrorCode = SP_EC_NO_HEIGHT;
                goto ERR1;
            }
            nErrorCode = ParseDoubleData(sTempStrs[0], oShapeState.dHeight);
            if (nErrorCode != SP_EC_OK) {
                nErrorCode = SP_EC_WRONG_FORMAT_FOR_HEIGHT;
                goto ERR1;
            }

            //
            // Text
            //
            txpErr = shapeParser.getTextValuesByXPath("/Shape/Text", sTempStrs);
            if (txpErr != entTlmXmlParser::E_ERR_OK) {
                nErrorCode = SP_EC_FAILED_TO_PARSE_XML;
                goto ERR1;
            }
            if (sTempStrs.size() != 1) {
                nErrorCode = SP_EC_NO_TEXT;
                goto ERR1;
            }
            oShapeState.sText = sTempStrs[0];

            //
            // handle others
            //
            nErrorCode = HandleShapeState(oShapeState);
            if (nErrorCode != SP_EC_OK) {
                goto ERR1;
            }
        } else if (sTypes[0] == "Transition") {
            ShapeTrans oShapeTrans;
            oShapeTrans.sXmlContent = sShapeItems[i];
            
            //
            // PinX
            //
            txpErr = shapeParser.getTextValuesByXPath("/Shape/BeginX", sTempStrs);
            if (txpErr != entTlmXmlParser::E_ERR_OK) {
                nErrorCode = SP_EC_FAILED_TO_PARSE_XML;
                goto ERR1;
            }
            if (sTempStrs.size() != 1) {
                nErrorCode = SP_EC_NO_BEGINX;
                goto ERR1;
            }
            nErrorCode = ParseDoubleData(sTempStrs[0], oShapeTrans.dBeginX);
            if (nErrorCode != SP_EC_OK) {
                nErrorCode = SP_EC_WRONG_FORMAT_FOR_BEGINX;
                goto ERR1;
            }

            //
            // BeginY
            //
            txpErr = shapeParser.getTextValuesByXPath("/Shape/BeginY", sTempStrs);
            if (txpErr != entTlmXmlParser::E_ERR_OK) {
                nErrorCode = SP_EC_FAILED_TO_PARSE_XML;
                goto ERR1;
            }
            if (sTempStrs.size() != 1) {
                nErrorCode = SP_EC_NO_BEGINY;
                goto ERR1;
            }
            nErrorCode = ParseDoubleData(sTempStrs[0], oShapeTrans.dBeginY);
            if (nErrorCode != SP_EC_OK) {
                nErrorCode = SP_EC_WRONG_FORMAT_FOR_BEGINY;
                goto ERR1;
            }

            //
            // EndX
            //
            txpErr = shapeParser.getTextValuesByXPath("/Shape/EndX", sTempStrs);
            if (txpErr != entTlmXmlParser::E_ERR_OK) {
                nErrorCode = SP_EC_FAILED_TO_PARSE_XML;
                goto ERR1;
            }
            if (sTempStrs.size() != 1) {
                nErrorCode = SP_EC_NO_ENDX;
                goto ERR1;
            }
            nErrorCode = ParseDoubleData(sTempStrs[0], oShapeTrans.dEndX);
            if (nErrorCode != SP_EC_OK) {
                nErrorCode = SP_EC_WRONG_FORMAT_FOR_ENDX;
                goto ERR1;
            }

            //
            // EndY
            //
            txpErr = shapeParser.getTextValuesByXPath("/Shape/EndY", sTempStrs);
            if (txpErr != entTlmXmlParser::E_ERR_OK) {
                nErrorCode = SP_EC_FAILED_TO_PARSE_XML;
                goto ERR1;
            }
            if (sTempStrs.size() != 1) {
                nErrorCode = SP_EC_NO_ENDY;
                goto ERR1;
            }
            nErrorCode = ParseDoubleData(sTempStrs[0], oShapeTrans.dEndY);
            if (nErrorCode != SP_EC_OK) {
                nErrorCode = SP_EC_WRONG_FORMAT_FOR_ENDY;
                goto ERR1;
            }

            //
            // Text
            //
            txpErr = shapeParser.getTextValuesByXPath("/Shape/Text", sTempStrs);
            if (txpErr != entTlmXmlParser::E_ERR_OK) {
                nErrorCode = SP_EC_FAILED_TO_PARSE_XML;
                goto ERR1;
            }
            if (sTempStrs.size() != 1) {
                nErrorCode = SP_EC_NO_TEXT;
                goto ERR1;
            }
            oShapeTrans.sText = sTempStrs[0];

            //
            // handle others
            //
            nErrorCode = HandleShapeTrans(oShapeTrans);
            if (nErrorCode != SP_EC_OK) {
                goto ERR1;
            }
        } else {
            nErrorCode = SP_EC_UNKNOWN_SHAPE_TYPE;
            goto ERR1;
        }
    }

    nErrorCode = BuildRelationship();
    if (nErrorCode != SP_EC_OK) {
        goto ERR1;
    }

    nErrorCode = SP_EC_OK;
ERR1:
    return nErrorCode;
}

SP_ErrorCode ShapeParser::HandleShapeState(ShapeState &oShapeState)
{
    SP_ErrorCode nErrorCode = SP_EC_FAILED;
    std::vector<std::string> arrLines;
    unsigned int i;
    bool bStateNameFound = false;

    arrLines = TK_Tools::SplitString(oShapeState.sText, "\n");
    if (arrLines.size() == 0) {
        nErrorCode = SP_EC_NO_STATE_NAME;
        goto ERR1;
    }
    for (i = 0; i < arrLines.size(); i++) {
        std::string sLine = arrLines[i];
        sLine = TK_Tools::TrimAll(sLine);
        if (sLine.empty()) {
            continue;
        }
        if (!bStateNameFound) {
            if (!IsStateNameValid(sLine)) {
                nErrorCode = SP_EC_INVALID_STATE_NAME;
                goto ERR1;
            }
            oShapeState.sStateName = sLine;
            bStateNameFound = true;
        } else {
            TransItem oTransItem;
            nErrorCode = ParseTransItem(sLine, oTransItem);
            if (nErrorCode != SP_EC_OK) {
                goto ERR1;
            }
            oShapeState.arrInternalTransItems.push_back(oTransItem);
        }
    }

    nErrorCode = InsertShapeStateToSortedList(oShapeState);
    if (nErrorCode != SP_EC_OK) {
        goto ERR1;
    }

    nErrorCode = SP_EC_OK;
ERR1:
    return nErrorCode;
}

SP_ErrorCode ShapeParser::HandleShapeTrans(ShapeTrans &oShapeTrans)
{
    SP_ErrorCode nErrorCode = SP_EC_FAILED;
    std::vector<std::string> arrLines;
    unsigned int i;

    arrLines = TK_Tools::SplitString(oShapeTrans.sText, "\n");
    for (i = 0; i < arrLines.size(); i++) {
        std::string sLine = arrLines[i];
        sLine = TK_Tools::TrimAll(sLine);
        if (sLine.empty()) {
            continue;
        } else {
            TransItem oTransItem;
            nErrorCode = ParseTransItem(sLine, oTransItem);
            if (nErrorCode != SP_EC_OK) {
                goto ERR1;
            }
            oShapeTrans.arrTransItems.push_back(oTransItem);
        }
    }

    m_arrShapeTranss.push_back(oShapeTrans);

    nErrorCode = SP_EC_OK;
ERR1:
    return nErrorCode;
}

SP_ErrorCode ShapeParser::BuildRelationship()
{
    SP_ErrorCode nErrorCode = SP_EC_FAILED;
    
    //
    // find parents and children for states
    //
    {
        ShapeState *pParentShapeState;
        unsigned int i;
        for (i = 0; i < m_arrShapeStates.size(); i++) {
            ShapeState &oShapeState = m_arrShapeStates[i];
            m_sLastErrorXml = oShapeState.sXmlContent;
            nErrorCode = FindParentShapeState(i, &pParentShapeState);
            if (nErrorCode != SP_EC_OK) {
                goto ERR1;
            }
            if (pParentShapeState != NULL) {
                oShapeState.pParentShapeState = pParentShapeState;
                pParentShapeState->arrChildShapeStates.push_back(&oShapeState);
            }
        }
    }

    //
    // find states of two ends for transs
    //
    {
        ShapeState *pBeginShapeState;
        ShapeState *pEndShapeState;
        unsigned int i;
        for (i = 0; i < m_arrShapeTranss.size(); i++) {
            ShapeTrans &oShapeTrans = m_arrShapeTranss[i];
            m_sLastErrorXml = oShapeTrans.sXmlContent;
            nErrorCode = FindBeginAndEndShapeState(oShapeTrans, &pBeginShapeState, &pEndShapeState);
            if (nErrorCode != SP_EC_OK) {
                goto ERR1;
            }
            if (pBeginShapeState == NULL) {
                nErrorCode = SP_EC_NO_BEGIN_SHAPE_STATE;
                goto ERR1;
            } else if (pEndShapeState == NULL) {
                nErrorCode = SP_EC_NO_END_SHAPE_STATE;
                goto ERR1;
            }
            if (oShapeTrans.arrTransItems.size() == 0) {
                if (pEndShapeState->pParentShapeState != pBeginShapeState) {
                    nErrorCode = SP_EC_INVALID_DEF_TRANSITION;
                    goto ERR1;
                }
            }
            oShapeTrans.pBeginShapeState = pBeginShapeState;
            oShapeTrans.pEndShapeState = pEndShapeState;
            pBeginShapeState->arrShapeTransFromMe.push_back(&oShapeTrans);
            pEndShapeState->arrShapeTransToMe.push_back(&oShapeTrans);
        }
    }

    //
    // find root state
    //
    {
        if (m_arrShapeStates.size() > 0) {
            ShapeState *pShapeState = &m_arrShapeStates[0];
            while (pShapeState != NULL && pShapeState->pParentShapeState != NULL) {
                pShapeState = pShapeState->pParentShapeState;
            }
            m_pShapeTree = pShapeState;
        } else {
            m_pShapeTree = NULL;
            nErrorCode = SP_EC_NO_ROOT_STATE;
            goto ERR1;
        }
    }

    //
    // check if states with same name
    //
    {
        if (AreThereSameStateNames()) {
            nErrorCode = SP_EC_SAME_STATE_NANE;
            goto ERR1;
        }
    }

    //
    // check if events inclusive
    //
    {
        if (AreThereSameOrInclusionEvents()) {
            nErrorCode = SP_EC_SAME_OR_INCLUSION_EVENTS_IN_STATE;
            goto ERR1;
        }
    }

    nErrorCode = SP_EC_OK;
ERR1:
    return nErrorCode;
}

//
// NOTE: 
// 1) The orignal point of the coordinates is on the left-bottom of the page.
// 2) The left-top corner is the smallest.
// 3) The sort order is from smaller to bigger.
//
SP_ErrorCode ShapeParser::InsertShapeStateToSortedList(ShapeState &oShapeState)
{
    SP_ErrorCode nErrorCode = SP_EC_FAILED;
    std::vector<ShapeState>::iterator it;

    for (it = m_arrShapeStates.begin(); it != m_arrShapeStates.end(); ++it) {
        ShapeState &oCurrentShapeState = *it;
        if (oShapeState.dPinY + oShapeState.dHeight / 2.0 > oCurrentShapeState.dPinY + oCurrentShapeState.dHeight / 2.0) {
            break;
        } else if (FLOAT_EQUAL(oShapeState.dPinY + oShapeState.dHeight / 2.0, oCurrentShapeState.dPinY + oCurrentShapeState.dHeight / 2.0)){
            if (oShapeState.dPinX - oShapeState.dWidth / 2.0 < oCurrentShapeState.dPinX - oCurrentShapeState.dWidth / 2.0) {
                break;
            }
        }
    }
    if (it == m_arrShapeStates.end()) {
        m_arrShapeStates.push_back(oShapeState);
    } else {
        m_arrShapeStates.insert(it, oShapeState);
    }

    nErrorCode = SP_EC_OK;
    return nErrorCode;
}

SP_ErrorCode ShapeParser::FindParentShapeState(unsigned int nShapeStateIndex, ShapeState **ppParentShapeState)
{
    SP_ErrorCode nErrorCode = SP_EC_FAILED;
    int i;

    if (ppParentShapeState == NULL) {
        nErrorCode = SP_EC_NULL_PTR;
        goto ERR1;
    }

    if (nShapeStateIndex >= m_arrShapeStates.size()) {
        nErrorCode = SP_EC_OUT_OF_ARRAY_SCOPE;
        goto ERR1;
    }

    ShapeState &oShapeState = m_arrShapeStates[nShapeStateIndex];
    for (i = nShapeStateIndex - 1; i >= 0; i--) {
        ShapeState &oCurrShapeState = m_arrShapeStates[i];
        if (   oCurrShapeState.dPinX - oCurrShapeState.dWidth / 2.0 <= oShapeState.dPinX - oShapeState.dWidth / 2.0
            && oCurrShapeState.dPinX + oCurrShapeState.dWidth / 2.0 >= oShapeState.dPinX + oShapeState.dWidth / 2.0 
            && oCurrShapeState.dPinY - oCurrShapeState.dHeight / 2.0 <= oShapeState.dPinY - oShapeState.dHeight / 2.0
            && oCurrShapeState.dPinY + oCurrShapeState.dHeight / 2.0 >= oShapeState.dPinY + oShapeState.dHeight / 2.0) {
            break;
        }
    }
    if (i >= 0) {
        *ppParentShapeState = &m_arrShapeStates[i];
    } else {
        *ppParentShapeState = NULL;
    }

    nErrorCode = SP_EC_OK;
ERR1:
    return nErrorCode;
}

SP_ErrorCode ShapeParser::FindBeginAndEndShapeState(ShapeTrans &oShapeTrans, ShapeState **ppBeginShapeState, ShapeState **ppEndShapeState)
{
    SP_ErrorCode nErrorCode = SP_EC_FAILED;
    unsigned int i;

    if (ppBeginShapeState == NULL || ppEndShapeState == NULL) {
        nErrorCode = SP_EC_NULL_PTR;
        goto ERR1;
    }
    
    for (i = 0; i < m_arrShapeStates.size(); i++) {
        ShapeState &oShapeState = m_arrShapeStates[i];
        if (IsPointOnShapeStateBorder(oShapeTrans.dBeginX, oShapeTrans.dBeginY, oShapeState)) {
            *ppBeginShapeState = &oShapeState;
            break;
        }
    }
    if (i >= m_arrShapeStates.size()) {
        nErrorCode = SP_EC_NO_BEGIN_SHAPE_STATE;
        goto ERR1;
    }

    for (i = 0; i < m_arrShapeStates.size(); i++) {
        ShapeState &oShapeState = m_arrShapeStates[i];
        if (IsPointOnShapeStateBorder(oShapeTrans.dEndX, oShapeTrans.dEndY, oShapeState)) {
            *ppEndShapeState = &oShapeState;
            break;
        }
    }
    if (i >= m_arrShapeStates.size()) {
        nErrorCode = SP_EC_NO_END_SHAPE_STATE;
        goto ERR1;
    }

    nErrorCode = SP_EC_OK;
ERR1:
    return nErrorCode;
}

SP_ErrorCode ShapeParser::ParseDoubleData(const std::string &sValue, double &dResult)
{
    SP_ErrorCode nErrorCode = SP_EC_FAILED;
    std::string sTemp = sValue;
    std::string sUnit = " mm";
    
    sTemp = TK_Tools::TrimAll(sTemp);

    if (sTemp.size() >= sUnit.size()) {
        if (sUnit == (sTemp.c_str() + sTemp.size() - sUnit.size())) {
            sTemp = sTemp.substr(0, sTemp.size() - sUnit.size());
        }
    }

    dResult = TK_Tools::StrToFloat(sTemp);

    nErrorCode = SP_EC_OK;
    return nErrorCode;
}

SP_ErrorCode ShapeParser::ParseTransItem(const std::string &sText, TransItem &oTransItem)
{
    SP_ErrorCode nErrorCode = SP_EC_FAILED;
    std::string sEventCondition;
    std::string sEvent;
    std::string sCondition;
    std::string sOperation;
    unsigned int nPos;

    nPos = sText.find('/');
    if (nPos != std::string::npos) {
        sEventCondition = sText.substr(0, nPos);
        sEventCondition = TK_Tools::TrimAll(sEventCondition);
        sOperation = sText.substr(nPos + 1);
        sOperation = TK_Tools::TrimAll(sOperation);
    } else {
        sEventCondition = sText;
        sOperation = "";
    }

    if (!sEventCondition.empty()) {
        if (sEventCondition[sEventCondition.size() - 1] == ']') {
            nPos = sText.find('[');
            if (nPos == std::string::npos) {
                nErrorCode = SP_EC_WRONG_TRANS_ITEM_FORMAT;
                goto ERR1;
            }
            sEvent = sEventCondition.substr(0, nPos);
            sEvent = TK_Tools::TrimAll(sEvent);
            sCondition = sEventCondition.substr(nPos + 1, sEventCondition.size() - nPos - 2);
            sCondition = TK_Tools::TrimAll(sCondition);
        } else {
            sEvent = sEventCondition;
            sCondition = "";
        }
    } else {
        sEvent = "";
        sCondition = "";
    }
    
    if (!IsEventNameValid(sEvent)) {
        nErrorCode = SP_EC_INVALID_EVENT_NAME;
        goto ERR1;
    }

    if (!IsEventConditionValid(sCondition)) {
        nErrorCode = SP_EC_INVALID_EVENT_CONDITION;
        goto ERR1;
    }

    if (!IsEventOperationValid(sOperation)) {
        nErrorCode = SP_EC_INVALID_EVENT_OPERATION;
        goto ERR1;
    }

    oTransItem.sEvent = sEvent;
    oTransItem.sCondition = UnescapeXmlText(sCondition);
    oTransItem.sOperation = sOperation;

    /// {{{ for test only
    if (oTransItem.sEvent.empty() && oTransItem.sCondition.empty()) {
        nErrorCode = nErrorCode;
    }
    if (oTransItem.sEvent.empty() && oTransItem.sOperation.empty()) {
        nErrorCode = nErrorCode;
    }
    if (oTransItem.sEvent.empty() && oTransItem.sCondition.empty() && oTransItem.sOperation.empty()) {
        nErrorCode = nErrorCode;
    }
    /// for test only }}}

    nErrorCode = SP_EC_OK;
ERR1:
    return nErrorCode;
}

bool ShapeParser::IsPointOnShapeStateBorder(double dX, double dY, const ShapeState &oShapeState)
{
    bool bReuslt = false;

    if (FLOAT_EQUAL(dX, oShapeState.dPinX - oShapeState.dWidth / 2.0) || FLOAT_EQUAL(dX, oShapeState.dPinX + oShapeState.dWidth / 2.0)) {
        if (oShapeState.dPinY - oShapeState.dHeight / 2.0 <= dY && dY <= oShapeState.dPinY + oShapeState.dHeight / 2.0) {
            bReuslt = true;
        }
    } else if (FLOAT_EQUAL(dY, oShapeState.dPinY - oShapeState.dHeight / 2.0) || FLOAT_EQUAL(dY, oShapeState.dPinY + oShapeState.dHeight / 2.0)) {
        if (oShapeState.dPinX - oShapeState.dWidth / 2.0 <= dX && dX <= oShapeState.dPinX + oShapeState.dWidth / 2) {
            bReuslt = true;
        }
    }

    return bReuslt;
}

std::string ShapeParser::GetErrorStr(SP_ErrorCode nErrorCode) const
{
    std::string sErrorStr;
    unsigned int nErrStrTableSize = TK_ARR_LEN(SP_ErrorStringMap);
    unsigned int i;

    for (i = 0; i < nErrStrTableSize; i++) {
        if (SP_ErrorStringMap[i].nErrorCode == nErrorCode) {
            sErrorStr = SP_ErrorStringMap[i].pErrorStr;
            break;
        }
    }
    if (i >= nErrStrTableSize) {
        sErrorStr = TK_Tools::FormatStr("%u", nErrorCode);
    }

    return sErrorStr;
}

bool ShapeParser::IsStateNameValid(const std::string &sStateName) const
{
    bool bResult = true;
    static std::string sValidChars = 
        "ABCDEFGHIGKLMNOPQRSTUVWXYZ"
        "abcdefghigklmnopqrstuvwxyz"
        "0123456789"
        "_";
    unsigned int nPos;
    unsigned int i;

    for (i = 0; i < sStateName.length(); i++) {
        nPos = sValidChars.find(sStateName[i]);
        if (nPos == std::string::npos) {
            bResult = false;
        }
    }

    return bResult;
}

bool ShapeParser::IsEventNameValid(const std::string &sEventName) const
{
    bool bResult = true;
    static std::string sValidChars = 
        "ABCDEFGHIGKLMNOPQRSTUVWXYZ"
        "abcdefghigklmnopqrstuvwxyz"
        "0123456789"
        "_";
    unsigned int nPos;
    unsigned int i;

    for (i = 0; i < sEventName.length(); i++) {
        nPos = sValidChars.find(sEventName[i]);
        if (nPos == std::string::npos) {
            bResult = false;
        }
    }

    return bResult;
}

bool ShapeParser::IsEventConditionValid(const std::string &sEventCondition) const
{
    bool bResult = true;

    return bResult;
}

bool ShapeParser::IsEventOperationValid(const std::string &sEventOperation) const
{
    bool bResult = true;
    static std::string sValidChars = 
        "ABCDEFGHIGKLMNOPQRSTUVWXYZ"
        "abcdefghigklmnopqrstuvwxyz"
        "0123456789"
        "_";
    unsigned int nPos;
    unsigned int i;

    for (i = 0; i < sEventOperation.length(); i++) {
        nPos = sValidChars.find(sEventOperation[i]);
        if (nPos == std::string::npos) {
            bResult = false;
        }
    }

    return bResult;
}

bool ShapeParser::AreThereSameStateNames()
{
    bool bResult = false;
    unsigned int i, j;

    for (i = 0; i < m_arrShapeStates.size(); i++) {
        for (j = i + 1; j < m_arrShapeStates.size(); j++) {
            if (m_arrShapeStates[i].sStateName == m_arrShapeStates[j].sStateName) {
                m_sLastErrorXml = m_arrShapeStates[i].sXmlContent + "\r\n\r\n" + m_arrShapeStates[j].sXmlContent;
                bResult = true;
                break;
            }
        }
    }

    return bResult;
}

bool ShapeParser::AreThereSameOrInclusionEvents()
{
    static const char *EVENT_CONDITION_SEP = "__###__";
    bool bResult = false;
    unsigned int i, j, k;
    std::vector<std::string> vecTemp;
    std::vector<std::string>::iterator it, it2;
    std::string sTempKey;

    for (i = 0; i < m_arrShapeStates.size(); i++) {
        ShapeState &oShapeState = m_arrShapeStates[i];
        vecTemp.clear();
        for (j = 0; j < oShapeState.arrInternalTransItems.size(); j++) {
            TransItem &oTransItem = oShapeState.arrInternalTransItems[j];
            if (TK_Tools::CompareNoCase(oTransItem.sCondition, CONDITION_ELSE) != 0) {
                sTempKey = oTransItem.sEvent + EVENT_CONDITION_SEP + oTransItem.sCondition;
            } else {
                sTempKey = oTransItem.sEvent + EVENT_CONDITION_SEP + CONDITION_ELSE;
            }
            it = std::find<std::vector<std::string>::iterator, std::string>(vecTemp.begin(), vecTemp.end(), sTempKey);
            if (it != vecTemp.end()) {
                bResult = true;
                m_sLastErrorXml = oShapeState.sXmlContent;
                goto ERR1;
            }
            vecTemp.push_back(sTempKey);
        }
        for (j = 0; j < oShapeState.arrShapeTransFromMe.size(); j++) {
            if (oShapeState.arrShapeTransFromMe[j] != NULL) {
                ShapeTrans &oShapeTrans = *oShapeState.arrShapeTransFromMe[j];
                if (oShapeTrans.arrTransItems.size() != 0) {
                    for (k = 0; k < oShapeTrans.arrTransItems.size(); k++) {
                        TransItem &oTransItem = oShapeTrans.arrTransItems[k];
                        if (TK_Tools::CompareNoCase(oTransItem.sCondition, CONDITION_ELSE) != 0) {
                            sTempKey = oTransItem.sEvent + EVENT_CONDITION_SEP + oTransItem.sCondition;
                        } else {
                            sTempKey = oTransItem.sEvent + EVENT_CONDITION_SEP + CONDITION_ELSE;
                        }
                        it = std::find<std::vector<std::string>::iterator, std::string>(vecTemp.begin(), vecTemp.end(), sTempKey);
                        if (it != vecTemp.end()) {
                            bResult = true;
                            m_sLastErrorXml = oShapeState.sXmlContent;
                            goto ERR1;
                        }
                        vecTemp.push_back(sTempKey);
                    }
                } else {
                    sTempKey = EVENT_CONDITION_SEP;
                    it = std::find<std::vector<std::string>::iterator, std::string>(vecTemp.begin(), vecTemp.end(), sTempKey);
                    if (it != vecTemp.end()) {
                        bResult = true;
                        m_sLastErrorXml = oShapeState.sXmlContent;
                        goto ERR1;
                    }
                    vecTemp.push_back(sTempKey);
                }
            }
        }

        for (j = 0; j < vecTemp.size(); j++) {
            std::string s1 = vecTemp[j];
            for (k = j + 1; k < vecTemp.size(); k++) {
                std::string s2 = vecTemp[k];
                if (TK_Tools::CompareCaseMinLen(s1, s2) == 0) {
                    bResult = true;
                    m_sLastErrorXml = oShapeState.sXmlContent;
                    goto ERR1;
                }
            }
        }        
    }

    bResult = false;
ERR1:
    return bResult;
}

SP_ErrorCode ShapeParser::Dump(std::string &sOutputContent)
{
    SP_ErrorCode nErrorCode = SP_EC_FAILED;
    unsigned int i;
    unsigned int j;

    sOutputContent += TK_Tools::FormatStr("<Root>") + NEW_LINE;

    for (i = 0; i < m_arrShapeStates.size(); i++) {
        ShapeState &oShapeState = m_arrShapeStates[i];
        sOutputContent += TK_Tools::FormatStr("    <State>") + NEW_LINE;
        sOutputContent += TK_Tools::FormatStr("        <Name>%s</Name>", oShapeState.sStateName.c_str()) + NEW_LINE;

        sOutputContent += TK_Tools::FormatStr("        <ParentState>") + NEW_LINE;
        if (oShapeState.pParentShapeState != NULL) {
            sOutputContent += TK_Tools::FormatStr("            <Name>%s</Name>", oShapeState.pParentShapeState->sStateName.c_str()) + NEW_LINE;
        }
        sOutputContent += TK_Tools::FormatStr("        </ParentState>") + NEW_LINE;

        sOutputContent += TK_Tools::FormatStr("        <InternalTransitions>") + NEW_LINE;
        for (j = 0; j < oShapeState.arrInternalTransItems.size(); j++) {
            sOutputContent += TK_Tools::FormatStr("            <TransitionItem>") + NEW_LINE;
            sOutputContent += TK_Tools::FormatStr("                <Event>%s</Event>", oShapeState.arrInternalTransItems[j].sEvent.c_str()) + NEW_LINE;
            sOutputContent += TK_Tools::FormatStr("                <Condition>%s</Condition>", EscapeXmlText(oShapeState.arrInternalTransItems[j].sCondition.c_str()).c_str()) + NEW_LINE;
            sOutputContent += TK_Tools::FormatStr("                <Operation>%s</Operation>", oShapeState.arrInternalTransItems[j].sOperation.c_str()) + NEW_LINE;
            sOutputContent += TK_Tools::FormatStr("            </TransitionItem>") + NEW_LINE;
        }
        sOutputContent += TK_Tools::FormatStr("        </InternalTransitions>") + NEW_LINE;

        sOutputContent += TK_Tools::FormatStr("        <ChildrenShapes>") + NEW_LINE;
        for (j = 0; j < oShapeState.arrChildShapeStates.size(); j++) {
            sOutputContent += TK_Tools::FormatStr("            <Shape>") + NEW_LINE;
            sOutputContent += TK_Tools::FormatStr("                <Name>%s</Name>", oShapeState.arrChildShapeStates[j]->sStateName.c_str()) + NEW_LINE;
            sOutputContent += TK_Tools::FormatStr("            </Shape>") + NEW_LINE;
        }
        sOutputContent += TK_Tools::FormatStr("        </ChildrenShapes>") + NEW_LINE;

        sOutputContent += TK_Tools::FormatStr("        <PinX>%f</PinX>", oShapeState.dPinX) + NEW_LINE;
        sOutputContent += TK_Tools::FormatStr("        <PinY>%f</PinY>", oShapeState.dPinY) + NEW_LINE;
        sOutputContent += TK_Tools::FormatStr("        <Width>%f</Width>", oShapeState.dWidth) + NEW_LINE;
        sOutputContent += TK_Tools::FormatStr("        <Height>%f</Height>", oShapeState.dHeight) + NEW_LINE;
        sOutputContent += TK_Tools::FormatStr("        <Text>%s</Text>", EscapeXmlText(oShapeState.sText.c_str()).c_str()) + NEW_LINE;
        sOutputContent += TK_Tools::FormatStr("    </State>") + NEW_LINE;
    }

    for (i = 0; i < m_arrShapeTranss.size(); i++) {
        ShapeTrans &oShapeTrans = m_arrShapeTranss[i];
        sOutputContent += TK_Tools::FormatStr("    <Transition>") + NEW_LINE;

        sOutputContent += TK_Tools::FormatStr("        <InternalTransitions>") + NEW_LINE;
        for (j = 0; j < oShapeTrans.arrTransItems.size(); j++) {
            sOutputContent += TK_Tools::FormatStr("            <TransitionItem>") + NEW_LINE;
            sOutputContent += TK_Tools::FormatStr("                <Event>%s</Event>", oShapeTrans.arrTransItems[j].sEvent.c_str()) + NEW_LINE;
            sOutputContent += TK_Tools::FormatStr("                <Condition>%s</Condition>", EscapeXmlText(oShapeTrans.arrTransItems[j].sCondition.c_str()).c_str()) + NEW_LINE;
            sOutputContent += TK_Tools::FormatStr("                <Operation>%s</Operation>", oShapeTrans.arrTransItems[j].sOperation.c_str()) + NEW_LINE;
            sOutputContent += TK_Tools::FormatStr("            </TransitionItem>") + NEW_LINE;
        }
        sOutputContent += TK_Tools::FormatStr("        </InternalTransitions>") + NEW_LINE;

        sOutputContent += TK_Tools::FormatStr("        <BeginState>") + NEW_LINE;
        if (oShapeTrans.pBeginShapeState != NULL) {
            sOutputContent += TK_Tools::FormatStr("            <Name>%s</Name>", oShapeTrans.pBeginShapeState->sStateName.c_str()) + NEW_LINE;
        }
        sOutputContent += TK_Tools::FormatStr("        </BeginState>") + NEW_LINE;

        sOutputContent += TK_Tools::FormatStr("        <EndState>") + NEW_LINE;
        if (oShapeTrans.pEndShapeState != NULL) {
            sOutputContent += TK_Tools::FormatStr("            <Name>%s</Name>", oShapeTrans.pEndShapeState->sStateName.c_str()) + NEW_LINE;
        }
        sOutputContent += TK_Tools::FormatStr("        </EndState>") + NEW_LINE;

        sOutputContent += TK_Tools::FormatStr("        <BeginX>%f</BeginX>", oShapeTrans.dBeginX) + NEW_LINE;
        sOutputContent += TK_Tools::FormatStr("        <BeginY>%f</BeginY>", oShapeTrans.dBeginY) + NEW_LINE;
        sOutputContent += TK_Tools::FormatStr("        <EndX>%f</EndX>", oShapeTrans.dEndX) + NEW_LINE;
        sOutputContent += TK_Tools::FormatStr("        <EndY>%f</EndY>", oShapeTrans.dEndY) + NEW_LINE;
        sOutputContent += TK_Tools::FormatStr("        <Text>%s</Text>", EscapeXmlText(oShapeTrans.sText.c_str()).c_str()) + NEW_LINE;
        sOutputContent += TK_Tools::FormatStr("    </Transition>") + NEW_LINE;
    }

    sOutputContent += TK_Tools::FormatStr("</Root>") + NEW_LINE;

    nErrorCode = SP_EC_OK;
    return nErrorCode;
}

std::string ShapeParser::EscapeXmlText(const std::string &sOrig)
{
    std::string sResult;
    unsigned int i;

    for (i = 0; i < sOrig.size(); i++) {
        char ch = sOrig[i];
        if (ch == '<') {
            sResult += "&lt;";
        } else if (ch == '>') {
            sResult += "&gt;";
        } else {
            sResult += ch;
        }
    }

    return sResult;
}

std::string ShapeParser::UnescapeXmlText(const std::string &sEscaped)
{
    std::string sResult;
    unsigned int i;

    for (i = 0; i < sEscaped.size(); i++) {
        char ch = sEscaped[i];
        if (ch == '&') {
            if (i + 3 < sEscaped.size()) {
                if (sEscaped[i + 1] == 'l' && sEscaped[i + 2] == 't' && sEscaped[i + 3] == ';') {
                    sResult += '<';
                    i++;
                    i++;
                    i++;
                } else if (sEscaped[i + 1] == 'g' && sEscaped[i + 2] == 't' && sEscaped[i + 3] == ';') {
                    sResult += '>';
                    i++;
                    i++;
                    i++;
                }
                else {
                    sResult += ch;
                }
            } else {
                sResult += ch;
            }
        } else {
            sResult += ch;
        }
    }

    return sResult;
}
