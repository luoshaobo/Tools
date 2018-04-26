#include "CodeGen.h"

#define DIR_SEP                                                 "\\"
#define NEW_LINE                                                "\r\n"
#define CONDITION_ELSE                                          "else"
#define CONDITION_ELSE_CPP_COMMENT                              "/*else*/"

#define CG_ENGINE_CLASS_NAME(engine)                            (std::string("Engine_") + engine)
#define CG_STATE_CLASS_NAME(state)                              (std::string("State_") + state)

#define CG_INTERNAL_ROOT_STATE_NAME                             std::string("_InternalRoot_")

#define CG_EVENT_HANDLER_CLASS_NAME                             (m_sPrefix + "_EventHandler")
#define CG_EVENT_HANDLER_IMPL_CLASS_NAME                        (m_sPrefix + "_EventHandlerImpl")
#define CG_BASE_STATE_CLASS_NAME                                (m_sPrefix + "_BaseState")

#define CG_INC_HPP_FILE_NAME                                    (m_sPrefix + "_afx.hpp")
#define CG_INC_CPP_FILE_NAME                                    (m_sPrefix + "_afx.cpp")
#define CG_ENGINES_HPP_FILE_NAME                                (m_sPrefix + "_Engines.hpp")
#define CG_ENGINES_CPP_FILE_NAME                                (m_sPrefix + "_Engines.cpp")
#define CG_STAGES_HPP_FILE_NAME                                 (m_sPrefix + "_States.hpp")
#define CG_STAGES_CPP_FILE_NAME                                 (m_sPrefix + "_States.cpp")
#define CG_EVENT_HANDLER_HPP_FILE_NAME                          (CG_EVENT_HANDLER_CLASS_NAME + ".hpp")
#define CG_EVENT_HANDLER_CPP_FILE_NAME                          (CG_EVENT_HANDLER_CLASS_NAME + ".cpp")
#define CG_EVENT_HANDLER_IMPL_HPP_FILE_NAME                     (CG_EVENT_HANDLER_IMPL_CLASS_NAME + ".hpp")
#define CG_EVENT_HANDLER_IMPL_CPP_FILE_NAME                     (CG_EVENT_HANDLER_IMPL_CLASS_NAME + ".cpp")
#define CG_BASE_STATE_HPP_FILE_NAME                             (CG_BASE_STATE_CLASS_NAME + ".hpp")
#define CG_BASE_STATE_CPP_FILE_NAME                             (CG_BASE_STATE_CLASS_NAME + ".cpp")

#define CG_INC_HPP_FILE_INC_ONCE_MACRO                          TK_Tools::UpperCase(m_sPrefix + "_afx_hpp")
#define CG_ENGINES_HPP_FILE_INC_ONCE_MACRO                      TK_Tools::UpperCase(m_sPrefix + "_Engines_hpp")
#define CG_STAGES_HPP_FILE_INC_ONCE_MACRO                       TK_Tools::UpperCase(m_sPrefix + "_States_hpp")
#define CG_EVENT_HANDLER_HPP_FILE_INC_ONCE_MACRO                TK_Tools::UpperCase(CG_EVENT_HANDLER_CLASS_NAME + "_hpp")
#define CG_EVENT_HANDLER_IMPL_HPP_FILE_INC_ONCE_MACRO           TK_Tools::UpperCase(CG_EVENT_HANDLER_IMPL_CLASS_NAME + "_hpp")
#define CG_BASE_STATE_HPP_FILE_INC_ONCE_MACRO                   TK_Tools::UpperCase(CG_BASE_STATE_CLASS_NAME + "_hpp")

#define CG_INC_HPP_PATH                                         (m_sOutputDir + DIR_SEP + CG_INC_HPP_FILE_NAME)
#define CG_INC_CPP_PATH                                         (m_sOutputDir + DIR_SEP + CG_INC_CPP_FILE_NAME)
#define CG_ENGINES_HPP_PATH                                     (m_sOutputDir + DIR_SEP + CG_ENGINES_HPP_FILE_NAME)
#define CG_ENGINES_CPP_PATH                                     (m_sOutputDir + DIR_SEP + CG_ENGINES_CPP_FILE_NAME)
#define CG_STAGES_HPP_PATH                                      (m_sOutputDir + DIR_SEP + CG_STAGES_HPP_FILE_NAME)
#define CG_STAGES_CPP_PATH                                      (m_sOutputDir + DIR_SEP + CG_STAGES_CPP_FILE_NAME)
#define CG_EVENT_HANDLER_HPP_PATH                               (m_sOutputDir + DIR_SEP + CG_EVENT_HANDLER_HPP_FILE_NAME)
#define CG_EVENT_HANDLER_CPP_PATH                               (m_sOutputDir + DIR_SEP + CG_EVENT_HANDLER_CPP_FILE_NAME)
#define CG_EVENT_HANDLER_IMPL_HPP_PATH                          (m_sOutputDir + DIR_SEP + CG_EVENT_HANDLER_IMPL_HPP_FILE_NAME)
#define CG_EVENT_HANDLER_IMPL_CPP_PATH                          (m_sOutputDir + DIR_SEP + CG_EVENT_HANDLER_IMPL_CPP_FILE_NAME)
#define CG_BASE_STATE_HPP_PATH                                  (m_sOutputDir + DIR_SEP + CG_BASE_STATE_HPP_FILE_NAME)
#define CG_BASE_STATE_CPP_PATH                                  (m_sOutputDir + DIR_SEP + CG_BASE_STATE_CPP_FILE_NAME)


#define CG_ESI_ITEM(err_name)       { CG_EC_ ## err_name, "CG_EC_" #err_name }

static const struct CG_ErrorStringItem {
    CG_ErrorCode nErrorCode;
    const char *pErrorStr;
} CG_ErrorStringMap[] = {
    CG_ESI_ITEM(OK),
    CG_ESI_ITEM(FAILED),
    CG_ESI_ITEM(FAILED_TO_SAVE_FILE),
    CG_ESI_ITEM(INVALID_DEF_TRANSITION),
};

CodeGen::CodeGen(std::vector<ShapeState *> &arrShapeStates, std::vector<ShapeTrans *> &arrShapeTranss)
    : m_arrShapeStates(arrShapeStates)
    , m_arrShapeTranss(arrShapeTranss)
    , m_sPrefix("UNKNOWN")
    , m_sOutputDir("tmp")
    , m_arrAllEventNames() 
    , m_arrAllEventOperations() 
{

}

CodeGen::~CodeGen()
{

}

void CodeGen::SetPrefix(const std::string &sPrefix)
{
    m_sPrefix = sPrefix;
}

void CodeGen::SetOutputDir(const std::string &sOutputDir)
{
    m_sOutputDir = sOutputDir;
}

CG_ErrorCode CodeGen::Generate()
{
    CG_ErrorCode nErrorCode = CG_EC_FAILED;

    nErrorCode = GenerateIncHpp();
    if (nErrorCode != CG_EC_OK) {
        goto ERR1;
    }

    nErrorCode = GenerateIncCpp();
    if (nErrorCode != CG_EC_OK) {
        goto ERR1;
    }

    nErrorCode = GenerateEnginesHpp();
    if (nErrorCode != CG_EC_OK) {
        goto ERR1;
    }

    nErrorCode = GenerateEnginesCpp();
    if (nErrorCode != CG_EC_OK) {
        goto ERR1;
    }

    nErrorCode = GenerateStatesHpp();
    if (nErrorCode != CG_EC_OK) {
        goto ERR1;
    }

    nErrorCode = GenerateStatesCpp();
    if (nErrorCode != CG_EC_OK) {
        goto ERR1;
    }

    nErrorCode = GenerateEventHandlerHpp();
    if (nErrorCode != CG_EC_OK) {
        goto ERR1;
    }

    nErrorCode = GenerateEventHandlerCpp();
    if (nErrorCode != CG_EC_OK) {
        goto ERR1;
    }

    nErrorCode = GenerateEventHandlerImplHpp();
    if (nErrorCode != CG_EC_OK) {
        goto ERR1;
    }

    nErrorCode = GenerateEventHandlerImplCpp();
    if (nErrorCode != CG_EC_OK) {
        goto ERR1;
    }

    nErrorCode = GenerateBaseStateHpp();
    if (nErrorCode != CG_EC_OK) {
        goto ERR1;
    }

    nErrorCode = GenerateBaseStateCpp();
    if (nErrorCode != CG_EC_OK) {
        goto ERR1;
    }

    nErrorCode = CG_EC_OK;
ERR1:
    return nErrorCode;
}

CG_ErrorCode CodeGen::GenerateIncHpp()
{
    CG_ErrorCode nErrorCode = CG_EC_FAILED;
    std::string sFileContent;
    unsigned int i;
    bool bSuc;

    nErrorCode = CalcAllEventNames();
    if (nErrorCode != CG_EC_OK) {
        goto ERR1;
    }

    sFileContent += TK_Tools::FormatStr("#ifndef %s", CG_INC_HPP_FILE_INC_ONCE_MACRO.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#define %s", CG_INC_HPP_FILE_INC_ONCE_MACRO.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("#include \"SMF_afx.h\"") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"SMF_BaseEngine.h\"") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"SMF_BaseState.h\"") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"SMF_BaseEventQueue.h\"") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"%s\"", CG_STAGES_HPP_FILE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"%s\"", CG_ENGINES_HPP_FILE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("#ifndef %s_INSTANCE_COUNT", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#define %s_INSTANCE_COUNT 1", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#endif") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("namespace %s {", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("enum {") + NEW_LINE;
    for (std::set<std::string>::iterator it = m_arrAllEventNames.begin(); it != m_arrAllEventNames.end(); ++it) {
        if (!(*it).empty()) {
            if (TK_Tools::CompareNoCase(*it, "ENTRY") == 0) {
                sFileContent += TK_Tools::FormatStr("    SMF_D_EVENT_ID(%s) = SMF_D_EVENT_ID(%s),", (*it).c_str(), "_ENTRY") + NEW_LINE;
            } else if (TK_Tools::CompareNoCase(*it, "EXIT") == 0) {
                sFileContent += TK_Tools::FormatStr("    SMF_D_EVENT_ID(%s) = SMF_D_EVENT_ID(%s),", (*it).c_str(), "_EXIT") + NEW_LINE;
            }
        }
    }
    
    sFileContent += TK_Tools::FormatStr("    SMF_D_EVENT_ID(USR_INVALID) = SMF_EVTID_USR_BASE,") + NEW_LINE;
    for (std::set<std::string>::iterator it = m_arrAllEventNames.begin(); it != m_arrAllEventNames.end(); ++it) {
        if (!(*it).empty()) {
            if (TK_Tools::CompareNoCase(*it, "ENTRY") != 0 && TK_Tools::CompareNoCase(*it, "EXIT") != 0) {
                sFileContent += TK_Tools::FormatStr("    SMF_D_EVENT_ID(%s),", (*it).c_str()) + NEW_LINE;
            }
        }
    }
    sFileContent += TK_Tools::FormatStr("};") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("SMF_DECLARE_ENGINE(MainEngine)") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("SMF_DECLARE_STATE(%s)", CG_INTERNAL_ROOT_STATE_NAME.c_str()) + NEW_LINE;
    for (i = 0; i < m_arrShapeStates.size(); i++) {
        ShapeState &oShapeState = *m_arrShapeStates[i];
        sFileContent += TK_Tools::FormatStr("SMF_DECLARE_STATE(%s)", oShapeState.sStateName.c_str()) + NEW_LINE;
    }
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("} // namespace %s {", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("#endif // #define %s", CG_INC_HPP_FILE_INC_ONCE_MACRO.c_str()) + NEW_LINE;

    bSuc = TK_Tools::SaveToFile(CG_INC_HPP_PATH, sFileContent);
    if (!bSuc) {
        nErrorCode = CG_EC_FAILED_TO_SAVE_FILE;
        goto ERR1;
    }

    nErrorCode = CG_EC_OK;
ERR1:
    return nErrorCode;
}

CG_ErrorCode CodeGen::GenerateIncCpp()
{
    CG_ErrorCode nErrorCode = CG_EC_FAILED;
    std::string sFileContent;
    unsigned int i;
    bool bSuc;

    nErrorCode = CalcAllEventNames();
    if (nErrorCode != CG_EC_OK) {
        goto ERR1;
    }
    
    sFileContent += TK_Tools::FormatStr("#include \"stdafx.h\"") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"SMF_afx.h\"") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"%s\"", CG_INC_HPP_FILE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("namespace %s {", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("SMF_DEFINE_ENGINE(MainEngine, %s_INSTANCE_COUNT)", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("SMF_DEFINE_STATE(MainEngine, %s, %s_INSTANCE_COUNT)", CG_INTERNAL_ROOT_STATE_NAME.c_str(), m_sPrefix.c_str()) + NEW_LINE;
    for (i = 0; i < m_arrShapeStates.size(); i++) {
        ShapeState &oShapeState = *m_arrShapeStates[i];
        sFileContent += TK_Tools::FormatStr("SMF_DEFINE_STATE(MainEngine, %s, %s_INSTANCE_COUNT)", oShapeState.sStateName.c_str(), m_sPrefix.c_str()) + NEW_LINE;
    }
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("} // namespace %s {", m_sPrefix.c_str()) + NEW_LINE;

    bSuc = TK_Tools::SaveToFile(CG_INC_CPP_PATH, sFileContent);
    if (!bSuc) {
        nErrorCode = CG_EC_FAILED_TO_SAVE_FILE;
        goto ERR1;
    }

    nErrorCode = CG_EC_OK;
ERR1:
    return nErrorCode;
}

CG_ErrorCode CodeGen::GenerateEnginesHpp()
{
    CG_ErrorCode nErrorCode = CG_EC_FAILED;
    std::string sFileContent;
    bool bSuc;

    sFileContent += TK_Tools::FormatStr("#ifndef %s", CG_ENGINES_HPP_FILE_INC_ONCE_MACRO.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#define %s", CG_ENGINES_HPP_FILE_INC_ONCE_MACRO.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("#include \"SMF_BaseEngine.h\"") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("namespace %s {", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("class %s : public SMF_BaseEngine", CG_ENGINE_CLASS_NAME("MainEngine").c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("{") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("public:") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    %s(const std::string &sEngineName, size_t nIndex) : SMF_BaseEngine(sEngineName, nIndex) {}", CG_ENGINE_CLASS_NAME("MainEngine").c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    virtual SMF_EventId GetEventIdByName(const std::string &sEventName) const;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    virtual std::string GetEventNameById(SMF_EventId nEventId) const;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("protected:") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    virtual SMF_ErrorCode OnInitStateMachine();") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    virtual SMF_ErrorCode OnDeinitStateMachine();") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("};") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("} // namespace %s {", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("#endif // #define %s", CG_ENGINES_HPP_FILE_INC_ONCE_MACRO.c_str()) + NEW_LINE;

    bSuc = TK_Tools::SaveToFile(CG_ENGINES_HPP_PATH, sFileContent);
    if (!bSuc) {
        nErrorCode = CG_EC_FAILED_TO_SAVE_FILE;
        goto ERR1;
    }

    nErrorCode = CG_EC_OK;
ERR1:
    return nErrorCode;
}

CG_ErrorCode CodeGen::GenerateEnginesCpp()
{
    CG_ErrorCode nErrorCode = CG_EC_FAILED;
    std::string sFileContent;
    bool bSuc;
    unsigned int i;

    sFileContent += TK_Tools::FormatStr("#include \"%s\"", CG_INC_HPP_FILE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"%s\"", CG_ENGINES_HPP_FILE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"TK_Tools.h\"") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("#define %s_EVENT_MAP_ITEM(event_name)  { #event_name, SMF_D_EVENT_ID(event_name) }", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("namespace %s {", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("static struct EventMap {") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    const char *pEventName;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    SMF_EventId nEventId;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("} g_arrEventMap[] = {") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    %s_EVENT_MAP_ITEM(_INVALID),", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    %s_EVENT_MAP_ITEM(_GOTO_SELF),", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    %s_EVENT_MAP_ITEM(_GOTO_PARENT),", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    %s_EVENT_MAP_ITEM(_GOTO_DEF_CHILD),", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    %s_EVENT_MAP_ITEM(_GOTO_FIRST_CHILD),", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    %s_EVENT_MAP_ITEM(_ENTRY),", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    %s_EVENT_MAP_ITEM(_EXIT),", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    for (std::set<std::string>::iterator it = m_arrAllEventNames.begin(); it != m_arrAllEventNames.end(); ++it) {
        if (!(*it).empty()) {
            sFileContent += TK_Tools::FormatStr("    %s_EVENT_MAP_ITEM(%s),", m_sPrefix.c_str(), (*it).c_str()) + NEW_LINE;
        }
    }
    sFileContent += TK_Tools::FormatStr("};") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("SMF_ErrorCode %s::OnInitStateMachine()", CG_ENGINE_CLASS_NAME("MainEngine").c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("{") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    SMF_ErrorCode nErrorCode = SMF_ERR_OK;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    for (i = 0; i < m_arrShapeStates.size(); i++) {
        ShapeState &oShapeState = *m_arrShapeStates[i];
        if (oShapeState.pParentShapeState != NULL) {
            ShapeState &oParentShapeState = *oShapeState.pParentShapeState;
            sFileContent += TK_Tools::FormatStr("    SMF_ADD_STATE_TO_PARENT_STATE(%s, %s, GetIndex());", oShapeState.sStateName.c_str(), oParentShapeState.sStateName.c_str()) + NEW_LINE;
        } else {
            sFileContent += TK_Tools::FormatStr("    SMF_ADD_STATE_TO_PARENT_STATE(%s, %s, GetIndex());", oShapeState.sStateName.c_str(), CG_INTERNAL_ROOT_STATE_NAME.c_str()) + NEW_LINE;
        }
    }
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    SMF_SET_STATE_MACHINE_TO_ENGINE(%s, MainEngine, GetIndex());", CG_INTERNAL_ROOT_STATE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    return nErrorCode;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("}") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("SMF_ErrorCode Engine_MainEngine::OnDeinitStateMachine()") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("{") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    SMF_ErrorCode nErrorCode = SMF_ERR_OK;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    return nErrorCode;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("}") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("SMF_EventId %s::GetEventIdByName(const std::string &sEventName) const", CG_ENGINE_CLASS_NAME("MainEngine").c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("{") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    unsigned int i;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    unsigned int nCount;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    SMF_EventId nEventId = SMF_D_EVENT_ID(_INVALID);") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    nCount = TK_ARR_LEN(g_arrEventMap);") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    for (i = 0; i < nCount; i++) {") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("        if (TK_Tools::CompareNoCase(g_arrEventMap[i].pEventName, sEventName) == 0) {") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("            nEventId = g_arrEventMap[i].nEventId;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("            break;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("        }") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    }") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    return nEventId;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("}") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("std::string %s::GetEventNameById(SMF_EventId nEventId) const", CG_ENGINE_CLASS_NAME("MainEngine").c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("{") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    std::string sResult;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    unsigned int i;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    unsigned int nCount;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    nCount = TK_ARR_LEN(g_arrEventMap);") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    for (i = 0; i < nCount; i++) {") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("        if (g_arrEventMap[i].nEventId == nEventId) {") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("            sResult = g_arrEventMap[i].pEventName;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("            break;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("        }") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    }") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    if (sResult.empty()) {") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("        sResult = TK_Tools::FormatStr(\"%%lu\", nEventId);") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    }") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    return sResult;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("}") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("} // namespace %s {", m_sPrefix.c_str()) + NEW_LINE;

    bSuc = TK_Tools::SaveToFile(CG_ENGINES_CPP_PATH, sFileContent);
    if (!bSuc) {
        nErrorCode = CG_EC_FAILED_TO_SAVE_FILE;
        goto ERR1;
    }

    nErrorCode = CG_EC_OK;
ERR1:
    return nErrorCode;
}

CG_ErrorCode CodeGen::GenerateStatesHpp()
{
    CG_ErrorCode nErrorCode = CG_EC_FAILED;
    std::string sFileContent;
    std::string sFileStateContent;
    unsigned int i;
    bool bSuc;

    sFileContent += TK_Tools::FormatStr("#ifndef %s", CG_STAGES_HPP_FILE_INC_ONCE_MACRO.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#define %s", CG_STAGES_HPP_FILE_INC_ONCE_MACRO.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"SMF_afx.h\"") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"SMF_BaseState.h\"") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"SMF_BaseEngine.h\"") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"%s\"", CG_INC_HPP_FILE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"%s\"", CG_BASE_STATE_HPP_FILE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"%s\"", CG_EVENT_HANDLER_HPP_FILE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"%s\"", CG_EVENT_HANDLER_IMPL_HPP_FILE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("namespace %s {", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    nErrorCode = GenerateInternalRootStateHpp(sFileStateContent);
    if (nErrorCode != CG_EC_OK) {
        goto ERR1;
    }
    
    sFileContent += sFileStateContent;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    for (i = 0; i < m_arrShapeStates.size(); i++) {
        ShapeState &oShapeState = *m_arrShapeStates[i];
        if (!oShapeState.bIsRootStateInSubDiagram) {
            nErrorCode = GenerateStateHpp(oShapeState, sFileStateContent);
            if (nErrorCode != CG_EC_OK) {
                goto ERR1;
            }
            
            sFileContent += sFileStateContent;
            sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
        }
    }

    sFileContent += TK_Tools::FormatStr("} // namespace %s {", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("#endif // #define %s", CG_STAGES_HPP_FILE_INC_ONCE_MACRO.c_str()) + NEW_LINE;

    bSuc = TK_Tools::SaveToFile(CG_STAGES_HPP_PATH, sFileContent);
    if (!bSuc) {
        nErrorCode = CG_EC_FAILED_TO_SAVE_FILE;
        goto ERR1;
    }

    nErrorCode = CG_EC_OK;
ERR1:
    return nErrorCode;
}

CG_ErrorCode CodeGen::GenerateStatesCpp()
{
    CG_ErrorCode nErrorCode = CG_EC_FAILED;
    std::string sFileContent;
    std::string sFileStateContent;
    unsigned int i;
    bool bSuc;

    sFileContent += TK_Tools::FormatStr("#include \"%s\"", CG_INC_HPP_FILE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"%s\"", CG_STAGES_HPP_FILE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("namespace %s {", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("//") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("// NOTE:") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("// 1) The transitions can be defined by the macro:") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("//        SMF_DEFINE_STATE_TRANS_ITEM(event_id,condition,operation,dest_state_name)") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("//    with the arguments as below:") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("//        event_id: the event ID without prefix.") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("//        condition: the condition which will be not changed.") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("//        operation: the operation which will be executed on the event.") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("//        dest_state_name: the new state name without prefix.  ") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("//") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    nErrorCode = GenerateInternalRootStateCpp(sFileStateContent);
    if (nErrorCode != CG_EC_OK) {
        goto ERR1;
    }
    
    sFileContent += sFileStateContent;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    for (i = 0; i < m_arrShapeStates.size(); i++) {
        ShapeState &oShapeState = *m_arrShapeStates[i];
        if (!oShapeState.bIsRootStateInSubDiagram) {
            nErrorCode = GenerateStateCpp(oShapeState, sFileStateContent);
            if (nErrorCode != CG_EC_OK) {
                goto ERR1;
            }
            
            sFileContent += sFileStateContent;
            sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
        }
    }

    sFileContent += TK_Tools::FormatStr("} // namespace %s {", m_sPrefix.c_str()) + NEW_LINE;

    bSuc = TK_Tools::SaveToFile(CG_STAGES_CPP_PATH, sFileContent);
    if (!bSuc) {
        nErrorCode = CG_EC_FAILED_TO_SAVE_FILE;
        goto ERR1;
    }

    nErrorCode = CG_EC_OK;
ERR1:
    return nErrorCode;
}

CG_ErrorCode CodeGen::GenerateEventHandlerHpp()
{
    CG_ErrorCode nErrorCode = CG_EC_FAILED;
    std::string sFileContent;
    bool bSuc;

    nErrorCode = CalcAllEventOperations();
    if (nErrorCode != CG_EC_OK) {
        goto ERR1;
    }

    sFileContent += TK_Tools::FormatStr("#ifndef %s", CG_EVENT_HANDLER_HPP_FILE_INC_ONCE_MACRO.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#define %s", CG_EVENT_HANDLER_HPP_FILE_INC_ONCE_MACRO.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("#include \"SMF_afx.h\"") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"SMF_BaseEngine.h\"") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"SMF_BaseState.h\"") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"SMF_BaseEventHandler.h\"") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("class %s;", CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("class %s : public SMF_BaseEventHandler", CG_EVENT_HANDLER_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("{") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("protected:") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    %s(size_t nIndex);", CG_EVENT_HANDLER_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("public:") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    virtual ~%s();", CG_EVENT_HANDLER_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    static %s &GetInstance(size_t nIndex);", CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("public:") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    virtual SMF_ErrorCode Reset() { return SMF_BaseEventHandler::Reset(); }") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    
    sFileContent += TK_Tools::FormatStr("public:") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    //") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    // Event handlers") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    //") + NEW_LINE;
    for (std::set<std::string>::iterator it = m_arrAllEventOperations.begin(); it != m_arrAllEventOperations.end(); ++it) {
        if (!(*it).empty()) {
            sFileContent += TK_Tools::FormatStr("    virtual SMF_ErrorCode %s(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }", (*it).c_str()) + NEW_LINE;
        }
    }
    sFileContent += TK_Tools::FormatStr("};") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("#endif // #define %s", CG_EVENT_HANDLER_HPP_FILE_INC_ONCE_MACRO.c_str()) + NEW_LINE;
    
    bSuc = TK_Tools::SaveToFile(CG_EVENT_HANDLER_HPP_PATH, sFileContent);
    if (!bSuc) {
        nErrorCode = CG_EC_FAILED_TO_SAVE_FILE;
        goto ERR1;
    }

    nErrorCode = CG_EC_OK;
ERR1:
    return nErrorCode;
}

CG_ErrorCode CodeGen::GenerateEventHandlerCpp()
{
    CG_ErrorCode nErrorCode = CG_EC_FAILED;
    std::string sFileContent;
    bool bSuc;

    nErrorCode = CalcAllEventOperations();
    if (nErrorCode != CG_EC_OK) {
        goto ERR1;
    }

    sFileContent += TK_Tools::FormatStr("#include \"%s\"", CG_INC_HPP_FILE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"%s\"", CG_EVENT_HANDLER_HPP_FILE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"%s\"", CG_EVENT_HANDLER_IMPL_HPP_FILE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("%s::%s(size_t nIndex) : SMF_BaseEventHandler(nIndex)", CG_EVENT_HANDLER_CLASS_NAME.c_str(), CG_EVENT_HANDLER_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("{") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("}") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("%s::~%s()", CG_EVENT_HANDLER_CLASS_NAME.c_str(), CG_EVENT_HANDLER_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("{") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("}") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("%s &%s::GetInstance(size_t nIndex)", CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str(), CG_EVENT_HANDLER_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("{") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    return %s::GetInstance(nIndex);", CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("}") + NEW_LINE;
    
    bSuc = TK_Tools::SaveToFile(CG_EVENT_HANDLER_CPP_PATH, sFileContent);
    if (!bSuc) {
        nErrorCode = CG_EC_FAILED_TO_SAVE_FILE;
        goto ERR1;
    }

    nErrorCode = CG_EC_OK;
ERR1:
    return nErrorCode;
}

CG_ErrorCode CodeGen::GenerateEventHandlerImplHpp()
{
    CG_ErrorCode nErrorCode = CG_EC_FAILED;
    std::string sFileContent;
    bool bSuc;

    nErrorCode = CalcAllEventOperations();
    if (nErrorCode != CG_EC_OK) {
        goto ERR1;
    }

    sFileContent += TK_Tools::FormatStr("#ifndef %s", CG_EVENT_HANDLER_IMPL_HPP_FILE_INC_ONCE_MACRO.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#define %s", CG_EVENT_HANDLER_IMPL_HPP_FILE_INC_ONCE_MACRO.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("#include \"%s\"", CG_EVENT_HANDLER_HPP_FILE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("struct SMF_OpArg;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("class %s : public %s", CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str(), CG_EVENT_HANDLER_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("{") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("public:") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    %s(size_t nIndex);", CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("public:") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    virtual ~%s();", CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    static %s &GetInstance(size_t nIndex);", CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    
    sFileContent += TK_Tools::FormatStr("public:") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    virtual SMF_ErrorCode Reset();") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    
    sFileContent += TK_Tools::FormatStr("protected:") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    //") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    // implementation interfaces") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    //") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("public:") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    //") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    // conditions") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    //") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("private:") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    //") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    // implementations") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    //") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("private:") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    //") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    // data") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    //") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("};") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("#endif // #define %s", CG_EVENT_HANDLER_IMPL_HPP_FILE_INC_ONCE_MACRO.c_str()) + NEW_LINE;
    
    if (!TK_Tools::FileExists(CG_EVENT_HANDLER_IMPL_HPP_PATH)) {
        bSuc = TK_Tools::SaveToFile(CG_EVENT_HANDLER_IMPL_HPP_PATH, sFileContent);
        if (!bSuc) {
            nErrorCode = CG_EC_FAILED_TO_SAVE_FILE;
            goto ERR1;
        }
    }

    nErrorCode = CG_EC_OK;
ERR1:
    return nErrorCode;
}

CG_ErrorCode CodeGen::GenerateEventHandlerImplCpp()
{
    CG_ErrorCode nErrorCode = CG_EC_FAILED;
    std::string sFileContent;
    bool bSuc;

    nErrorCode = CalcAllEventOperations();
    if (nErrorCode != CG_EC_OK) {
        goto ERR1;
    }

    sFileContent += TK_Tools::FormatStr("#include \"%s\"", CG_INC_HPP_FILE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"%s\"", CG_EVENT_HANDLER_HPP_FILE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"%s\"", CG_EVENT_HANDLER_IMPL_HPP_FILE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("using namespace %s;", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("%s::%s(size_t nIndex)", CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str(), CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    : %s(nIndex)", CG_EVENT_HANDLER_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("{") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("}") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("%s::~%s()", CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str(), CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("{") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("}") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("class %s_EventHandleFactory", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("{") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("public:") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    static %s *NewObject(size_t nIndex) {", CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("        return new %s(nIndex);", CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    }") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    static void DeleteObject(%s *pObject) {", CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("        delete pObject;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    }") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("};") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("%s &%s::GetInstance(size_t nIndex)", CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str(), CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("{") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    static SMF_AutoReleaseObjectPool<%s, %s_INSTANCE_COUNT, %s_EventHandleFactory> arop;", CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str(), m_sPrefix.c_str(), m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    return arop.At(nIndex);") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("}") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("SMF_ErrorCode %s_EventHandlerImpl::Reset()", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("{") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    SMF_ErrorCode nErrorCode = SMF_ERR_OK;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    if (nErrorCode == SMF_ERR_OK) {") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("        nErrorCode = %s_EventHandler::Reset();", m_sPrefix.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    }") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    return nErrorCode;") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("}") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    
    if (!TK_Tools::FileExists(CG_EVENT_HANDLER_IMPL_CPP_PATH)) {
        bSuc = TK_Tools::SaveToFile(CG_EVENT_HANDLER_IMPL_CPP_PATH, sFileContent);
        if (!bSuc) {
            nErrorCode = CG_EC_FAILED_TO_SAVE_FILE;
            goto ERR1;
        }
    }

    nErrorCode = CG_EC_OK;
ERR1:
    return nErrorCode;
}

CG_ErrorCode CodeGen::GenerateBaseStateHpp()
{
    CG_ErrorCode nErrorCode = CG_EC_FAILED;
    std::string sFileContent;
    bool bSuc;

    sFileContent += TK_Tools::FormatStr("#ifndef %s", CG_BASE_STATE_HPP_FILE_INC_ONCE_MACRO.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#define %s", CG_BASE_STATE_HPP_FILE_INC_ONCE_MACRO.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("#include \"SMF_afx.h\"") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"SMF_BaseEngine.h\"") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"SMF_BaseState.h\"") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"%s.hpp\"", CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("class %s;", CG_EVENT_HANDLER_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("class %s : public SMF_BaseState", CG_BASE_STATE_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("{") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("public:") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    %s(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex)", CG_BASE_STATE_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("        : SMF_BaseState(rEngine, sStateName, nIndex) {}") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    virtual ~%s() {}", CG_BASE_STATE_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    virtual %s &GetEventHandler();", CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    virtual %s &eh();", CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    virtual SMF_ErrorCode Reset() { return SMF_BaseState::Reset(); }") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("};") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("#endif // #define %s", CG_BASE_STATE_HPP_FILE_INC_ONCE_MACRO.c_str()) + NEW_LINE;
    
    bSuc = TK_Tools::SaveToFile(CG_BASE_STATE_HPP_PATH, sFileContent);
    if (!bSuc) {
        nErrorCode = CG_EC_FAILED_TO_SAVE_FILE;
        goto ERR1;
    }

    nErrorCode = CG_EC_OK;
ERR1:
    return nErrorCode;
}

CG_ErrorCode CodeGen::GenerateBaseStateCpp()
{
    CG_ErrorCode nErrorCode = CG_EC_FAILED;
    std::string sFileContent;
    bool bSuc;
    
    sFileContent += TK_Tools::FormatStr("#include \"%s\"", CG_BASE_STATE_HPP_FILE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"%s\"", CG_INC_HPP_FILE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#include \"%s\"", CG_EVENT_HANDLER_HPP_FILE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("%s &%s::eh()", CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str(), CG_BASE_STATE_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("{") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    return %s::GetInstance(GetIndex());", CG_EVENT_HANDLER_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("}") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("%s &%s::GetEventHandler()", CG_EVENT_HANDLER_IMPL_CLASS_NAME.c_str(), CG_BASE_STATE_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("{") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    return %s::GetInstance(GetIndex());", CG_EVENT_HANDLER_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("}") + NEW_LINE;
    
    bSuc = TK_Tools::SaveToFile(CG_BASE_STATE_CPP_PATH, sFileContent);
    if (!bSuc) {
        nErrorCode = CG_EC_FAILED_TO_SAVE_FILE;
        goto ERR1;
    }

    nErrorCode = CG_EC_OK;
ERR1:
    return nErrorCode;
}

CG_ErrorCode CodeGen::GenerateStateHpp(ShapeState &oShapeState, std::string &sOutput)
{
    CG_ErrorCode nErrorCode = CG_EC_FAILED;
    std::string &sFileContent(sOutput);
    unsigned int i, j;
    std::set<std::string> setOperations;

    sFileContent.clear();

    sFileContent += TK_Tools::FormatStr("////////////////////////////////////////////////////////////////////////////////////////////////////") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("// %s", CG_STATE_CLASS_NAME(oShapeState.sStateName).c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("//") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("class %s : public %s", CG_STATE_CLASS_NAME(oShapeState.sStateName).c_str(), CG_BASE_STATE_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("{") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("public:") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    %s(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : %s(rEngine, sStateName, nIndex) {}", CG_STATE_CLASS_NAME(oShapeState.sStateName).c_str(), CG_BASE_STATE_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    virtual ~%s() {}", CG_STATE_CLASS_NAME(oShapeState.sStateName).c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    SMF_DECLARE_STATE_TRANS_TABLE()") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("public:") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    //") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    // Event handlers") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    //") + NEW_LINE;

    for (i = 0; i < oShapeState.arrInternalTransItems.size(); i++) {
        TransItem &oTransItem = oShapeState.arrInternalTransItems[i];
        if (!IsValueInSet(oTransItem.sOperation, setOperations)) {
            setOperations.insert(oTransItem.sOperation);
            if (!oTransItem.sOperation.empty()) {
                sFileContent += TK_Tools::FormatStr("    SMF_ErrorCode %s(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().%s(rOpArg); }", oTransItem.sOperation.c_str(), oTransItem.sOperation.c_str()) + NEW_LINE;
            }
        }
    }
    for (i = 0; i < oShapeState.arrShapeTransFromMe.size(); i++) {
        std::vector<TransItem> &arrTransItems = oShapeState.arrShapeTransFromMe[i]->arrTransItems;
        for (j = 0; j < arrTransItems.size(); j++) {
            TransItem &oTransItem = arrTransItems[j];
            if (!IsValueInSet(oTransItem.sOperation, setOperations)) {
                setOperations.insert(oTransItem.sOperation);
                if (!oTransItem.sOperation.empty()) {
                    sFileContent += TK_Tools::FormatStr("    SMF_ErrorCode %s(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().%s(rOpArg); }", oTransItem.sOperation.c_str(), oTransItem.sOperation.c_str()) + NEW_LINE;
                }
            }
        }
    }

    sFileContent += TK_Tools::FormatStr("};") + NEW_LINE;

    nErrorCode = CG_EC_OK;
//ERR1:
    return nErrorCode;
}

CG_ErrorCode CodeGen::GenerateStateCpp(ShapeState &oShapeState, std::string &sOutput)
{
    CG_ErrorCode nErrorCode = CG_EC_FAILED;
    std::string &sFileContent(sOutput);
    unsigned int i, j;
    std::string sElseTransItemsTempLines;

    sFileContent.clear();

    sFileContent += TK_Tools::FormatStr("////////////////////////////////////////////////////////////////////////////////////////////////////") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("// %s", CG_STATE_CLASS_NAME(oShapeState.sStateName).c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("//") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(%s)", oShapeState.sStateName.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()") + NEW_LINE;
    
    for (i = 0; i < oShapeState.arrInternalTransItems.size(); i++) {
        TransItem &oTransItem = oShapeState.arrInternalTransItems[i];
        if (TK_Tools::CompareNoCase(oTransItem.sCondition, CONDITION_ELSE) != 0) {
            sFileContent += TK_Tools::FormatStr("    SMF_DEFINE_STATE_TRANS_ITEM(%s, %s, %s, %s)",
                oTransItem.sEvent.empty() ? "_GOTO_SELF" : oTransItem.sEvent.c_str(),
                oTransItem.sCondition.empty() ? "true" : (std::string("(") + oTransItem.sCondition + std::string(")")).c_str(),
                oTransItem.sOperation.empty() ? "null" : oTransItem.sOperation.c_str(),
                (TK_Tools::CompareNoCase(oTransItem.sEvent, "Entry") == 0 || TK_Tools::CompareNoCase(oTransItem.sEvent, "Exit") == 0) ? "null" : oShapeState.sStateName.c_str()
            ) + NEW_LINE;
        } else {
            sElseTransItemsTempLines += TK_Tools::FormatStr("    SMF_DEFINE_STATE_TRANS_ITEM(%s, %s, %s, %s)",
                oTransItem.sEvent.empty() ? "_GOTO_SELF" : oTransItem.sEvent.c_str(),
                "true" CONDITION_ELSE_CPP_COMMENT,
                oTransItem.sOperation.empty() ? "null" : oTransItem.sOperation.c_str(),
                (TK_Tools::CompareNoCase(oTransItem.sEvent, "Entry") == 0 || TK_Tools::CompareNoCase(oTransItem.sEvent, "Exit") == 0) ? "null" : oShapeState.sStateName.c_str()
            ) + NEW_LINE;
        }
    }
    for (i = 0; i < oShapeState.arrShapeTransFromMe.size(); i++) {
        ShapeState *pEndShapeState = oShapeState.arrShapeTransFromMe[i]->pEndShapeState;
        std::vector<TransItem> &arrTransItems = oShapeState.arrShapeTransFromMe[i]->arrTransItems;
        if (arrTransItems.size() > 0) {
            for (j = 0; j < arrTransItems.size(); j++) {
                TransItem &oTransItem = arrTransItems[j];
                if (TK_Tools::CompareNoCase(oTransItem.sCondition, CONDITION_ELSE) != 0) {
                    sFileContent += TK_Tools::FormatStr("    SMF_DEFINE_STATE_TRANS_ITEM(%s, %s, %s, %s)",
                        oTransItem.sEvent.empty() ? (
                            pEndShapeState->sStateName == oShapeState.sStateName ? "_GOTO_SELF" : 
                                (pEndShapeState->pParentShapeState != NULL && pEndShapeState->pParentShapeState->sStateName == oShapeState.sStateName ? "_GOTO_DEF_CHILD" : "_GOTO_PARENT")
                        ) : oTransItem.sEvent.c_str(),
                        oTransItem.sCondition.empty() ? "true" : (std::string("(") + oTransItem.sCondition + std::string(")")).c_str(),
                        oTransItem.sOperation.empty() ? "null" : oTransItem.sOperation.c_str(),
                        oShapeState.arrShapeTransFromMe[i]->pEndShapeState->sStateName.c_str()
                    ) + NEW_LINE;
                } else {
                    sElseTransItemsTempLines += TK_Tools::FormatStr("    SMF_DEFINE_STATE_TRANS_ITEM(%s, %s, %s, %s)",
                        oTransItem.sEvent.empty() ? (
                            pEndShapeState->sStateName == oShapeState.sStateName ? "_GOTO_SELF" : 
                                (pEndShapeState->pParentShapeState != NULL && pEndShapeState->pParentShapeState->sStateName == oShapeState.sStateName ? "_GOTO_DEF_CHILD" : "_GOTO_PARENT")
                        ) : oTransItem.sEvent.c_str(),
                        "true" CONDITION_ELSE_CPP_COMMENT,
                        oTransItem.sOperation.empty() ? "null" : oTransItem.sOperation.c_str(),
                        oShapeState.arrShapeTransFromMe[i]->pEndShapeState->sStateName.c_str()
                    ) + NEW_LINE;
                }
            }
        } else {
            if (pEndShapeState->pParentShapeState->sStateName == oShapeState.sStateName) {
                sFileContent += TK_Tools::FormatStr("    SMF_DEFINE_STATE_TRANS_ITEM(%s, %s, %s, %s)",
                    pEndShapeState->sStateName == oShapeState.sStateName ? "_GOTO_SELF" : 
                        (pEndShapeState->pParentShapeState != NULL && pEndShapeState->pParentShapeState->sStateName == oShapeState.sStateName ? "_GOTO_DEF_CHILD" : "_GOTO_PARENT")
                    ,
                    "true",
                    "null",
                    pEndShapeState->sStateName.c_str()
                ) + NEW_LINE;
            } else {
                nErrorCode = CG_EC_INVALID_DEF_TRANSITION;
                goto ERR1;
            }
        }
    }

    sFileContent += sElseTransItemsTempLines;

    sFileContent += TK_Tools::FormatStr("SMF_DEFINE_STATE_TRANS_TABLE_END()") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#undef SMF_CURRENT_STATE_CLASS") + NEW_LINE;

    nErrorCode = CG_EC_OK;
ERR1:
    return nErrorCode;
}

CG_ErrorCode CodeGen::GenerateInternalRootStateHpp(std::string &sOutput)
{
    CG_ErrorCode nErrorCode = CG_EC_FAILED;
    std::string &sFileContent(sOutput);

    sFileContent.clear();

    sFileContent += TK_Tools::FormatStr("////////////////////////////////////////////////////////////////////////////////////////////////////") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("// %s", CG_STATE_CLASS_NAME(CG_INTERNAL_ROOT_STATE_NAME).c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("//") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("class %s : public %s", CG_STATE_CLASS_NAME(CG_INTERNAL_ROOT_STATE_NAME).c_str(), CG_BASE_STATE_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("{") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("public:") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    %s(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : %s(rEngine, sStateName, nIndex) {}", CG_STATE_CLASS_NAME(CG_INTERNAL_ROOT_STATE_NAME).c_str(), CG_BASE_STATE_CLASS_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    virtual ~%s() {}", CG_STATE_CLASS_NAME(CG_INTERNAL_ROOT_STATE_NAME).c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    SMF_DECLARE_STATE_TRANS_TABLE()") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("public:") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    //") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    // Event handlers") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("    //") + NEW_LINE;
    
    sFileContent += TK_Tools::FormatStr("};") + NEW_LINE;

    nErrorCode = CG_EC_OK;
//ERR1:
    return nErrorCode;
}

CG_ErrorCode CodeGen::GenerateInternalRootStateCpp(std::string &sOutput)
{
    CG_ErrorCode nErrorCode = CG_EC_FAILED;
    std::string &sFileContent(sOutput);

    sFileContent.clear();

    sFileContent += TK_Tools::FormatStr("////////////////////////////////////////////////////////////////////////////////////////////////////") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("// %s", CG_STATE_CLASS_NAME(CG_INTERNAL_ROOT_STATE_NAME).c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("//") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(%s)", CG_INTERNAL_ROOT_STATE_NAME.c_str()) + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()") + NEW_LINE;
    
    sFileContent += TK_Tools::FormatStr("    SMF_DEFINE_STATE_TRANS_ITEM(_GOTO_FIRST_CHILD, true, null, null)") + NEW_LINE;

    sFileContent += TK_Tools::FormatStr("SMF_DEFINE_STATE_TRANS_TABLE_END()") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("") + NEW_LINE;
    sFileContent += TK_Tools::FormatStr("#undef SMF_CURRENT_STATE_CLASS") + NEW_LINE;

    nErrorCode = CG_EC_OK;
//ERR1:
    return nErrorCode;
}

CG_ErrorCode CodeGen::CalcAllEventNames()
{
    CG_ErrorCode nErrorCode = CG_EC_FAILED;
    unsigned int i, j, k;

    m_arrAllEventNames.clear();

    for (i = 0; i < m_arrShapeStates.size(); i++) {
        ShapeState &oShapeState = *m_arrShapeStates[i];
        for (j = 0; j < oShapeState.arrInternalTransItems.size(); j++) {
            TransItem &TransItem = oShapeState.arrInternalTransItems[j];
            m_arrAllEventNames.insert(TransItem.sEvent);
        }
        for (j = 0; j < oShapeState.arrShapeTransFromMe.size(); j++) {
            if (oShapeState.arrShapeTransFromMe[j] != NULL) {
                ShapeTrans &oShapeTrans = *oShapeState.arrShapeTransFromMe[j];
                for (k = 0; k < oShapeTrans.arrTransItems.size(); k++) {
                    TransItem &TransItem = oShapeTrans.arrTransItems[k];
                    m_arrAllEventNames.insert(TransItem.sEvent);
                }
            }
        }
    }

    nErrorCode = CG_EC_OK;
//ERR1:
    return nErrorCode;
}

CG_ErrorCode CodeGen::CalcAllEventOperations()
{
    CG_ErrorCode nErrorCode = CG_EC_FAILED;
    unsigned int i, j, k;

    m_arrAllEventOperations.clear();

    for (i = 0; i < m_arrShapeStates.size(); i++) {
        ShapeState &oShapeState = *m_arrShapeStates[i];
        for (j = 0; j < oShapeState.arrInternalTransItems.size(); j++) {
            TransItem &TransItem = oShapeState.arrInternalTransItems[j];
            m_arrAllEventOperations.insert(TransItem.sOperation);
        }
        for (j = 0; j < oShapeState.arrShapeTransFromMe.size(); j++) {
            if (oShapeState.arrShapeTransFromMe[j] != NULL) {
                ShapeTrans &oShapeTrans = *oShapeState.arrShapeTransFromMe[j];
                for (k = 0; k < oShapeTrans.arrTransItems.size(); k++) {
                    TransItem &TransItem = oShapeTrans.arrTransItems[k];
                    m_arrAllEventOperations.insert(TransItem.sOperation);
                }
            }
        }
    }

    nErrorCode = CG_EC_OK;
//ERR1:
    return nErrorCode;
}

bool CodeGen::IsValueInSet(const std::string &sValue, const std::set<std::string> &oSet)
{
    std::set<std::string>::const_iterator it = oSet.find(sValue);
    if (it != oSet.end()) {
        return true;
    } else {
        return false;
    }
}

std::string CodeGen::GetErrorStr(CG_ErrorCode nErrorCode) const
{
    std::string sErrorStr;
    unsigned int nErrStrTableSize = TK_ARR_LEN(CG_ErrorStringMap);
    unsigned int i;

    for (i = 0; i < nErrStrTableSize; i++) {
        if (CG_ErrorStringMap[i].nErrorCode == nErrorCode) {
            sErrorStr = CG_ErrorStringMap[i].pErrorStr;
            break;
        }
    }
    if (i >= nErrStrTableSize) {
        sErrorStr = TK_Tools::FormatStr("%u", nErrorCode);
    }

    return sErrorStr;
}
