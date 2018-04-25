#ifndef USB_EVENTHANDLERIMPL_HPP
#define USB_EVENTHANDLERIMPL_HPP

#include "USB_EventHandler.hpp"

struct SMF_OpArg;

class USB_EventHandlerImpl : public USB_EventHandler
{
public:
    USB_EventHandlerImpl(size_t nIndex);

public:
    virtual ~USB_EventHandlerImpl();
    static USB_EventHandlerImpl &GetInstance(size_t nIndex);

public:
    virtual SMF_ErrorCode Reset();

public:
    //
    // implementation interfaces
    //

public:
    //
    // conditions
    //

private:
    //
    // implementations
    //

private:
    //
    // data
    //

};

#endif // #define USB_EVENTHANDLERIMPL_HPP
