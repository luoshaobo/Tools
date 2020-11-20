////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file dBUSManager.cc
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   12-Sep-2018
////////////////////////////////////////////////////////////////////////////

#include <rms/service/dbus/dBusManager.h>
#include <rms/config/settings.h>
#include <rms/service/data/dataProcessor.h>
#include <rms/service/dbus/routeDBusMethods.h>
#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

///////////////////////////////////////////////////////////////////////////////
//! \fn DBusManager::DBusManager(const Mailbox& mbox)
//! \brief constructor
//! \param[in] mbox Thread mailbox
//! \return void
////////////////////////////////////////////////////////////////////////////////
DBusManager::DBusManager(const Mailbox& mbox) :
    Service(mbox)
//,   glibThread(f_dbus_stuff)
{

}
///////////////////////////////////////////////////////////////////////////////
//! \fn DBusManager::~DBusManager()
//! \brief destructor
//! \return void
////////////////////////////////////////////////////////////////////////////////
DBusManager::~DBusManager()
{
}

///////////////////////////////////////////////////////////////////////////////
//! \fn DBusManager::handleReadRes(const boost::shared_ptr<Notify>& notify)
//! \brief send Cable data to d-bus
//! \param[in] notify Incoming message eith cable data
//! \return void
///////////////////////////////////////////////////////////////////////////////
void DBusManager::handleReadRes(const boost::shared_ptr<Notify>& notify)
{
    //sendCableData(reinterpret_cast<const char*>(notify->getPayloadRef().data()),
    //        notify->getPayloadRef().size());
}

///////////////////////////////////////////////////////////////////////////////
//! \fn DBusManager::handle(const boost::shared_ptr<Notify>& notify)
//! \brief Process incoming messages to Dbus manager mail box
//! \param[in] notify Incoming message
//! \return void
///////////////////////////////////////////////////////////////////////////////
void DBusManager::handle(const boost::shared_ptr<Notify>& notify)
{
    typedef EventHandler<
            NotifyId,
            DBusManager,
            void (DBusManager::*)(const boost::shared_ptr<Notify>&)
    > hr;
    static hr amh[] = {
        hr(NotifyId::MSG_AM_READ_RES,  *this, &DBusManager::handleReadRes),
    };

    for (uint32_t i = 0; i < sizeof(amh)/ sizeof(amh[0]); i++ ) {
        amh[i].handle(notify->getNotifyId(), notify);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn DBusManager::sendMessageWrapper(const Mailbox& mbox, const boost::shared_ptr<Message>& msg)
//! \brief Wrapper over sendMessage
//! \param[in] mbox recipient Mailbox
//! \param[in] msg Message to be sent
//!  \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DBusManager::sendMessageWrapper(const Mailbox& mbox, const boost::shared_ptr<Message>& msg) const
{
    sendMessage(mbox, msg);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn DBusManager::sendEventWrapper(const boost::shared_ptr<Event>& msg) const
//! \brief Wrapper over sendEvent
//! \param[in] msg Message to be sent
//!  \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DBusManager::sendEventWrapper(const boost::shared_ptr<Event>& msg) const
{
    sendEvent(msg);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn DBusManager* DBusManager::getInstance()
//! \brief get instance of DBusManager class
//! \return pointer to DBusManager object
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DBusManager* DBusManager::getInstance()
{
    static DBusManager dbusManager(Mailbox::SERVICE_DBUS_MANAGER);
    return &dbusManager;
}
