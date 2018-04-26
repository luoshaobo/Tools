#pragma once

#include "stdafx.h"

struct TransItem {
    TransItem()
        : sEvent()
        , sCondition()
        , sOperation()
    {}

    std::string sEvent;
    std::string sCondition;
    std::string sOperation;
};

struct ShapeState;
struct ShapeTrans;

struct ShapeState {
    ShapeState() 
        : bIsRootStateInSubDiagram(false)
        , sXmlContent()
        , dPinX(0.0)
        , dPinY(0.0)
        , dWidth(0.0)
        , dHeight(0.0)
        , sText()
        , sStateName()
        , arrInternalTransItems()
        , pParentShapeState(NULL)
        , arrChildShapeStates()
        , arrShapeTransFromMe()
        , arrShapeTransToMe()
    {}

    bool bIsRootStateInSubDiagram;

    std::string sXmlContent;

    double dPinX;
    double dPinY;
    double dWidth;
    double dHeight;
    std::string sText;

    std::string sStateName;
    std::vector<TransItem> arrInternalTransItems;

    ShapeState *pParentShapeState;
    std::vector<ShapeState *> arrChildShapeStates;

    std::vector<ShapeTrans *> arrShapeTransFromMe;
    std::vector<ShapeTrans *> arrShapeTransToMe;
};

struct ShapeTrans {
    ShapeTrans()
        : sXmlContent()
        , dBeginX(0.0)
        , dBeginY(0.0)
        , dEndX(0.0)
        , dEndY(0.0)
        , sText()
        , arrTransItems()
        , pBeginShapeState(NULL)
        , pEndShapeState(NULL)
    {}

    std::string sXmlContent;

    double dBeginX;
    double dBeginY;
    double dEndX;
    double dEndY;
    std::string sText;

    std::vector<TransItem> arrTransItems;

    ShapeState *pBeginShapeState;
    ShapeState *pEndShapeState;
};
