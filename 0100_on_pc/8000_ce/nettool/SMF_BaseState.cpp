#include "SMF_BaseState.h"
#include "SMF_BaseEngine.h"
#include "SMF_BaseEventHandler.h"

SMF_DEFINE_STATE(null, null, 1)

SMF_BaseState::SMF_BaseState(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex)
    : SMF_NonCopyable()
    , m_nIndex(nIndex)
    , m_rEngine(rEngine)
    , m_sStateName(sStateName)
    , m_pParentState(NULL)
    , m_arrChildStates()
{

}

SMF_BaseState::~SMF_BaseState()
{

}

SMF_ErrorCode SMF_BaseState::Enter(SMF_SenderId nSenderId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    nErrorCode = ProcessEvent(SMF_D_EVENT_ID(_ENTRY), NULL, nSenderId, NULL);
    if (nErrorCode == SMF_ERR_NOT_PROCESSED) {
        SMF_SM_LOG();
        nErrorCode = SMF_ERR_OK;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseState::Exit(SMF_SenderId nSenderId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    nErrorCode = ProcessEvent(SMF_D_EVENT_ID(_EXIT), NULL, nSenderId, NULL);
    if (nErrorCode == SMF_ERR_NOT_PROCESSED) {
        SMF_SM_LOG();
        nErrorCode = SMF_ERR_OK;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseState::ProcessEvent(SMF_EventId nEventId, void *pData, SMF_SenderId nSenderId, SMF_BaseState **pDestState)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_TransItem oTransItem;

    do {
        nErrorCode = GetTransItemByEventId(nEventId, oTransItem);
        if (nErrorCode != SMF_ERR_OK) {
            break;
        }

        if (oTransItem.pDestState == NULL) {
            nErrorCode = SMF_ERR_NULL_DEST_STATE;
            break;
        }

        if (pDestState != NULL) {
            *pDestState = oTransItem.pDestState;
        }

        if (oTransItem.pOperation == NULL) {
            nErrorCode = SMF_ERR_NULL_OPERATION_PTR;
            break;
        }

        SMF_OpArg oOpArg(*this, *oTransItem.pDestState, nEventId, pData, nSenderId);
        nErrorCode = (this->*oTransItem.pOperation)(oOpArg);
    } while (0);

    return nErrorCode;
}

SMF_BaseState *SMF_BaseState::GetNextState(SMF_EventId nEventId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_BaseState *pNextState = NULL;
    SMF_TransItem oTransItem;

    nErrorCode = GetTransItemByEventId(nEventId, oTransItem);
    if (nErrorCode != SMF_ERR_OK) {
        pNextState = NULL;
    } else {
        pNextState = oTransItem.pDestState;
    }

    return pNextState;
}

SMF_BaseState *SMF_BaseState::GetDefChildState()
{
    SMF_BaseState *pDestState;

    pDestState = GetNextState(SMF_D_EVENT_ID(_GOTO_DEF_CHILD));

    return pDestState;
}

SMF_ErrorCode SMF_BaseState::AddChildState(SMF_BaseState *pChildState)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    unsigned int i;

    do {
        if (pChildState == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
            break;
        }

        for (i = 0; i < m_arrChildStates.size(); i++) {
            if (pChildState == m_arrChildStates[i]) {
                break;
            }
        }
        if (i >= m_arrChildStates.size()) {
            m_arrChildStates.push_back(pChildState);
            pChildState->m_pParentState = this;
        }
    } while (0);

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseState::SetParentState(SMF_BaseState *pParentState)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    do {
        if (pParentState == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
            break;
        }

        m_pParentState = pParentState;
        nErrorCode = m_pParentState->AddChildState(this);
    } while (0);

    return nErrorCode;
}

SMF_StateRelationship SMF_BaseState::RelationshipTo(SMF_BaseState *pDstState) const
{
    SMF_StateRelationship srs = SMF_SR_OHTER;
    unsigned int i;

    do {
        if (pDstState == NULL) {
            srs = SMF_SR_INVALID;
            break;
        }

        if (pDstState == this) {
            srs = SMF_SR_SELF;
            break;
        }

        if (pDstState == m_pParentState) {
            srs = SMF_SR_CHILD;
            break;
        }

        for (i = 0; i < m_arrChildStates.size(); i++) {
            if (pDstState == m_arrChildStates[i]) {
                srs = SMF_SR_PARENT;
                break;
            }
        }
        if (srs != SMF_SR_OHTER) {
            break;
        }

        if (m_pParentState != NULL) {
            for (i = 0; i < m_pParentState->m_arrChildStates.size(); i++) {
                if (pDstState == m_pParentState->m_arrChildStates[i]) {
                    srs = SMF_SR_BROTHER;
                    break;
                }
            }
        }
        if (srs != SMF_SR_OHTER) {
            break;
        }
    } while (0);

    return srs;
}

SMF_BaseState *SMF_BaseState::GetCurrentState() const
{
    return m_rEngine.GetCurrentState();
}

bool SMF_BaseState::IsEqualTo(const SMF_BaseState &rState) const
{
    bool bResult;

    if (this == &rState) {
        bResult = true;
    } else {
        bResult = false;
    }

    return bResult;
}
