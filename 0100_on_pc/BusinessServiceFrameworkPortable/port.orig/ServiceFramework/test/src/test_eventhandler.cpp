/**
* @file   eventhandlertest.cpp
*
* @brief
*          Header file
* @author  (last changes):
*          - Elmar Weber
*          - elmar.weber@continental-corporation.com
*          - Continental AG
* @par Project:
* @par SW-Package:
*
* @par SW-Module:
*
* @note
*
* @par Module-History:
*  Date        Author                   Reason
*
* @par Copyright Notice:
* Copyright (C) Continental AG 2017
* Alle Rechte vorbehalten. All Rights Reserved.
* The reproduction, transmission or use of this document or its contents is
* not permitted without express written authority.
* Offenders will be liable for damages. All rights, including rights created
* by patent grant or registration of a utility model or design, are reserved.
*/


#include "tut/tinyunittest.hpp"
#include "eventhandler.h"


TUT_TESTSUITE(EventHandlerTest);

class StringEvent : public Event
{
public:
  StringEvent() {}
  //Event implementation
  const char* getName() const { return "StringEvent"; }
  Event* clone() const { return new StringEvent(*this); }

  std::string m_sValue;
};


class EventHandlerClient01
{
public:
  EventHandlerClient01() : m_EventHandler("EventHandlerClient01")
  {
    //register the eventhandler for the MqttRequestEvent which stores an external Mqtt-request
    m_EventHandler.registerEventHandler(this, &EventHandlerClient01::handleStringEvent);
  }

  void handleStringEvent(const StringEvent * ev)
  {
    m_sValue = ev->m_sValue;
  }

  EventHandler m_EventHandler;
  std::string m_sValue;
};


TUT_TESTCASE(EventHandlerTest_01)
{
  EventHandlerClient01 eventHandler;

  //check if the name was stored which is assigned to the handler
  TUT_CHECK(eventHandler.m_EventHandler.getName() == "EventHandlerClient01" );

  //Check if the eventHandler has registered this method for eventhandling
  TUT_CHECK(eventHandler.m_EventHandler.isRegistered(&eventHandler, &EventHandlerClient01::handleStringEvent) == true);

  //check that value is empty now
  TUT_CHECK(eventHandler.m_sValue.empty() == true);

  //send event
  StringEvent *event = new StringEvent;
  event->m_sValue = "TestString01";
  eventHandler.m_EventHandler.handleEvent(event);
  TUT_CHECK(eventHandler.m_sValue == "TestString01");

  //send another event
  StringEvent *event2 = new StringEvent;
  event2->m_sValue = "TestString02";
  eventHandler.m_EventHandler.handleEvent(event2);
  TUT_CHECK(eventHandler.m_sValue == "TestString02");

}
