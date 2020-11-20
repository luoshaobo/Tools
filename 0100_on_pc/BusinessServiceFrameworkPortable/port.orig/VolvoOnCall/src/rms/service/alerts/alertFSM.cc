////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file alertFSM.cc
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   12-Sep-2018
////////////////////////////////////////////////////////////////////////////

#include <string>

#include "dlt/dlt.h"
#include <rms/service/alerts/alertFSM.h>
#include <rms/gbt/gbt32960_3_simple.h>
#include <rms/util/utils.h>

using namespace std;

DLT_IMPORT_CONTEXT(dlt_voc);

namespace afsm {

//! \var loginSnFile

//! \brief definition of static variable with name of file where last login serial number is stored
string AlertFSM::loginSnFile = "login_sn";


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::AlertFSM(AlertCallbackInterface& parent)
//! \brief AlertFSM constructor
//! \param[in] parent parent whose callbacks will be called
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AlertFSM::AlertFSM(AlertCallbackInterface& parent) :
        mParent(parent) {
    mLoginData.time = Settings::getInstance()->getChinaTime();
    mLoginData.sn = VLL_LOGIN_SN_MIN;
    mLastSentCmd = CU_INVALID;
    boost::filesystem::path file(
            Settings::getInstance()->getWorkdir() + "/" + loginSnFile);
    do {
        if (!boost::filesystem::exists(file)) {
            break;
        }

        if (boost::filesystem::file_size(file) != sizeof(LoginData)) {
            boost::filesystem::remove(file);
            break;
        }

        boost::filesystem::ifstream ifs(file);
        ifs.read(reinterpret_cast<char*>(&mLoginData), sizeof(LoginData));
    } while(false);

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Last login SN %d at %02d/%02d/%02d %02d:%02d:%02d", mLoginData.sn,
            mLoginData.time.month, mLoginData.time.day, mLoginData.time.year,
            mLoginData.time.hour, mLoginData.time.minute, mLoginData.time.second);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::connect(const aev::EventLogin& ev)
//! \brief implementation of connect action
//! \param[in] ev triggering login event
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertFSM::connect(const aev::EventLogin& ev) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    mParent.connect(Settings::getInstance()->getServerUrl());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::disconnect(const aev::EventDisconnect& ev)
//! \brief implementation of disconnect action
//! \param[in] ev triggering disconnect event
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertFSM::disconnect(const aev::EventDisconnect& ev) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    mParent.disconnect();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::reconnect(const iev::EventReconnect& ev)
//! \brief implementation of reconnect action
//! \param[in] ev triggering reconnect event
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertFSM::reconnect(const iev::EventReconnect& ev) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    mParent.disconnect();
    mParent.onConnectFailure();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::login(const aev::EventLogin& ev)
//! \brief implementation of login action
//! \param[in] ev triggering login event
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertFSM::login(const aev::EventLogin& ev) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    mLastSentCmd = CU_VEHICLE_LOGIN;
    doLogin();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::logout(const aev::EventLogout& ev)
//! \brief implementation of logout action
//! \param[in] ev triggering logout event
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertFSM::logout(const aev::EventLogout& ev) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    mLastSentCmd = CU_VEHICLE_LOGOUT;
    doLogout();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::connected(const pev::EventConnected& ev)
//! \brief implementation of work to be performed on connection
//! \param[in] ev triggering connected event
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertFSM::connected(const pev::EventConnected& ev) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    mParent.onConnectSuccess();
    doLogin();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::disconnected(const pev::EventDisconnected& ev)
//! \brief implementation of work to be performed on unexpected disconnection
//! \param[in] ev triggering disconnected event
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertFSM::disconnected(const pev::EventDisconnected& ev) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    mParent.onConnectFailure();
//    login();//delete
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::connectionLost(const pev::EventConnectionLost& ev)
//! \brief implementation of work to be performed on unexpected loss of connection
//! \param[in] ev triggering loss of connection event
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertFSM::connectionLost(const pev::EventConnectionLost& ev) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    mParent.onConnectFailure();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::validatePublishResponse(const pev::EventSendCompleted& ev)
//! \brief implementation of processing and validation of publishing response
//! \param[in] ev triggering sending completed event
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertFSM::validatePublishResponse(const pev::EventSendCompleted& ev) {
    CommandId id = getResponseCmdType(ev.notify->getPayloadRef());
    ResponseMark rm = getResponseMarkType(ev.notify->getPayloadRef());
    bool valid = validateResponseIntegrity(ev.notify->getPayloadRef());
    bool correct = false;

    switch (mLastSentCmd) {
    case CU_HEARTBEAT:
        {
            bool isAlive = true;
            if (!valid || rm != RM_SUCCESS) {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: Invalid response on Heartbeat");
                isAlive = mParent.onHeartbeatNAck();
            } else if (mLastSentCmd == id) {
                mParent.onHeartbeatAck();
                correct = true;
            }

            if (!isAlive) {
                post_event(iev::EventReconnect());
            }
        }
        break;
    case CU_INVALID:    // occurs when response is empty
    case CU_REALTIME_REPORT:
    case CU_SUPPLEMENTARY_REPORT:
        if ((!ev.notify->getPayloadRef().size()) ||
                ((rm == RM_SUCCESS) && valid)) {
            mParent.onPublishSuccess();
            correct = true;
        } else {
//            mParent.onPublishFailure(); // delete
            // if receive a publish error ack, retry 1 min 3 times and then abandon message
            mParent.onPublishFailureAck();
        }
        break;
    default:
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Unknown CommandId = %d in response",id);
        mParent.onPublishFailure();
        break;
    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: Correct = %d: (Integrity = %d; CommandId = %d; ResponseMark = %d)",
            correct, valid, id, rm);

    if (correct) {
        post_event(iev::EventResponseSuccess());
    } else {
        post_event(iev::EventResponseFailure());
    }
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Publish response %s correct", correct ? "is" : "isn't");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::validateLoginResponse(const pev::EventSendCompleted& ev)
//! \brief processing and validation of login response
//! \param[in] ev triggering sending completed event
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertFSM::validateLoginResponse(const pev::EventSendCompleted& ev) {
    bool correct = false;
    bool valid = validateResponseIntegrity(ev.notify->getPayloadRef());
    ResponseMark rm = getResponseMarkType(ev.notify->getPayloadRef());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: Integrity = %d; ResponseMark = %d", valid, rm);

    if (!valid) {
        correct = false;
    } else if (rm == RM_SUCCESS) {
        mLoginData.sn++;
        correct = true;
    } else if (rm == RM_VIN_REPETITION) {
        correct = true;
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: Already logged in");
    }
    if (correct) {
        mParent.onLoginSuccess();
        post_event(iev::EventResponseSuccess());
    } else {
        mParent.onLoginFailure();
        post_event(iev::EventResponseFailure());
    }

    if (mLoginData.sn >= VLL_LOGIN_SN_MAX) {
        mLoginData.sn = VLL_LOGIN_SN_MIN;
    }

    boost::filesystem::path file(
            Settings::getInstance()->getWorkdir() + "/" + loginSnFile);
    boost::filesystem::ofstream ofs(file);
    ofs.write(reinterpret_cast<char*>(&mLoginData), sizeof(LoginData));

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Last login SN %d at %02d/%02d/%02d %02d:%02d:%02d",
            mLoginData.sn,
            mLoginData.time.month, mLoginData.time.day, mLoginData.time.year,
            mLoginData.time.hour, mLoginData.time.minute, mLoginData.time.second);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Login response %s correct", correct ? "is" : "isn't");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::validateLogoutResponse(const pev::EventSendCompleted& ev)
//! \brief processing and validation of logout response
//! \param[in] ev triggering sending completed event
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertFSM::validateLogoutResponse(const pev::EventSendCompleted& ev) {
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: Validate Logout Response");
    bool valid = validateResponseIntegrity(ev.notify->getPayloadRef());
    ResponseMark rm = getResponseMarkType(ev.notify->getPayloadRef());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: Integrity = %d; ResponseMark = %d", valid, rm);
    if (!valid || rm != RM_SUCCESS) {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: Ignore unsuccessful logout response");
    }
    mParent.onLogoutFinish();
    mParent.disconnect();
    post_event(iev::EventResponseSuccess());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::validateResponseIntegrity(const Payload& resp)
//! \brief validation of response integrity
//! \param[in] resp response payload
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AlertFSM::validateResponseIntegrity(const Payload& resp) {
    bool rc = false;
    do {
        uint32_t size = sizeof(DataPacketPart1);

        if (resp.size() < size) {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMS: Truncated packet. First check Sizes: (%d < %d)", resp.size(),
                    size);
            break;
        }

        const DataPacketPart1* p1 = NULL;
        if (!(p1 = reinterpret_cast<const DataPacketPart1*>(&resp[0]))) {
            break;
        }
        size += (uint32_t)ntohs(p1->unitLength);
        if (resp.size() < size) {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMS: Truncated packet. Second check Sizes: (%d < %d)", resp.size(),
                    size);
            break;
        }

        const DataPacketPart2* p2 = NULL;
        if (!(p2 = reinterpret_cast<const DataPacketPart2*>(&resp[size]))) {
            break;
        }
        size += sizeof(DataPacketPart2);
        if (resp.size() != size) {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMS: Bad packet size (%d!=%d). Third check", resp.size(), size);
            break;
        }

        if (memcmp(p1->start, GBT_START_CHAR_DATA, GBT_START_CHAR_SIZE)) {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "RMS: Bad start character");
            break;
        }

        uint8_t crc = ::calcCrc(reinterpret_cast<const uint8_t*>(&p1->unit),
                size - sizeof(DataPacketPart2) - GBT_START_CHAR_SIZE);

        if (crc != p2->crc) {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "RMS: Bad CRC");
            break;
        }

        rc = true;
    } while(false);

    return rc;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::getResponseCmdType(const Payload& resp) {
//! \brief getting response command type
//! \param[in] resp response payload
//! \return CommandId
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CommandId AlertFSM::getResponseCmdType(const Payload& resp) {
    //bool rc = false;
    CommandId id = CU_INVALID;
    do {
        uint32_t size = sizeof(DataPacketPart1);
        if (resp.size() < size) {
            break;
        }

        const DataPacketPart1* p1 = NULL;
        if (!(p1 = reinterpret_cast<const DataPacketPart1*>(&resp[0]))) {
            break;
        }
        id = p1->unit.cmd;
    } while(false);

    return id;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::getResponseMarkType(const Payload& resp)
//! \brief getting response mark type
//! \param[in] resp response payload
//! \return ResponseMark
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ResponseMark AlertFSM::getResponseMarkType(const Payload& resp) {
    //bool rc = false;
    ResponseMark rm = RM_INVALID;
    do {
        uint32_t size = sizeof(DataPacketPart1);
        if (resp.size() < size) {
            break;
        }

        const DataPacketPart1* p1 = NULL;
        if (!(p1 = reinterpret_cast<const DataPacketPart1*>(&resp[0]))) {
            break;
        }
        rm = p1->unit.resp;
    } while(false);

    return rm;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::publishDefer(const aev::EventPublish& ev)
//! \brief deferring publishing request when not ready
//! \param[in] ev triggering publish event
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertFSM::publishDefer(const aev::EventPublish& ev) {
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: Not ready for sending alerts. Consider as failure");
    mParent.onPublishFailure();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::publish(const aev::EventPublish& ev)
//! \brief initiating publishing
//! \param[in] ev triggering publish event
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertFSM::publish(const aev::EventPublish& ev) {
    if (ev.xevinfo->getType() == nvm::DataRealTime::type()) {
        mLastSentCmd = CU_REALTIME_REPORT;
    } else if (ev.xevinfo->getType() == nvm::DataSupplementary::type()) {
        mLastSentCmd = CU_SUPPLEMENTARY_REPORT;
    } else if (ev.xevinfo->getType() == nvm::DataWarning::type()) {
        mLastSentCmd = CU_REALTIME_REPORT;
    } else {
        mLastSentCmd = CU_INVALID;
    }
    doPublish(ev.xevinfo);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::heartbeat(const aev::EventHeartbeat& ev)
//! \brief initiating sending heartbeat
//! \param[in] ev triggering heartbeat event
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertFSM::heartbeat(const aev::EventHeartbeat& ev) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    mLastSentCmd = CU_HEARTBEAT;
    doHeartbeat();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::postpone(const aev::EventLogout& ev)
//! \brief postponing logout
//! \param[in] ev triggering logout event
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertFSM::postpone(const aev::EventLogout& ev) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    mParent.postpone(ev.notify);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::postponeAlert(const aev::EventPublish& ev)
//! \brief postponing publishing alert
//! \param[in] ev triggering publish event
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertFSM::postponeAlert(const aev::EventPublish& ev) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    mParent.postponeAlert(ev.xevinfo);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::calcCrc(const Payload& pkt)
//! \brief calculation of CRC for packet
//! \param[in] pkt packet data
//! \return calculated CRC
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t AlertFSM::calcCrc(const Payload& pkt) {
    return ::calcCrc(pkt.data(), pkt.size());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::~AlertFSM()
//! \brief AlertFSM destructor
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// destructor
AlertFSM::~AlertFSM() {
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::doLogout()
//! \brief implementation of sending logout request
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertFSM::doLogout() {
    Settings* st = Settings::getInstance();
    vector<uint8_t> buff(GBT_DATA_PKT_MIN_SIZE + sizeof(VehicleLogoutData), 0);
    DataPacketPart1* pkt = reinterpret_cast<DataPacketPart1*>(buff.data());
    DataPacketPart2* pkt2 = reinterpret_cast<DataPacketPart2*>(&buff.back());
    pkt->encrypt = ENCRYPT_NOT;
    pkt->unit.cmd = CU_VEHICLE_LOGOUT;
    pkt->unit.resp = RM_COMMAND;
    pkt->unitLength = htons(sizeof(VehicleLogoutData));
    memcpy(pkt->start, GBT_START_CHAR_DATA, GBT_START_CHAR_SIZE);
    memcpy(pkt->vin, st->getVin().data(), st->getVin().size());

    pkt->unitData->logout.time = st->getChinaTime();
    pkt->unitData->logout.sn = htons(mLoginData.sn - 1);

    pkt2->crc = ::calcCrc(reinterpret_cast<uint8_t*>(&pkt->unit),
            GBT_DATA_CRC_MIN_SIZE + sizeof(VehicleLogoutData));
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: Send Logout message");
    mParent.sendData(buff, Settings::getInstance()->getTimeouts().logout);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::doPublish(const boost::shared_ptr<nvm::Data>& xevinfo)
//! \brief implementation of publishing alert
//! \param[in] xevinfo alert data
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertFSM::doPublish(const boost::shared_ptr<nvm::Data>& xevinfo) {
    Settings* st = Settings::getInstance();
    vector<uint8_t> buff(GBT_DATA_PKT_MIN_SIZE + xevinfo->getData().size(), 0);
    DataPacketPart1* pkt = reinterpret_cast<DataPacketPart1*>(buff.data());
    DataPacketPart2* pkt2 = reinterpret_cast<DataPacketPart2*>(&buff.back());
    pkt->encrypt = ENCRYPT_NOT;
    pkt->unit.resp = RM_COMMAND;
    pkt->unitLength = htons(xevinfo->getData().size());
    memcpy(pkt->start, GBT_START_CHAR_DATA, GBT_START_CHAR_SIZE);
    memcpy(pkt->vin, st->getVin().data(), st->getVin().size());

    switch (xevinfo->getType()) {
    case nvm::Data::Type::RealTtime:
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: Send RealTime message");
        pkt->unit.cmd = CU_REALTIME_REPORT;
        memcpy(&pkt->unitData->rtinfo, xevinfo->getData().data(),
                xevinfo->getData().size());
        break;
    case nvm::Data::Type::Warning:
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: Send Warning message as Supplementary");
        if(mParent.getWarningMode())
        {
            pkt->unit.cmd = CU_REALTIME_REPORT;
        }
        else
        {    
            pkt->unit.cmd = CU_SUPPLEMENTARY_REPORT;
        }
        memcpy(&pkt->unitData->sinfo, xevinfo->getData().data(),
                xevinfo->getData().size());
        break;
    case nvm::Data::Type::Supplementary:
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: Send Supplementary message");
        pkt->unit.cmd = CU_SUPPLEMENTARY_REPORT;
        memcpy(&pkt->unitData->sinfo, xevinfo->getData().data(),
                xevinfo->getData().size());
        break;
    default:
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "RMS: Unknown Data type will not be sent");
        break;

    }
    pkt2->crc = ::calcCrc(reinterpret_cast<uint8_t*>(&pkt->unit),
            GBT_DATA_CRC_MIN_SIZE + xevinfo->getData().size());
    mParent.sendData(buff, xevinfo->getTimeout());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::doHeartbeat()
//! \brief implementation of sending heartbeat
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertFSM::doHeartbeat() {
    Settings* st = Settings::getInstance();
    vector<uint8_t> buff(GBT_DATA_PKT_MIN_SIZE, 0);
    DataPacketPart1* pkt = reinterpret_cast<DataPacketPart1*>(buff.data());
    DataPacketPart2* pkt2 = reinterpret_cast<DataPacketPart2*>(&buff.back());
    pkt->encrypt = ENCRYPT_NOT;
    pkt->unit.resp = RM_COMMAND;
    pkt->unitLength = 0;
    memcpy(pkt->start, GBT_START_CHAR_DATA, GBT_START_CHAR_SIZE);
    memcpy(pkt->vin, st->getVin().data(), st->getVin().size());
    pkt->unit.cmd = CU_HEARTBEAT;
    pkt2->crc = ::calcCrc(reinterpret_cast<uint8_t*>(&pkt->unit),
            GBT_DATA_CRC_MIN_SIZE);
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: Send Heartbeat message");
    mParent.sendData(buff, Settings::getInstance()->getTimeouts().heartbeat);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertFSM::doLogin()
//! \brief implementation of sending login request
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertFSM::doLogin() {
    Settings* st = Settings::getInstance();
    vector<uint8_t> buff(GBT_DATA_PKT_MIN_SIZE + sizeof(VehicleLoginData), 0);
    DataPacketPart1* pkt = reinterpret_cast<DataPacketPart1*>(buff.data());
    DataPacketPart2* pkt2 = reinterpret_cast<DataPacketPart2*>(&buff.back());
    pkt->encrypt = ENCRYPT_NOT;
    pkt->unit.cmd = CU_VEHICLE_LOGIN;
    pkt->unit.resp = RM_COMMAND;
    pkt->unitLength = htons(sizeof(VehicleLoginData));
    memcpy(pkt->start, GBT_START_CHAR_DATA, GBT_START_CHAR_SIZE);
    memcpy(pkt->vin, st->getVin().data(), st->getVin().size());
    memcpy(pkt->unitData->login.iccid, st->getIccid().data(),
            st->getIccid().size());
    pkt->unitData->login.time = st->getChinaTime();
    if (mLoginData.time.day != pkt->unitData->login.time.day) {
        mLoginData.time = pkt->unitData->login.time;
        mLoginData.sn = VLL_LOGIN_SN_MIN;   // Reset every new day
    }
    pkt->unitData->login.sn = htons(mLoginData.sn);
    pkt->unitData->login.qcess = VLL_QCESS_REAL;
    pkt->unitData->login.lcess = VLL_LCESS_REAL;
    bzero(pkt->unitData->login.ccess, sizeof(pkt->unitData->login.ccess));

    pkt2->crc = ::calcCrc(reinterpret_cast<uint8_t*>(&pkt->unit),
            GBT_DATA_CRC_MIN_SIZE + sizeof(VehicleLoginData));
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: Send Login message");
    mParent.sendData(buff, Settings::getInstance()->getTimeouts().login);
}

}


