////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file networkManager.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   21-Sep-2018
////////////////////////////////////////////////////////////////////////////

#pragma once
#include <rms/sys/service.h>
#include <rms/service/network/networkFSM.h>

//! \class NetworkManager
//! \brief Service which process network connection and sending data
class NetworkManager:
        public Service,
        public nfsm::NetworkCallbackInterface {

    NetworkManager();
    NetworkManager(const Mailbox& mbox);
public:
    virtual ~NetworkManager();
    static NetworkManager* getInstance();
private:
    //--------------------------------------------------------------------------
    // nfsm::NetworkCallbackInterface
    //--------------------------------------------------------------------------
    virtual void onConnected() const;
    virtual void onDisconnected() const;
    virtual void onConnectionLost(const boost::shared_ptr<Notify>& notify) const;
    virtual void onSendCompleted(const std::vector<uint8_t>& resp) const;
    virtual void handle(const boost::shared_ptr<Notify>& notify);

    //--------------------------------------------------------------------------
    // Asynchronous Message/Event handlers
    //--------------------------------------------------------------------------
    void handleConnect(const boost::shared_ptr<Notify>& notify);

    void handleDisconnect(const boost::shared_ptr<Notify>& notify);

    void handleSend(const boost::shared_ptr<Notify>& notify);

    void handleSendTm(const boost::shared_ptr<Notify>& notify);

    //--------------------------------------------------------------------------
    // Service
    //--------------------------------------------------------------------------
    virtual void onStart();
    virtual void onStop();

private:
    const std::string mUrl;
    nfsm::NetworkFSM mFsm;
};


