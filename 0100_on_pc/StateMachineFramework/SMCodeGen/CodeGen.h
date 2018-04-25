#pragma once

#include "stdafx.h"
#include "Shape.h"

enum CG_ErrorCode {
    CG_EC_OK,
    CG_EC_FAILED,
    CG_EC_FAILED_TO_SAVE_FILE,
    CG_EC_INVALID_DEF_TRANSITION,
};

class CodeGen 
{
public:
    CodeGen(std::vector<ShapeState *> &arrShapeStates, std::vector<ShapeTrans *> &arrShapeTranss);
    virtual ~CodeGen();

public:
    void SetInstanceCount(unsigned int nInstanceCount) { m_nInstanceCount = nInstanceCount; }
    void SetPrefix(const std::string &m_sPrefix);
    void SetOutputDir(const std::string &m_sOutputDir);
    
    CG_ErrorCode Generate();

    std::string GetErrorStr(CG_ErrorCode nErrorCode) const;

private:
    CG_ErrorCode GenerateIncHpp();
    CG_ErrorCode GenerateIncCpp();
    CG_ErrorCode GenerateEnginesHpp();
    CG_ErrorCode GenerateEnginesCpp();
    CG_ErrorCode GenerateStatesHpp();
    CG_ErrorCode GenerateStatesCpp();
    CG_ErrorCode GenerateEventHandlerHpp();
    CG_ErrorCode GenerateEventHandlerCpp();
    CG_ErrorCode GenerateEventHandlerImplHpp();
    CG_ErrorCode GenerateEventHandlerImplCpp();
    CG_ErrorCode GenerateBaseStateHpp();
    CG_ErrorCode GenerateBaseStateCpp();

    CG_ErrorCode GenerateStateHpp(ShapeState &oShapeState, std::string &sOutput);
    CG_ErrorCode GenerateStateCpp(ShapeState &oShapeState, std::string &sOutput);
    CG_ErrorCode GenerateInternalRootStateHpp(std::string &sOutput);
    CG_ErrorCode GenerateInternalRootStateCpp(std::string &sOutput);

    CG_ErrorCode CalcAllEventNames();
    CG_ErrorCode CalcAllEventOperations();

    static bool IsValueInSet(const std::string &sValue, const std::set<std::string> &oSet);

private:
    unsigned int m_nInstanceCount;
    std::vector<ShapeState *> &m_arrShapeStates;
    std::vector<ShapeTrans *> &m_arrShapeTranss;
    std::string m_sPrefix;
    std::string m_sOutputDir;
    std::set<std::string> m_arrAllEventNames;
    std::set<std::string> m_arrAllEventOperations;
};
