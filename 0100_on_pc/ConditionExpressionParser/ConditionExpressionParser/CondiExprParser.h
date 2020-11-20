#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <string>
#include <stack>
#include <vector>
#include "IntermediateData.h"

class CondiExprParser
{
public:
    CondiExprParser();
    ~CondiExprParser();

    void setSymbolParsingCallback(SymbolParsingCallback symbolParsingCallback);
    bool evaluate(const std::string &expr, int &exprResult);

private:
    bool parserExpr(const std::string &expr);
    bool processIdStackUntilLeftParentheses();
    bool processIdStackLeftPart();
    bool processSegment(const std::vector<IntermediateDataPtr> &idSegment);
    bool processSegment_ComparingExpr(const std::vector<IntermediateDataPtr> &idInputSegment, std::vector<IntermediateDataPtr> &idOutSegment);
    bool processSegment_NotExpr(const std::vector<IntermediateDataPtr> &idInputSegment, std::vector<IntermediateDataPtr> &idOutSegment);
    bool processSegment_AndExpr(const std::vector<IntermediateDataPtr> &idInputSegment, std::vector<IntermediateDataPtr> &idOutSegment);
    bool processSegment_OrExpr(const std::vector<IntermediateDataPtr> &idInputSegment, std::vector<IntermediateDataPtr> &idOutSegment);
    bool pushVarNameToIdStack(std::string &varName);

private:
    std::stack<IntermediateDataPtr> m_idStack;
    SymbolParsingCallback m_symbolParsingCallback;
};

