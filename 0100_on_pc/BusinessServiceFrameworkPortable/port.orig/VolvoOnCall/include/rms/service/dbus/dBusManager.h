////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file dBusManager.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   21-Sep-2018
////////////////////////////////////////////////////////////////////////////

#ifndef PRJ_SOC_FORD_PACKAGE_XEVCDM_INCLUDE_SERVICE_DBUS_DBUSMANAGER_H_
#define PRJ_SOC_FORD_PACKAGE_XEVCDM_INCLUDE_SERVICE_DBUS_DBUSMANAGER_H_

#include <rms/sys/service.h>

//! \class DBusManager
//! \brief Perform routing D-bus message from d-bus server glib loop to xevcdm mainloop.
///  DBusManager is implemented as singleton
class DBusManager:public Service
{
private:
    DBusManager();
    DBusManager(const Mailbox& mbox);
public:
    virtual ~DBusManager();

    static DBusManager* getInstance();

    virtual void handle(const boost::shared_ptr<Notify>& notify);

    void sendMessageWrapper(const Mailbox& mbox,
                const boost::shared_ptr<Message>& msg) const;

    void sendEventWrapper(const boost::shared_ptr<Event>& msg) const;

private:

    void handleReadRes(const boost::shared_ptr<Notify>& notify);

private:
    //! \var glibThread
    //! \brief separate thread for glib d-bus
    boost::thread glibThread;
};



#endif /* PRJ_SOC_FORD_PACKAGE_XEVCDM_INCLUDE_SERVICE_DBUS_DBUSMANAGER_H_ */
