// ConditionExpressionParser.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CondiExprParser.h"

bool SymbolParser(const std::string &symbal, int &result)
{
    if (symbal == "a") {
        result = 100;
    } else if (symbal == "b") {
        result = 78;
    } else if (symbal == "c") {
        result = 231;
    } else if (symbal == "d") {
        result = 111;
    } else if (symbal == "e") {
        result = 76;
    } else if (symbal == "f") {
        result = 45;
    } else if (symbal == "g") {
        result = 222;
    } else if (symbal == "aa") {
        result = 654;
    } else if (symbal == "bb") {
        result = 1223;
    } else {
        if (symbal.length() >= 1 && isdigit(symbal[0])) {
            result = (int)strtol(symbal.c_str(), NULL, 0);
            return true;
        } else {
            return false;
        }
    }
    return true;
}

int main(int argc, const char *argv[])
{
    int exprResult = -1;

    //std::string expression("a gt b || aa eq bb && (not c le d || e ne f)");
    //std::string expression("(a gt b || aa eq bb) && (not c le d || e ne f)");
    //std::string expression("(a gt b || aa eq bb) && (not (c le d || e ne f))");
    //std::string expression("(a gt b || not aa eq bb) && (not(c le d || e ne f)) && g gt 111");
    std::string expression("(a gt b || not aa eq bb) && (not(c le d || e ne f)) && g gt 111");
    //std::string expression("not (100 gt 21) || 3 eq 5");

    CondiExprParser condiExprParser;
    condiExprParser.setSymbolParsingCallback(SymbolParser);
    bool ret = condiExprParser.evaluate(expression, exprResult);
    printf("\n\nret=%d, exprResult=%d\n", ret, exprResult);

    return 0;
}
