#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <string>
#include <stack>
#include <vector>
#include <functional>

struct IntermediateData;

typedef std::shared_ptr<IntermediateData> IntermediateDataPtr;
typedef std::function<bool (const std::string &, int &)> SymbolParsingCallback;

struct IntermediateData
{
public:
    enum DataType
    {
        IDDT_Invalid = 0,
        IDDT_Str,
        IDDT_Int,
        IDDT_ComplexData,
        IDOT_Op,
    };

    enum OpType
    {
        IDOT_Invalid = 0,
        IDOT_Equal,
        IDOT_NotEqual,
        IDOT_Greater,
        IDOT_GreaterEqual,
        IDOT_Less,
        IDOT_LessEqual,
        IDOT_And,
        IDOT_Or,
        IDOT_Not,
    };

public:
    IntermediateData(const std::string &s);
    IntermediateData(int n);
    IntermediateData(OpType op);
    IntermediateData(IntermediateDataPtr arg1, IntermediateDataPtr arg2, OpType op);

    bool operator ==(const IntermediateData &rhs) const;
    bool operator !=(const IntermediateData &rhs) const;

public:
    bool isStr() const;
    bool isInt() const;
    bool isComplexData() const;
    bool isSimpleValue() const;
    bool isValue() const;
    bool isComparingOp() const;
    bool isAndOrOp() const;
    bool isNotOp() const;
    bool isOp() const;

public:
    void print1(SymbolParsingCallback symbolParsingCallback, unsigned int outputLevel = 0);
    void print2();
    void print3(std::string &result);

public:
    bool evaluate(SymbolParsingCallback symbolParsingCallback, int &exprResult);

private:
    void printLeadingSpace(unsigned int outputLevel);
    bool evaluateId(SymbolParsingCallback symbolParsingCallback, IntermediateData &id, int &exprResult);

public:
    DataType dataType{IDDT_Invalid};
    struct
    {
        std::string s{};
        int n{0};
        OpType op{IDOT_Invalid};
        struct {
            IntermediateDataPtr arg1{};
            IntermediateDataPtr arg2{};
            IntermediateData::OpType op{};
        } complexData;
    } data;
};
