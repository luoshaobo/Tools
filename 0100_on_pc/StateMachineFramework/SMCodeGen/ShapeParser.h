#pragma once

#include "stdafx.h"
#include "Shape.h"

enum SP_ErrorCode {
    SP_EC_OK,
    SP_EC_FAILED,
    SP_EC_NULL_PTR,
    SP_EC_OUT_OF_ARRAY_SCOPE,
    SP_EC_FAILED_TO_PARSE_XML,
    SP_EC_NO_SHAPE_TYPE,
    SP_EC_UNKNOWN_SHAPE_TYPE,
    SP_EC_NO_PINX,
    SP_EC_WRONG_FORMAT_FOR_PINX,
    SP_EC_NO_PINY,
    SP_EC_WRONG_FORMAT_FOR_PINY,
    SP_EC_NO_WIDTH,
    SP_EC_WRONG_FORMAT_FOR_WIDTH,
    SP_EC_NO_HEIGHT,
    SP_EC_WRONG_FORMAT_FOR_HEIGHT,
    SP_EC_NO_BEGINX,
    SP_EC_WRONG_FORMAT_FOR_BEGINX,
    SP_EC_NO_BEGINY,
    SP_EC_WRONG_FORMAT_FOR_BEGINY,
    SP_EC_NO_ENDX,
    SP_EC_WRONG_FORMAT_FOR_ENDX,
    SP_EC_NO_ENDY,
    SP_EC_WRONG_FORMAT_FOR_ENDY,
    SP_EC_NO_TEXT,
    SP_EC_NO_STATE_NAME,
    SP_EC_INVALID_STATE_NAME,
    SP_EC_NO_BEGIN_SHAPE_STATE,
    SP_EC_NO_END_SHAPE_STATE,
    SP_EC_WRONG_TRANS_ITEM_FORMAT,
    SP_EC_INVALID_DEF_TRANSITION,
    SP_EC_INVALID_EVENT_NAME,
    SP_EC_INVALID_EVENT_CONDITION,
    SP_EC_INVALID_EVENT_OPERATION,
    SP_EC_SAME_STATE_NANE,
    SP_EC_SAME_OR_INCLUSION_EVENTS_IN_STATE,
    SP_EC_NO_ROOT_STATE,
};

class ShapeParser
{
public:
    ShapeParser(const std::string &sShapesXml);
    virtual ~ShapeParser();

public:
    SP_ErrorCode DoParsing();
    std::string GetErrorStr(SP_ErrorCode nErrorCode) const;
    std::string GetLastErrorXml() const { return m_sLastErrorXml; }
    SP_ErrorCode Dump(std::string &sOutputContent);

    std::vector<ShapeState> &GetShapeStates() { return m_arrShapeStates; }
    std::vector<ShapeTrans> &GetShapeTranss() { return m_arrShapeTranss; }

private:
    SP_ErrorCode HandleShapeState(ShapeState &oShapeState);
    SP_ErrorCode HandleShapeTrans(ShapeTrans &oShapeTrans);
    SP_ErrorCode BuildRelationship();

    SP_ErrorCode InsertShapeStateToSortedList(ShapeState &oShapeState);
    SP_ErrorCode FindParentShapeState(unsigned int nShapeStateIndex, ShapeState **ppParentShapeState);
    SP_ErrorCode FindBeginAndEndShapeState(ShapeTrans &oShapeTrans, ShapeState **ppBeginShapeState, ShapeState **ppEndShapeState);
    SP_ErrorCode ParseDoubleData(const std::string &sValue, double &dResult);
    SP_ErrorCode ParseTransItem(const std::string &sText, TransItem &oTransItem);

    bool IsPointOnShapeStateBorder(double dX, double dY, const ShapeState &oShapeState);
    bool IsStateNameValid(const std::string &sStateName) const;
    bool IsEventNameValid(const std::string &sEventName) const;
    bool IsEventConditionValid(const std::string &sEventCondition) const;
    bool IsEventOperationValid(const std::string &sEventOperation) const;
    bool AreThereSameStateNames();
    bool AreThereSameOrInclusionEvents();

    static std::string EscapeXmlText(const std::string &sOrig);
    static std::string UnescapeXmlText(const std::string &sEscaped);

private:
    std::string m_sShapesXml;
    std::vector<ShapeState> m_arrShapeStates;
    std::vector<ShapeTrans> m_arrShapeTranss;
    ShapeState *m_pShapeTree;
    std::string m_sLastErrorXml;
};
