#ifndef SMF_BASE_EVENT_HANDLER__78378r230402393285723423782352397885454858943954
#define SMF_BASE_EVENT_HANDLER__78378r230402393285723423782352397885454858943954

#include "SMF_afx.h"

class SMF_BaseEventHandler
{
public:
    SMF_BaseEventHandler(size_t nIndex);
    virtual ~SMF_BaseEventHandler();

public:
    size_t GetIndex() const { return m_nIndex; }

public:
    virtual SMF_ErrorCode Reset() { return SMF_ERR_OK; }

private:
    size_t m_nIndex;
};

#endif // #ifndef SMF_BASE_EVENT_HANDLER__78378r230402393285723423782352397885454858943954
