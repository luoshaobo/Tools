#ifndef SMF_BASE_STATE_H__28938923958923588238492835783478547878346
#define SMF_BASE_STATE_H__28938923958923588238492835783478547878346

#include "SMF_afx.h"

class SMF_BaseEngine;

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_BaseState
//
class SMF_BaseState : private SMF_NonCopyable
{
protected:
    SMF_BaseState(SMF_BaseEngine &rEngine, const std::string &sStateName);

public:
    virtual ~SMF_BaseState();
    
    //
    // interfaces
    // NOTE: 
    // 1) These functions should be defined by macros in the derived class.
    //
    virtual SMF_ErrorCode GetTransItemByEventId(SMF_EventId nEventId, SMF_TransItem &oTransItem) = 0;

    //
    // operations
    //
    SMF_ErrorCode Enter(SMF_SenderId nSenderId);
    SMF_ErrorCode Exit(SMF_SenderId nSenderId);
    SMF_ErrorCode ProcessEvent(SMF_EventId nEventId, void *pData, SMF_SenderId nSenderId, SMF_BaseState **pDestState);

    SMF_BaseState *GetNextState(SMF_EventId nEventId);
    SMF_BaseState *GetDefChildState();
    SMF_BaseState *GetCurrentState() const;

    bool IsEqualTo(const SMF_BaseState &rState) const;
    
    //
    // setter
    //
    SMF_ErrorCode SetParentState(SMF_BaseState *pParentState);
    SMF_ErrorCode AddChildState(SMF_BaseState *pChildState);

    //
    // getter
    //
    SMF_BaseEngine &GetEngine() const { return m_rEngine; }
    const std::string &GetStateName() const { return m_sStateName; }
    SMF_BaseState *GetParentState() const { return m_pParentState; }
    std::vector<SMF_BaseState *> &GetChildStates() { return m_arrChildStates; }

    //
    // helper
    //
    SMF_StateRelationship RelationshipTo(SMF_BaseState *pDstState) const;
    SMF_ErrorCode null(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }

    //
    // data members
    //
private:
    SMF_BaseEngine &m_rEngine;
    std::string m_sStateName;
    SMF_BaseState *m_pParentState;
    std::vector<SMF_BaseState *> m_arrChildStates;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_null
//
class State_null : public SMF_BaseState
{
public:
    State_null(SMF_BaseEngine &rEngine, const std::string &sStateName) : SMF_BaseState(rEngine, sStateName) {}
    virtual ~State_null() {}
    
    virtual SMF_ErrorCode GetTransItemByEventId(SMF_EventId nEventId, SMF_TransItem &oTransItem) { return SMF_ERR_NOT_PROCESSED; }
};

SMF_DECLARE_STATE(null)

#endif // #ifndef SMF_BASE_STATE_H__28938923958923588238492835783478547878346
