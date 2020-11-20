#include "stdafx.h"
#include "CondiExprParser.h"


CondiExprParser::CondiExprParser()
    : m_idStack()
    , m_symbolParsingCallback()
{
}


CondiExprParser::~CondiExprParser()
{
}

bool CondiExprParser::processSegment(const std::vector<IntermediateDataPtr> &idSegment)
{
    bool ret = true;
    std::vector<IntermediateDataPtr> idSegmentResult_ComparingExpr;
    std::vector<IntermediateDataPtr> idSegmentResult_NotExpr;
    std::vector<IntermediateDataPtr> idSegmentResult_AndExpr;
    std::vector<IntermediateDataPtr> idSegmentResult_OrExpr;

    ret = processSegment_ComparingExpr(idSegment, idSegmentResult_ComparingExpr);
    if (!ret) {
        return false;
    }

    ret = processSegment_NotExpr(idSegmentResult_ComparingExpr, idSegmentResult_NotExpr);
    if (!ret) {
        return false;
    }

    ret = processSegment_AndExpr(idSegmentResult_NotExpr, idSegmentResult_AndExpr);
    if (!ret) {
        return false;
    }

    ret = processSegment_OrExpr(idSegmentResult_AndExpr, idSegmentResult_OrExpr);
    if (!ret) {
        return false;
    }

    //
    // at last
    //
    if (idSegmentResult_OrExpr.size() != 1) {
        return false;
    }
    m_idStack.push(idSegmentResult_OrExpr[0]);

    return ret;
}

bool CondiExprParser::processSegment_ComparingExpr(const std::vector<IntermediateDataPtr> &idInputSegment, std::vector<IntermediateDataPtr> &idOutSegment)
{
    int i = 0;
    while (i < (int)idInputSegment.size()) {
        IntermediateDataPtr idp = idInputSegment[i];
        ++i;
        if (!idp->isComparingOp()) {
            idOutSegment.push_back(idp);
            continue;
        } else {
            if (idOutSegment.size() == 0) {
                return false;
            } else {
                IntermediateDataPtr idpArg1 = idOutSegment.back();
                idOutSegment.pop_back();
                if (!idpArg1->isValue()) {
                    return false;
                } else {
                    if (i >= (int)idInputSegment.size()) {
                        return false;
                    } else {
                        IntermediateDataPtr idpArg2 = idInputSegment[i];
                        ++i;
                        if (!idpArg2->isValue()) {
                            return false;
                        } else {
                            IntermediateDataPtr idpTemp = std::make_shared<IntermediateData>(idpArg1, idpArg2, idp->data.op);
                            idOutSegment.push_back(idpTemp);
                        }
                    }
                }
            }
        }
    }
    if (i != (int)idInputSegment.size()) {
        return false;
    }

    return true;
}

bool CondiExprParser::processSegment_NotExpr(const std::vector<IntermediateDataPtr> &idInputSegment, std::vector<IntermediateDataPtr> &idOutSegment)
{
    int i = 0;
    while (i < (int)idInputSegment.size()) {
        IntermediateDataPtr idp = idInputSegment[i];
        ++i;
        if (*idp != IntermediateData(IntermediateData::IDOT_Not)) {
            idOutSegment.push_back(idp);                
            continue;
        } else {
            if (i >= (int)idInputSegment.size()) {
                return false;
            } else {
                IntermediateDataPtr idpArg2 = idInputSegment[i];
                ++i;
                if (!idpArg2->isValue()) {
                    return false;
                } else {
                    IntermediateDataPtr idpTemp = std::make_shared<IntermediateData>(nullptr, idpArg2, IntermediateData::IDOT_Not);
                    idOutSegment.push_back(idpTemp);
                }
            }
        }
    }
    if (i != (int)idInputSegment.size()) {
        return false;
    }

    return true;
}

bool CondiExprParser::processSegment_AndExpr(const std::vector<IntermediateDataPtr> &idInputSegment, std::vector<IntermediateDataPtr> &idOutSegment)
{
    int i = 0;
    while (i < (int)idInputSegment.size()) {
        IntermediateDataPtr idp = idInputSegment[i];
        ++i;
        if (*idp != IntermediateData(IntermediateData::IDOT_And)) {
            idOutSegment.push_back(idp);                
            continue;
        } else {
            if (idOutSegment.size() == 0) {
                return false;
            } else {
                IntermediateDataPtr idpArg1 = idOutSegment.back();
                idOutSegment.pop_back();
                if (!idpArg1->isValue()) {
                    return false;
                } else {
                    if (i >= (int)idInputSegment.size()) {
                        return false;
                    } else {
                        IntermediateDataPtr idpArg2 = idInputSegment[i];
                        ++i;
                        if (!idpArg2->isValue()) {
                            return false;
                        } else {
                            IntermediateDataPtr idpTemp = std::make_shared<IntermediateData>(idpArg1, idpArg2, IntermediateData::IDOT_And);
                            idOutSegment.push_back(idpTemp);
                        }
                    }
                }
            }
        }
    }
    if (i != (int)idInputSegment.size()) {
        return false;
    }

    return true;
}

bool CondiExprParser::processSegment_OrExpr(const std::vector<IntermediateDataPtr> &idInputSegment, std::vector<IntermediateDataPtr> &idOutSegment)
{
    int i = 0;
    while (i < (int)idInputSegment.size()) {
        IntermediateDataPtr idp = idInputSegment[i];
        ++i;
        if (*idp != IntermediateData(IntermediateData::IDOT_Or)) {
            idOutSegment.push_back(idp);
            continue;
        } else {
            if (idOutSegment.size() == 0) {
                return false;
            } else {
                IntermediateDataPtr idpArg1 = idOutSegment.back();
                idOutSegment.pop_back();
                if (!idpArg1->isValue()) {
                    return false;
                } else {
                    if (i >= (int)idInputSegment.size()) {
                        return false;
                    } else {
                        IntermediateDataPtr idpArg2 = idInputSegment[i];
                        ++i;
                        if (!idpArg2->isValue()) {
                            return false;
                        } else {
                            IntermediateDataPtr idpTemp = std::make_shared<IntermediateData>(idpArg1, idpArg2, IntermediateData::IDOT_Or);
                            idOutSegment.push_back(idpTemp);
                        }
                    }
                }
            }
        }
    }
    if (i != (int)idInputSegment.size()) {
        return false;
    }

    return true;
}

bool CondiExprParser::processIdStackUntilLeftParentheses()
{
    bool ret = true;
    std::vector<IntermediateDataPtr> idSegment;
    bool leftParenthesesFound = false;

    while (!m_idStack.empty()) {
        IntermediateDataPtr idp = m_idStack.top();
        m_idStack.pop();
        if (*idp == IntermediateData("(")) {
            leftParenthesesFound = true;
            break;
        } else {
            idSegment.push_back(idp);
        }
    }
    if (!leftParenthesesFound) {
        return false;
    }

    std::reverse(idSegment.begin(), idSegment.end());
    ret = processSegment(idSegment);
    if (!ret) {
        return false;
    }

    return true;
}

bool CondiExprParser::processIdStackLeftPart()
{
    bool ret = true;
    std::vector<IntermediateDataPtr> idSegment;

    while (!m_idStack.empty()) {
        IntermediateDataPtr  idp = m_idStack.top();
        m_idStack.pop();
        idSegment.push_back(idp);
    }

    std::reverse(idSegment.begin(), idSegment.end());
    ret = processSegment(idSegment);
    if (!ret) {
        return false;
    }

    return true;
}

std::string Trim(const std::string &str, const std::string& whitespace /*= " \t"*/)
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos) {
        return std::string("");
    }

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

bool CondiExprParser::pushVarNameToIdStack(std::string &varName)
{
    if (varName.empty()) {
        return true;
    }

    std::string varNameTrimmed = Trim(varName, " ");
    varName.clear();
    for (char ch : varNameTrimmed) {
        if (ch == ' ') {
            return false;
        }
    }
    m_idStack.push(std::make_shared<IntermediateData>(varNameTrimmed));

    return true;
}

bool CondiExprParser::parserExpr(const std::string &expr)
{
    bool ret = true;
    std::string varName;
    int i = 0;

    for (; i < (int)expr.length(); ++i) {
        char ch = expr[i + 0];
        char chNext = ((i + 1) < (int)expr.length()) ? expr[i + 1] : ' ';
        char chNextNext = ((i + 2) < (int)expr.length()) ? expr[i + 2] : ' ';
        char chNextNextNext = ((i + 3) < (int)expr.length()) ? expr[i + 3] : ' ';

        if (ch == ' ') {
            if (!varName.empty()) {
                varName += ch;
            }
            continue;
        }

        if (ch == '(') {
            if (!pushVarNameToIdStack(varName)) {
                return false;
            }

            m_idStack.push(std::make_shared<IntermediateData>("("));
        } else if (ch == ')') {
            if (!pushVarNameToIdStack(varName)) {
                return false;
            }

            ret = processIdStackUntilLeftParentheses();
            if (!ret) {
                return false;
            }
        } else if (ch == 'n' && chNext == 'o' && chNextNext == 't' && (chNextNextNext == ' ' || chNextNextNext == '(')) {
            if (!pushVarNameToIdStack(varName)) {
                return false;
            }

            m_idStack.push(std::make_shared<IntermediateData>(IntermediateData::IDOT_Not));
            i += 2;
        } else if (ch == 'e' && chNext == 'q' && chNextNext == ' ') {
            if (!pushVarNameToIdStack(varName)) {
                return false;
            }

            m_idStack.push(std::make_shared<IntermediateData>(IntermediateData::IDOT_Equal));
            ++i;
        } else if (ch == 'n' && chNext == 'e' && chNextNext == ' ') {
            if (!pushVarNameToIdStack(varName)) {
                return false;
            }

            m_idStack.push(std::make_shared<IntermediateData>(IntermediateData::IDOT_NotEqual));
            ++i;
        } else if (ch == 'g' && chNext == 'e' && chNextNext == ' ') {
            if (!pushVarNameToIdStack(varName)) {
                return false;
            }

            m_idStack.push(std::make_shared<IntermediateData>(IntermediateData::IDOT_GreaterEqual));
            ++i;
        } else if (ch == 'g' && chNext == 't' && chNextNext == ' ') {
            if (!pushVarNameToIdStack(varName)) {
                return false;
            }

            m_idStack.push(std::make_shared<IntermediateData>(IntermediateData::IDOT_Greater));
            ++i;
        } else if (ch == 'l' && chNext == 'e' && chNextNext == ' ') {
            if (!pushVarNameToIdStack(varName)) {
                return false;
            }

            m_idStack.push(std::make_shared<IntermediateData>(IntermediateData::IDOT_LessEqual));
            ++i;
        } else if (ch == 'l' && chNext == 't' && chNextNext == ' ') {
            if (!pushVarNameToIdStack(varName)) {
                return false;
            }

            m_idStack.push(std::make_shared<IntermediateData>(IntermediateData::IDOT_Less));
            ++i;
        } else if (ch == '&') {
            if (!pushVarNameToIdStack(varName)) {
                return false;
            }

            if (chNext == '&') {
                m_idStack.push(std::make_shared<IntermediateData>(IntermediateData::IDOT_And));
                ++i;
            } else {
                return false;
            }
        } else if (ch == '|') {
            if (!pushVarNameToIdStack(varName)) {
                return false;
            }

            if (chNext == '|') {
                m_idStack.push(std::make_shared<IntermediateData>(IntermediateData::IDOT_Or));
                ++i;
            } else {
                return false;
            }
        } else {
            varName += ch;
        }
    }
    if (!pushVarNameToIdStack(varName)) {
        return false;
    }

    ret = processIdStackLeftPart();
    if (!ret) {
        return false;
    }

    if (m_idStack.size() != 1) {
        return false;
    }

    IntermediateDataPtr idp = m_idStack.top();

    //std::string result;
    //idp->print3(result);
    idp->print1(m_symbolParsingCallback);

    return true;
}

void CondiExprParser::setSymbolParsingCallback(SymbolParsingCallback symbolParsingCallback)
{
    m_symbolParsingCallback = symbolParsingCallback;
}

bool CondiExprParser::evaluate(const std::string &expr, int &exprResult)
{
    bool ret = true;

    ret = parserExpr(expr);
    if (!ret) {
        return ret;
    }

    IntermediateDataPtr idp = m_idStack.top();
    ret = idp->evaluate(m_symbolParsingCallback, exprResult);
    if (!ret) {
        return ret;
    }

    return true;
}
