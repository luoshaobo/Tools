#include "stdafx.h"
#include "IntermediateData.h"

IntermediateData::IntermediateData(const std::string &s)
{
    dataType = IDDT_Str;
    data.s = s;
}

IntermediateData::IntermediateData(int n)
{
    dataType = IDDT_Int;
    data.n = n;
}

IntermediateData::IntermediateData(OpType op)
{
    dataType = IDOT_Op;
    data.op = op;
}

IntermediateData::IntermediateData(IntermediateDataPtr arg1, IntermediateDataPtr arg2, OpType op)
{
    dataType = IDDT_ComplexData;
    data.complexData.arg1 = arg1;
    data.complexData.arg2 = arg2;
    data.complexData.op = op;
}

bool IntermediateData::operator ==(const IntermediateData &rhs) const
{
    if (rhs.dataType != dataType) {
        return false;
    }

    if (dataType == IDDT_Str) {
        return rhs.data.s == data.s;
    } else if (dataType == IDDT_Int) {
        return rhs.data.n == data.n;
    } else if (dataType == IDOT_Op) {
        return rhs.data.op == data.op;
    }
    else if (dataType == IDDT_ComplexData) {
        return (rhs.data.complexData.arg1 == data.complexData.arg1
            && rhs.data.complexData.arg2 == data.complexData.arg2
            && rhs.data.complexData.op == data.complexData.op
            );
    } else {
        return false;
    }
}

bool IntermediateData::operator !=(const IntermediateData &rhs) const
{
    return !operator ==(rhs);
}

bool IntermediateData::isStr() const
{
    if (dataType != IDDT_Str) {
        return false;
    }

    return true;
}

bool IntermediateData::isInt() const
{
    if (dataType != IDDT_Int) {
        return false;
    }

    return true;
}

bool IntermediateData::isComplexData() const
{
    if (dataType != IDDT_ComplexData) {
        return false;
    }

    return true;
}

bool IntermediateData::isSimpleValue() const
{
    bool ret = isStr() || isInt();

    return ret;
}

bool IntermediateData::isValue() const
{
    bool ret = isStr() || isInt() || isComplexData();

    return ret;
}

bool IntermediateData::isComparingOp() const
{
    if (dataType != IDOT_Op) {
        return false;
    }

    switch (data.op) {
        case IDOT_Equal:
        case IDOT_NotEqual:
        case IDOT_Greater:
        case IDOT_GreaterEqual:
        case IDOT_Less:
        case IDOT_LessEqual:
            {
                return true;
            }
            break;
        default:
            {
                return false;
            }
            break;
    }
}

bool IntermediateData::isAndOrOp() const
{
    if (dataType != IDOT_Op) {
        return false;
    }

    switch (data.op) {
        case IDOT_And:
        case IDOT_Or:
            {
                return true;
            }
            break;
        default:
            {
                return false;
            }
            break;
    }
}

bool IntermediateData::isNotOp() const
{
    if (dataType != IDOT_Op) {
        return false;
    }

    switch (data.op) {
        case IDOT_Not:
            {
                return true;
            }
            break;
        default:
            {
                return false;
            }
            break;
    }
}

bool IntermediateData::isOp() const
{
    bool ret = isComparingOp() || isAndOrOp() || isNotOp();

    return ret;
}

bool IntermediateData::evaluate(SymbolParsingCallback symbolParsingCallback, int &exprResult)
{
    if (symbolParsingCallback == nullptr) {
        return false;
    }

    return evaluateId(symbolParsingCallback, *this, exprResult);
}

bool IntermediateData::evaluateId(SymbolParsingCallback symbolParsingCallback, IntermediateData &id, int &exprResult)
{
    if (id.isStr()) {
        if (!symbolParsingCallback(id.data.s, exprResult)) {
            return false;
        }
        return true;
    } else if (id.isInt()) {
        exprResult = id.data.n;
        return true;
    }

    if (!id.isComplexData()) {
        false;
    }

    IntermediateDataPtr arg1 = id.data.complexData.arg1;
    OpType op = id.data.complexData.op;
    IntermediateDataPtr arg2 = id.data.complexData.arg2;

    int arg1ExprResult = 0;
    int arg2ExprResult = 0;

    if (arg1 != nullptr) {
        if (!evaluateId(symbolParsingCallback, *arg1, arg1ExprResult)) {
            return false;
        }
    }

    if (arg2 != nullptr) {
        if (!evaluateId(symbolParsingCallback, *arg2, arg2ExprResult)) {
            return false;
        }
    }

    switch (op) {
        case IDOT_Equal:
            {
                exprResult = (arg1ExprResult == arg2ExprResult);
            }
            break;
        case IDOT_NotEqual:
            {
                exprResult = (arg1ExprResult != arg2ExprResult);
            }
            break;
        case IDOT_Greater:
            {
                exprResult = (arg1ExprResult > arg2ExprResult);
            }
            break;
        case IDOT_GreaterEqual:
            {
                exprResult = (arg1ExprResult >= arg2ExprResult);
            }
            break;
        case IDOT_Less:
            {
                exprResult = (arg1ExprResult < arg2ExprResult);
            }
            break;
        case IDOT_LessEqual:
            {
                exprResult = (arg1ExprResult <= arg2ExprResult);
            }
            break;
        case IDOT_Not:
            {
                exprResult = (!arg2ExprResult);
            }
            break;
        case IDOT_And:
            {
                exprResult = (arg1ExprResult && arg2ExprResult);
            }
            break;
        case IDOT_Or:
            {
                exprResult = (arg1ExprResult || arg2ExprResult);
            }
            break;
        default:
            {
                return false;
            }
            break;
    }

    return true;
}

void IntermediateData::printLeadingSpace(unsigned int outputLevel)
{
    unsigned int count = outputLevel * 4;

    for (unsigned int i = 0; i < count; ++i) {
        printf(" ");
    }
}

void IntermediateData::print1(SymbolParsingCallback symbolParsingCallback, unsigned int outputLevel)
{
    switch (dataType) {
        case IDDT_Str:
            {
                printLeadingSpace(outputLevel);
                printf("%s", data.s.c_str());
                int exprResult = 0;
                if (symbolParsingCallback(data.s, exprResult)) {
                    printf("[=%d]", exprResult);
                }
                printf("\n");
            }
            break;
        case IDDT_Int:
            {
                printLeadingSpace(outputLevel);
                printf("%d", data.n);
                printf("\n");
            }
            break;
        case IDOT_Op:
            {
                printLeadingSpace(outputLevel);
                switch (data.op) {
                    case IDOT_Equal:
                        {
                            printf("==");
                        }
                        break;
                    case IDOT_NotEqual:
                        {
                            printf("!=");
                        }
                        break;
                    case IDOT_Greater:
                        {
                            printf(">");
                        }
                        break;
                    case IDOT_GreaterEqual:
                        {
                            printf(">=");
                        }
                        break;
                    case IDOT_Less:
                        {
                            printf("<");
                        }
                        break;
                    case IDOT_LessEqual:
                        {
                            printf("<=");
                        }
                        break;
                    case IDOT_Not:
                        {
                            printf("!");
                        }
                        break;
                    case IDOT_And:
                        {
                            printf("&&");
                        }
                        break;
                    case IDOT_Or:
                        {
                            printf("||");
                        }
                        break;
                    default:
                        {
                            printf("UNKNOWN_SIGN");
                        }
                        break;
                }
                printf("\n");
            }
            break;
        case IDDT_ComplexData:
            {
                if (data.complexData.arg1 != nullptr) {
                    if (data.complexData.arg1->isSimpleValue()) {
                        printLeadingSpace(1);
                    }
                    data.complexData.arg1->print1(symbolParsingCallback, outputLevel + 1);
                }

                IntermediateData(data.complexData.op).print1(symbolParsingCallback, outputLevel + 1);
                
                if (data.complexData.arg2 != nullptr) {
                    if (data.complexData.arg2->isSimpleValue()) {
                        printLeadingSpace(1);
                    }
                    data.complexData.arg2->print1(symbolParsingCallback, outputLevel + 1);
                }
            }
            break;
        default:
            {
                printLeadingSpace(outputLevel);
                printf("UNKNOWN_DATA_TYPE");
                printf("\n");
            }
            break;
    }
}

void IntermediateData::print2()
{
    switch (dataType) {
        case IDDT_Str:
            {
                printf("%s", data.s.c_str());
            }
            break;
        case IDDT_Int:
            {
                printf("%d", data.n);
            }
            break;
        case IDOT_Op:
            {
                switch (data.op) {
                    case IDOT_Equal:
                        {
                            printf("==");
                        }
                        break;
                    case IDOT_NotEqual:
                        {
                            printf("!=");
                        }
                        break;
                    case IDOT_Greater:
                        {
                            printf(">");
                        }
                        break;
                    case IDOT_GreaterEqual:
                        {
                            printf(">=");
                        }
                        break;
                    case IDOT_Less:
                        {
                            printf("<");
                        }
                        break;
                    case IDOT_LessEqual:
                        {
                            printf("<=");
                        }
                        break;
                    case IDOT_Not:
                        {
                            printf("!");
                        }
                        break;
                    case IDOT_And:
                        {
                            printf("&&");
                        }
                        break;
                    case IDOT_Or:
                        {
                            printf("||");
                        }
                        break;
                    default:
                        {
                            printf("UNKNOWN_SIGN");
                        }
                        break;
                }
            }
            break;
        case IDDT_ComplexData:
            {
                printf("(");

                if (data.complexData.arg1 != nullptr) {
                    data.complexData.arg1->print2();
                }

                IntermediateData(data.complexData.op).print2();

                if (data.complexData.arg2 != nullptr) {
                    data.complexData.arg2->print2();
                }

                printf(")");
            }
            break;
        default:
            {
                printf("UNKNOWN_DATA_TYPE");
            }
            break;
    }
}

void IntermediateData::print3(std::string &result)
{
    switch (dataType) {
        case IDDT_Str:
            {
                result += data.s;
                printf("%s\n", result.c_str());
            }
            break;
        case IDDT_Int:
            {
                result += std::to_string(data.n);
                printf("%s\n", result.c_str());
            }
            break;
        case IDOT_Op:
            {
                switch (data.op) {
                    case IDOT_Equal:
                        {
                            result += "==";
                        }
                        break;
                    case IDOT_NotEqual:
                        {
                            result += "!=";
                        }
                        break;
                    case IDOT_Greater:
                        {
                            result += ">";
                        }
                        break;
                    case IDOT_GreaterEqual:
                        {
                            result += ">=";
                        }
                        break;
                    case IDOT_Less:
                        {
                            result += "<";
                        }
                        break;
                    case IDOT_LessEqual:
                        {
                            result += "<=";
                        }
                        break;
                    case IDOT_Not:
                        {
                            result += "!";
                        }
                        break;
                    case IDOT_And:
                        {
                            result += "&&";
                        }
                        break;
                    case IDOT_Or:
                        {
                            result += "||";
                        }
                        break;
                    default:
                        {
                            result += "UNKNOWN_SIGN";
                        }
                        break;
                }
                printf("%s\n", result.c_str());
            }
            break;
        case IDDT_ComplexData:
            {
                if (data.complexData.arg1 != nullptr) {
                    data.complexData.arg1->print3(result);
                }

                IntermediateData(data.complexData.op).print3(result);

                if (data.complexData.arg2 != nullptr) {
                    data.complexData.arg2->print3(result);
                }
            }
            break;
        default:
            {
                result += "UNKNOWN_DATA_TYPE";
                printf("%s\n", result.c_str());
            }
            break;
    }
}
