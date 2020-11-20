/**
* Copyright (C) 2016 Continental Automotive GmbH
*
* Licensed under the "Supplier unrestricted white box" category, as
* described in the 20160322-1_SW-amendment TCAM.pdf agreement
* between Volvo Cars and Continental Automotive GmbH.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/

/** @file vc_test.cpp
 * This file implements a test binary simulating telematic service
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Johan Bohlin & Ola Lilja
 * @date        07-Dec-2016
 ***************************************************************************/

#include <iostream>
#include <thread>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <future>
#include <chrono>
#include <cstring>

extern "C"
{
#include "tpsys.h"
}

#include "vc_log.hpp"
#include "vc_utility.hpp"

#include "vc_test.hpp"

std::atomic<bool> VCTest::exit_;
struct termios VCTest::oldt;

void VCTest::SignalHandler(int signum)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s: Force-quitting...", __FUNCTION__);

    UNUSED(signum);

    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);

    tpSYS_deinitIPC();
    vc::LOG_DEINIT();

    raise(SIGTERM);
}

void VCTest::ResponseCallback(vc::MessageBase *m, vc::ReturnValue status)
{
    vc::LOG(vc::LOG_INFO, "VCTest: %s.", __FUNCTION__);

    if (m == NULL) {
        vc::LOG(vc::LOG_INFO, "VCTest: %s: Message is NULL!");
        PrintPrompt();
        return;
    }

    vc::LOG(vc::LOG_INFO, "VCTest: %s: Message received (id = %s, client_id = %d, size = %d, status = %s)",
                __FUNCTION__, vc::ResponseStr[(vc::ResponseID)m->id_], m->client_id_, m->GetSize(), vc::ReturnValueStr[status]);

    if (status != vc::RET_OK) {
        vc::LOG(vc::LOG_ERR, "VCTest: %s: Error received (status = %s)!", __FUNCTION__, vc::ReturnValueStr[status]);
        PrintPrompt();
        delete m;
        return;
    }

    PrintMessage(m);

    if (m->type_ != vc::MESSAGE_RESPONSE) {
        vc::LOG(vc::LOG_ERR, "VCTest: %s: Message not a response!");
        PrintPrompt();
        delete m;
        return;
    }

    if (waitfor) {
        std::unique_lock<std::mutex> lk(mtx_msg);
        m_current = m;
        cv_msg.notify_all();
        cv_msg.wait(lk);
        if (m_current != NULL) {
            delete m_current;
            m_current = NULL;
        }
    } else {
        delete m;
        PrintPrompt();
    }
}

void VCTest::EventCallback(vc::MessageBase *m)
{
    vc::LOG(vc::LOG_INFO, "VCTest: %s", __FUNCTION__);

    if (m == NULL) {
        vc::LOG(vc::LOG_INFO, "VCTest: %s: Message is NULL!");
        PrintPrompt();
        return;
    }

    vc::LOG(vc::LOG_INFO, "VCTest: %s: Message received (id = %s, client_id = %d, size = %d) ",
                __FUNCTION__, vc::EventStr[(vc::EventID)m->id_], m->client_id_, m->GetSize());

    if (m->type_ != vc::MESSAGE_EVENT) {
        vc::LOG(vc::LOG_ERR, "VCTest: %s: Message not an event!");
        PrintPrompt();
        delete m;
        return;
    }

    PrintMessage(m);

    if (waitfor) {
        std::unique_lock<std::mutex> lk(mtx_msg);
        m_current = m;
        cv_msg.notify_all();
        cv_msg.wait(lk);
        if (m_current != NULL) {
            delete m_current;
            m_current = NULL;
        }
    } else {
        delete m;
        PrintPrompt();
    }
}

vc::ReturnValue VCTest::ParseFile(const std::string &fname, std::list<std::string> &cmd_list)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s", __FUNCTION__);

    std::ifstream file(fname);
    if (!file) {
        vc::LOG(vc::LOG_ERR, "VCTest: %s: Failed to open test-file '%s'!", __FUNCTION__, fname.c_str());
        return vc::RET_ERR_EXTERNAL;
    }

    std::stringstream ss;
    ss << file.rdbuf();
    file.close();

    std::string line;
    while (std::getline(ss, line)) {
        vc::LOG(vc::LOG_INFO, "VCTest: %s: Command: %s", __FUNCTION__, line.c_str());
        cmd_list.push_back(line);
    }

    return vc::RET_OK;
}

void VCTest::PrintPrompt()
{
    std::cout << std::flush << ">> " << std::flush;
}

void VCTest::PrintMenu()
{
    vc::LOG(vc::LOG_DEBUG, "Menu");
    vc::LOG(vc::LOG_DEBUG, "=======================================");
    vc::LOG(vc::LOG_DEBUG, "<ENTER> = Print this menu\n");
    vc::LOG(vc::LOG_DEBUG, "<CTRL-C> = Quit VCTest\n");
    vc::LOG(vc::LOG_DEBUG, "<TAB> = TAB-completion/cycle commands\n");
    vc::LOG(vc::LOG_DEBUG, "<ARROW UP> = Cycle command history back\n");
    vc::LOG(vc::LOG_DEBUG, "<ARROW DOWN> = Cycle command history forward\n");

    vc::LOG(vc::LOG_DEBUG, "Commands:");
    vc::LOG(vc::LOG_DEBUG, "-------------------------------");

    for (unsigned int i = 0; i < nc; i++)
        vc::LOG(vc::LOG_DEBUG, "\t%s", commands[i].name.c_str());


    vc::LOG(vc::LOG_DEBUG, "-------------------------------");
    vc::LOG(vc::LOG_DEBUG, "");

    vc::LOG(vc::LOG_DEBUG, "=======================================\n");
}

unsigned int VCTest::CountMatch()
{
    int nm = 0;

    for (unsigned int j = 0; j < nc; j++)
        if (match[j])
            nm++;

    return nm;
}

unsigned int VCTest::SetMatch(const char *buffer, unsigned int idx)
{
    int nm = 0;
    unsigned int i, j;

    for (j = 0; j < nc; j++)
        match[j] = 1;

    for (i = 0; i < idx; i++)
        for (j = 0; j < nc; j++)
            if (match[j]) {
                if (i < commands[j].name.length()) {
                    if (buffer[i] != commands[j].name[i]) {
                        match[j] = 0;
                        nm--;
                    }
                }
            }

    return nm;
}

int VCTest::GetMatch(unsigned int idx)
{
    unsigned int i = 0;

    for (unsigned int j = 0; j < nc; j++)
        if (match[j]) {
            if (i == idx)
                return j;
            i++;
        }

    return -1;
}

unsigned int VCTest::CountSpaces(const char *buffer, unsigned int idx)
{
    int ns = 0;
    unsigned int i;

    for (i = 0; i < idx; i++)
            if (buffer[i] == ' ')
                ns++;

    return ns;
}

void VCTest::ClearBuffer(char *buffer, unsigned int& idx)
{
    while (idx > 0) {
        std::cout << "\b \b";
        idx--;
    }

    buffer[0] = 0;
}

void VCTest::ClearParam(const char *buffer, unsigned int idx)
{
    tabparam_active = false;

    int i_fm = GetMatch(0);
    if (i_fm == -1)
        return;

    unsigned int param_idx = CountSpaces(buffer, idx) - 1;
    for (unsigned int i = 0; i < commands[i_fm].params[param_idx].length(); i++)
        std::cout << ' ';
    std::cout << "  ";
    for (unsigned int i = 0; i < commands[i_fm].params[param_idx].length(); i++)
        std::cout << '\b';
    std::cout << "\b\b";
}

void VCTest::TabComplete(char *buffer, unsigned int& idx)
{
    unsigned int j, nm;

    if (!tablookup_active) {
        nm = SetMatch(buffer, idx);

        tablookup_idx = 0;

        int i_fm = GetMatch(0);
        if (i_fm == -1)
            return;

        bool all_equal = true;
        bool idx_changed = false;
        while (all_equal == true) {
            for (j = i_fm; j < nc; j++)
                if (match[j]) {
                    if (idx >= commands[j].name.length())
                        all_equal = false;
                    else if (commands[i_fm].name[idx] != commands[j].name[idx])
                        all_equal = false;
                }
            if (all_equal) {
                buffer[idx] = commands[i_fm].name[idx];
                std::cout << buffer[idx];
                idx++;
                idx_changed = true;
            };
        }
        if (!idx_changed) {
            if (buffer[idx-1] == ' ') {
                unsigned int param_idx = CountSpaces(buffer, idx) - 1;
                if (param_idx < commands[i_fm].params.size()) {
                    std::cout << '<' << commands[i_fm].params[param_idx] << '>';
                    for (unsigned int i = 0; i < commands[i_fm].params[param_idx].length(); i++)
                        std::cout << '\b';
                    std::cout << "\b\b";
                    tabparam_active = true;
                }
            } else
                tablookup_active = true;
        }
    }

    if (tablookup_active) {
        ClearBuffer(buffer, idx);
        int cmd_idx = GetMatch(tablookup_idx);
        if (cmd_idx == -1)
            return;

        std::cout << commands[cmd_idx].name;
        commands[cmd_idx].name.copy(buffer, commands[cmd_idx].name.length(), 0);
        idx = commands[cmd_idx].name.length();
        tablookup_idx++;
        nm = CountMatch();
        if (tablookup_idx >= nm)
            tablookup_idx = 0;
    }
}

vc::ReturnValue VCTest::WaitForMessage(std::unique_lock<std::mutex>& lk, std::list<TSMessage>&& list_msg, int timeout)
{
   vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);

    using namespace std::chrono;

    time_point<system_clock> t_start = system_clock::now();
    milliseconds msecs(timeout * 1000);
    time_point<system_clock> t_timeout = t_start + msecs;

    bool timed_out;
    bool all_arrived = true;
    do {
        vc::LOG(vc::LOG_DEBUG, "VCTest: %s: Waiting for messages...", __FUNCTION__);
        cv_msg.wait_until(lk, t_timeout);

        milliseconds d = duration_cast<milliseconds>(system_clock::now() - t_start);
        timed_out = d >= msecs;
        if (timed_out) {
            vc::LOG(vc::LOG_DEBUG, "VCTest: %s: Timeout!", __FUNCTION__);
            cv_msg.notify_all();
            continue;
        }

        if (m_current == NULL) {
            vc::LOG(vc::LOG_DEBUG, "VCTest: %s: Incoming message is NULL.", __FUNCTION__);
            cv_msg.notify_all();
            continue;
        }

        vc::LOG(vc::LOG_DEBUG, "VCTest: %s: Compare %s with message-list (%dms left)...", __FUNCTION__,
                MessageStr(m_current->id_, m_current->type_), duration_cast<milliseconds>(msecs-d));
        all_arrived = true;
        for (std::list<TSMessage>::iterator it = list_msg.begin(); it != list_msg.end(); ++it) {
            if (!it->arrived) {
                if (m_current != NULL) {
                    it->arrived = (m_current->type_ == it->type ) && (m_current->id_ == it->id);
                    if (!it->arrived)
                        all_arrived = false;
                }
            } else {
                vc::LOG(vc::LOG_INFO, "VCTest: %s: Message %s arrived!", __FUNCTION__, MessageStr(it->id, it->type));
                delete m_current;
                m_current = NULL;
            }
        }
        cv_msg.notify_all();
    } while ((!all_arrived) && (!timed_out));

    if (timed_out) {
        vc::LOG(vc::LOG_INFO, "VCTest: %s: Timeout while waiting for messages!", __FUNCTION__);
        return vc::RET_ERR_TIMEOUT;
    } else {
        vc::LOG(vc::LOG_INFO, "VCTest: %s: All messages arrived!", __FUNCTION__);
        return vc::RET_OK;
    }
}

void VCTest::PrintResult(bool ok)
{
    vc::LOG(vc::LOG_INFO, "VCTest: %s: Test %s!", __FUNCTION__, ok ? "OK" : "FAILED");
}

int VCTest::IsCommand(std::string& str)
{

    for (unsigned int i = 0; i < nc; i++)
        if (!commands[i].name.compare(str))
            return i;

    return -1;
}

int VCTest::CreateCommand(std::string& cmd_line, std::string& cmd, std::vector<std::string> &args)
{
    std::stringstream ss(cmd_line);

    std::getline(ss, cmd, ' ');
    int ic = IsCommand(cmd);
    if (ic == -1) {
        vc::LOG(vc::LOG_DEBUG, "VCTest: %s: Unknown command '%s'!", __FUNCTION__, cmd.c_str());
        return -1;
    }

    args.clear();
    std::string arg;
    while (std::getline(ss, arg, ' '))
         args.push_back(arg);

    return ic;
}

vc::ReturnValue VCTest::Send_Request_LockDoor(vc::ReqDoorLockUnlock& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_LockDoor(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_HornAndLight(vc::ReqHornNLight& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_HornAndLight(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_SwitchToFlashBootloader()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = idiag_.Request_SwitchToFlashBootloader(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_AntennaSwitch(vc::ReqAntennaSwitch& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_AntennaSwitch(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_LastBackupBatteryState()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = idiag_.Request_LastBackupBatteryState(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_LanLinkTrigger(vc::ReqLanLinkTrigger& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_LanLinkTrigger(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_AdcValue(vc::ReqAdcValue& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_AdcValue(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_VucTraceConfig(vc::ReqVucTraceConfig& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_VucTraceConfig(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_VucTraceControl(vc::ReqVucTraceControl& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_VucTraceControl(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_BleDisconnect(vc::ReqBleDisconnect& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = ible_.Request_BleDisconnect(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_BleDisconnectAll()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = ible_.Request_BleDisconnectAll(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_BleGenerateBdak()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = ible_.Request_BleGenerateBdak(session_id_next++);
    mtx_msg.unlock();
    return ret;

}

vc::ReturnValue VCTest::Send_Request_BleGetBdak()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = ible_.Request_BleGetBdak(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_BleConfigDatacomService(vc::ReqBleCfgDataCommService& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = ible_.Request_BleConfigDatacomService(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_BleConfigIbeacon(vc::ReqBleCfgIbeacon& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = ible_.Request_BleConfigIbeacon(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_BleConfigGenericAccess(vc::ReqBleCfgGenAcc& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = ible_.Request_BleConfigGenericAccess(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_BleStartPairingService(vc::ReqBleStartPairService& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = ible_.Request_BleStartPairingService(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_BleStopPairingService()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = ible_.Request_BleStopPairingService(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_BleDataAccessAuthConfirmed(vc::ReqBleDataAccAuthCon& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = ible_.Request_BleDataAccessAuthConfirmed(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_BleWakeupResponse()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = ible_.Request_BleWakeupResponse(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_BleStartAdvertising(vc::ReqBleStartAdvertising& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = ible_.Request_BleStartAdvertising(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_BleStopAdvertising()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = ible_.Request_BleStopAdvertising(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_DtcEntries(vc::ReqDtcEntries& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = idiag_.Request_DtcEntries(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_DtcCtlSettings(vc::ReqDtcCtlSettings& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = idiag_.Request_DtcCtlSettings(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_IpWakeup(vc::ReqIpWakeup& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_IpWakeup(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_SetOhcLed(vc::ReqSetOhcLed& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_SetOhcLed(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_VINNumber()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_VINNumber(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_CarUsageMode()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_CarUsageMode(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_CarMode()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_CarMode(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetVucSwBuild()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = idiag_.Request_GetVucSwBuild(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetBleSwBuild()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = idiag_.Request_GetBleSwBuild(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_VucSwVersion()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = idiag_.Request_VucSwVersion(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_BleSwVersion()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = idiag_.Request_BleSwVersion(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_ElectEnergyLevel()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_ElectEnergyLevel(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetWifiData()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetWifiData(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetPSIMAvailability()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetPSIMAvailability(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetFuelType()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetFuelType(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetPropulsionType()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetPropulsionType(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetVucPowerMode()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetVucPowerMode(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetVucWakeupReason()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetVucWakeupReason(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetOhcBtnState()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetOhcBtnState(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetNumberOfDoors()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetNumberOfDoors(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetSteeringWheelPosition()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetSteeringWheelPosistion(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetFuelTankVolume()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetFuelTankVolume(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetAssistanceServices()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetAssistanceServices(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetAutonomousDrive()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetAutonomousDrive(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetCombinedInstrument()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetCombinedInstrument(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetTelematicModule()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetTelematicModule(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetConnectivity()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetConnectivity(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetConnectedServiceBooking()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetConnectedServiceBooking(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetTcamHwVersion()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = idiag_.Request_GetTcamHwVersion(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetRfVersion()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = idiag_.Request_GetRfVersion(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetFblSwBuild()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = idiag_.Request_GetFblSwBuild(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetBeltInformation()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetBeltInformation(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_CarConfigFactoryRestore()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = idiag_.Request_CarConfigFactoryRestore(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_SendPosnFromSatltCon(vc::ReqSendPosnFromSatltCon& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_SendPosnFromSatltCon(req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetDiagErrReport()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetDiagErrReport(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetDIDGlobalSnapshotData()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = idiag_.Request_GetDIDGlobalSnapshotData(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_DTCsDetected(vc::ReqDTCsDetected& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = idiag_.Request_DTCsDetected(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_PSIMData(vc::ReqPSIMData& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_PSIMData(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_ThermalMitigation(vc::ReqThermalMitigataion& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_ThermalMitigation(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

// Test

vc::ReturnValue VCTest::Send_Request_VGMTest(vc::ReqVGMTestMsg& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_VGMTest(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_IHUTest(vc::ReqIHUTestMsg& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_IHUTest(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Event_Test_FromVC(vc::EventTestFromVC& ev)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending event...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Event_Test_FromVC(&ev, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_Test_ToggleUsageMode()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_Test_ToggleUsageMode(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_Test_ToggleCarMode()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_Test_ToggleCarMode(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_Test_CrashState()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_Test_CrashState(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_Test_OhcState()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_Test_OhcState(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_Test_SetFakeValuesDefault()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_Test_SetFakeValuesDefault(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_Test_SetFakeValueInt(vc::ReqTestSetFakeValueInt& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_Test_SetFakeValueInt(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_Test_SetFakeValueStr(vc::ReqTestSetFakeValueStr& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_Test_SetFakeValueStr(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetTheftNotification()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetCarCfgTheftNotification(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetStolenVehicleTracking()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetCarCfgStolenVehicleTracking(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetGNSS()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetGNSSReceiver(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetDigitalVideoRecorder()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetDigitalVideoRecorder(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetDriverAlertControl()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetDriverAlertControl(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_GetDoorState()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_GetDoorState(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_WindowsCtrl(vc::ReqWinCtrl& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_WindowsCtrl(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}
vc::ReturnValue VCTest::Send_Request_SunRoofAndCurtCtrl(vc::ReqRoofCurtCtrl& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_SunRoofAndCurtCtrl(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}
vc::ReturnValue VCTest::Send_Request_WinVentilation(vc::ReqWinVentilation& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_WinVentilation(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_PM25Enquire()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_PM25Enquire(session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_VFCActivate(vc::ReqVFCActivate& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_VFCActivate(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

//add by uia93888
vc::ReturnValue VCTest::Send_Request_RMTFeedbackCertResult(vc::Req_RMTEngineSecurityResponse& req){
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_RmtEngineSecurityRespose(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}


vc::ReturnValue VCTest::Send_Request_EngineDelay(vc::Req_DelayEngineRunngTime& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_DelayEngineRunningTime(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;

}

vc::ReturnValue VCTest::Send_Request_Climate(vc::Req_OperateRMTClimate& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_RemoteClimateOperate(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;

}

vc::ReturnValue VCTest::Send_Request_SeatHeat(vc::Req_OperateRMTSeatHeat& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_RMTSeatHeat(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;

}

vc::ReturnValue VCTest::Send_Request_SeatVentilation(vc::Req_OperateRMTSeatVenti& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_RMTSeatVentilation(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

vc::ReturnValue VCTest::Send_Request_ParkingClimate(vc::Req_ParkingClimateOper_S& req){
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_ParkingClimateOperate(&req, session_id_next++);
    mtx_msg.unlock();
    return ret;
}

//end uia93888


vc::ReturnValue VCTest::Send_Request_Test_LocalConfig(vc::ReqTestSetLocalConfig& req)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.localCfgParam = %ld, localCfgParamValue = %ld", __FUNCTION__, req.localCfgParam, req.localCfgParamValue);

    mtx_msg.lock();
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s Sending request...", __FUNCTION__);
    vc::ReturnValue ret = igen_.Request_Test_SetLocalConfig(&req, session_id_next++);
    mtx_msg.unlock();
    
    return ret;
}

vc::ReturnValue VCTest::ExecuteCommand(std::string& cmd_line)
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s ('%s')", __FUNCTION__, cmd_line.c_str());

    std::vector<std::string> args;
    std::string cmd;
    int ic = CreateCommand(cmd_line, cmd, args);
    if (ic == -1)
        return vc::RET_ERR_INVALID_ARG;
    unsigned int n_args = args.size();

    vc::LOG(vc::LOG_DEBUG, "VCTest: %s: Command '%s' (idx = %d)", __FUNCTION__, cmd.c_str(), ic);
    for (unsigned int i = 0; i < commands[ic].params.size(); i++)
        vc::LOG(vc::LOG_DEBUG, "VCTest: %s: \t %s = %s", __FUNCTION__,  commands[ic].params[i].c_str(),
                (i >= n_args) ? "<default>" : args[i].c_str());

    std::unique_lock<std::mutex> lk(mtx_msg);

    switch (ic) {

    // VCTest-specific
    case LOAD_FILE: {
        if (n_args != 1) {
            vc::LOG(vc::LOG_DEBUG, "VCTest: %s: Command 'load' expects 1 arguments!", __FUNCTION__);
            return vc::RET_ERR_INVALID_ARG;
        }

        vc::ReturnValue ret = ParseFile(args[0], cmd_list);
        if (ret != vc::RET_OK) {
            vc::LOG(vc::LOG_WARN, "VCTest: %s: Failed to parse test-file '%s'!", __FUNCTION__, args[0].c_str());
            return ret;
        }

        mtx_msg.unlock();

        for (std::list<std::string>::iterator it=cmd_list.begin(); it != cmd_list.end(); ++it)
            ExecuteCommand(*it);
        break;
    }
    case DELAY: {
        if (args.size() != 1) {
            vc::LOG(vc::LOG_DEBUG, "VCTest: %s: Command 'delay' expects 1 arguments!", __FUNCTION__);
            return vc::RET_ERR_INVALID_ARG;
        }

        int delay = std::stoi(args[0], nullptr, 0);
        if (delay == 0) {
            vc::LOG(vc::LOG_WARN, "VCTest: %s: Illegal value for delay!", __FUNCTION__);
            return vc::RET_ERR_INVALID_ARG;
        }
        vc::LOG(vc::LOG_INFO, "VCTest: %s: Waiting %d seconds...", __FUNCTION__, delay);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay*1000));
        break;
    }
    case WAIT_FOR: {
        if (args.size() < 1) {
            vc::LOG(vc::LOG_DEBUG, "VCTest: %s: Command 'wait_for' needs a message-name as argument!", __FUNCTION__);
            return vc::RET_ERR_INVALID_ARG;
        }

        vc::MessageType type_waitfor;
        vc::MessageID id_waitfor;
        if (!MessageNameLookup(args[0], type_waitfor, id_waitfor)) {
            vc::LOG(vc::LOG_DEBUG, "VCTest: %s: Message %s not known!", __FUNCTION__, args[0].c_str());
            return vc::RET_ERR_INVALID_ARG;
        }

        waitfor = true;
        WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{type_waitfor, (vc::MessageID)id_waitfor, FALSE},
                                                  }, (n_args > 1) ? std::stoi(args[1], nullptr, 0) : 5);
        waitfor = false;

        break;
    }
    case QUIT: {
        exit_ = true;
        break;
    }


    // Vuc
    case REQUEST_LOCKDOOR: {
        vc::ReqDoorLockUnlock req;
        req.centralLockReq = (n_args > 0) ? std::stoi(args[0], nullptr, 0) : 0;
//        req.centralLockReqTailgate = (n_args > 1) ? std::stoi(args[1], nullptr, 0) : 0;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_LockDoor, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_HORNANDLIGHT: {
        vc::ReqHornNLight req;
        req.mode = (n_args > 0) ? (vc::HNLMode)std::stoi(args[0], nullptr, 0) : vc::HL_REQUEST_HORN_AND_LIGHT;

        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_HornAndLight, this, std::ref(req));
        vc::ReturnValue ret_stage1 = WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_HORNNLIGHT, FALSE},
                                                    TSMessage{vc::MESSAGE_EVENT, (vc::MessageID)vc::EVENT_HORNNLIGHTSTATE, FALSE}
                                                  }, 2);
        if (ret_stage1 == vc::RET_OK) {
            vc::ReturnValue ret_stage2 = WaitForMessage(lk, std::list<TSMessage>{
                                                        TSMessage{vc::MESSAGE_EVENT, (vc::MessageID)vc::EVENT_HORNNLIGHTSTATE, FALSE}
                                                    }, 8);
            PrintResult((ret_stage1 == vc::RET_OK) && (ret_stage2 == vc::RET_OK));
        }
        else
            PrintResult(false);
        
        waitfor = false;

        break;
    }
    case REQUEST_SWITCHTOFLASHBOOTLOADER: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_SwitchToFlashBootloader, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_ANTENNASWITCH: {
        vc::ReqAntennaSwitch req;
        req.mode = (n_args > 0) ? (vc::AntennaMode)std::stoi(args[0], nullptr, 0) : vc::ANTENNA_HANDLED_BY_NAD_REQUEST_PHONE1;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_AntennaSwitch, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_ANTENNASWITCH, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_LANLINKTRIGGER: {
        vc::ReqLanLinkTrigger req;
        req.trigger = (n_args > 0) ? (vc::LinkTrigger)std::stoi(args[0], nullptr, 0) : vc::LINK_TRIGGER_WLAN_OR_ETH_ESTABLISHED;
        req.session_state = (n_args > 1) ? (vc::LinkSessionState)std::stoi(args[1], nullptr, 0) : vc::LINK_SESSION_DOIP_ONGOING;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_LanLinkTrigger, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_ADC_VALUE: {
        vc::ReqAdcValue req;
        req.id = (n_args > 0) ? (vc::AdcId)std::stoi(args[0], nullptr, 0) : vc::ADC_ID_LED1_STATUS;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_AdcValue, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_ADCVALUE, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_VUCTRACE_CONFIG: {
        vc::ReqVucTraceConfig req;
        memset(&req, 0, sizeof(req));
        req.bus = (n_args > 0) ? (vc::TraceBus)std::stoi(args[0], nullptr, 0) : vc::TRACE_BUS_IPC;
        req.config[0].module_id = (n_args > 1) ? (vc::TraceModuleId)std::stoi(args[1], nullptr, 0) : vc::TRACE_MODULE_ID_CANH;
        req.config[0].module_lvl = (n_args > 2) ? (vc::TraceModuleLevel)std::stoi(args[2], nullptr, 0) : vc::TRACE_REPORTS_DISABLE;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_VucTraceConfig, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_VUCTRACE_CONFIG, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_VUCTRACE_CONTROL: {
        vc::ReqVucTraceControl req;
        req.mode = (n_args > 0) ? (vc::TracingMode)std::stoi(args[0], nullptr, 0) : vc::TRACING_SUSPEND_FOR_TIMEINTERVAL;
        req.interval = (n_args > 1) ? (unsigned int)std::stoi(args[1], nullptr, 0) : 0x3f;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_VucTraceControl, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_VUCTRACE_CONTROL, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_BLE_DISCONNECT: {
        vc::ReqBleDisconnect req;
        req.connection_id = (n_args > 0) ? std::stoi(args[0], nullptr, 0) : 0;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_BleDisconnect, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_BLE_DISCONNECT, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_BLE_DISCONNECTALL: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_BleDisconnectAll, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_BLE_DISCONNECTALL, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_BLE_GENERATEBDAK: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_BleGenerateBdak, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_BLE_GENERATEBDAK, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_BLE_GETBDAK: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_BleGetBdak, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_BLE_GETBDAK, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_BLE_CONFIG_DATACOMSERVICE: {
        vc::ReqBleCfgDataCommService req;
        char test_uuid[16] = {0x42, 0xBE, 0xA7, 0x33, 0xD1, 0x5C, 0x99, 0x67, 0xD3, 0x39, 0xAB, 0xAD, 0x1D, 0x3A, 0x52, 0x31};
        std::memcpy(req.service_uuid, test_uuid, sizeof(req.service_uuid));
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_BleConfigDatacomService, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_BLE_CONFIG_DATACOMSERVICE, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_BLE_CONFIG_IBEACON: {
        vc::ReqBleCfgIbeacon req;
        char test_uuid[16] = {0xE2, 0x0A, 0x39, 0xF4, 0x73, 0xF5, 0x4B, 0xC4, 0x18, 0x64, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x62};
        req.major = (n_args > 1) ? std::stoi(args[1], nullptr, 0) : 0;
        req.minor = (n_args > 2) ? std::stoi(args[2], nullptr, 0) : 0;
        std::memcpy(req.proximity_uuid, test_uuid, sizeof(req.proximity_uuid));
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_BleConfigIbeacon, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_BLE_CONFIG_IBEACON, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_BLE_CONFIG_GENERICACCESS: {
        vc::ReqBleCfgGenAcc req { "VOLVO XC90 TSSERVER" };
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_BleConfigGenericAccess, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_BLE_CONFIG_GENERICACCESS, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_BLE_STARTPARINGSERVICE: {
        vc::ReqBleStartPairService req{
            0xFE43, // service_uuid
            "XC90, 2017, 123456" // name_string <model>, <year>, <six last digits of vin>
        };
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_BleStartPairingService, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_BLE_STARTPARINGSERVICE, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_BLE_STOPPARINGSERVICE: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_BleStopPairingService, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_BLE_STOPPARINGSERVICE, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_BLE_DATAACCESSAUTHCONFIRMED: {
        vc::ReqBleDataAccAuthCon req;
        req.connection_id = (n_args > 0) ? std::stoi(args[0], nullptr, 0) : 0;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_BleDataAccessAuthConfirmed, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_BLE_STARTADVERTISING: {
        vc::ReqBleStartAdvertising req;
        req.adv_frame = (n_args > 0) ? (vc::BLEAdvType)std::stoi(args[0], nullptr, 0) : vc::ADV_FRAME_1_AND_2;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_BleStartAdvertising, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_BLE_STARTADVERTISING, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_BLE_STOPADVERTISING: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_BleStopAdvertising, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_BLE_STOPADVERTISING, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_BLE_WAKEUP_RESPONSE: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_BleWakeupResponse, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_DTC_ENTRIES: {
        vc::ReqDtcEntries req;
        req.request_id = (n_args > 0) ? (vc::DtcReqEntries)std::stoi(args[0], nullptr, 0) : vc::DTC_SEND_ALL_ENTRIES;
        req.event_id = (n_args > 1) ? (unsigned int)std::stoi(args[1], nullptr, 0) : 0;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_DtcEntries, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_DTC_CTLSETTINGS: {
        vc::ReqDtcCtlSettings req;
        req.settings = (n_args > 0) ? (vc::DtcCtlSetting)std::stoi(args[0], nullptr, 0) : vc::DTC_CONTROL_SETTINGS_ON;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_DtcCtlSettings, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_DTCCTLSETTINGS, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_IPWAKEUP: {
        vc::ReqIpWakeup req;
        req.prio = (n_args > 0) ? (vc::IpWakeupPrio)std::stoi(args[0], nullptr, 0) : vc::IP_WAKEUP_PRIO_NORMAL;
        req.res_group = (n_args > 1) ? (vc::IpWakeupResGroup)std::stoi(args[1], nullptr, 0) : vc::IP_WAKEUP_RES_GROUP_1;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_IpWakeup, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_SETOHCLED: {
        vc::ReqSetOhcLed req;
        req.sos_red_status_blink = (n_args > 0) ? (vc::LedBlink)std::stoi(args[0], nullptr, 0) : vc::LEDBLINK_OFF;
        req.sos_red_status_repeat = (n_args > 1) ? (vc::LedRepeat)std::stoi(args[1], nullptr, 0) : vc::LEDREPEAT_ENDLESS;
        req.sos_red_duty = (n_args > 2) ? (uint8_t)std::stoi(args[2], nullptr, 0) : 0;
        req.sos_white_status_blink = (n_args > 3) ? (vc::LedBlink)std::stoi(args[3], nullptr, 0) : vc::LEDBLINK_1HZ;
        req.sos_white_status_repeat = (n_args > 4) ? (vc::LedRepeat)std::stoi(args[4], nullptr, 0) : vc::LEDREPEAT_1_TIME;
        req.sos_white_duty = (n_args > 5) ? (uint8_t)std::stoi(args[5], nullptr, 0) : 100;
        req.voc_status_blink = (n_args > 6) ? (vc::LedBlink)std::stoi(args[6], nullptr, 0) : vc::LEDBLINK_ON;
        req.voc_status_repeat = (n_args > 7) ? (vc::LedRepeat)std::stoi(args[7], nullptr, 0) : vc::LEDREPEAT_GLOW;
        req.voc_duty = (n_args > 8) ? (uint8_t)std::stoi(args[8], nullptr, 0) : 100;
        req.brightness  = (n_args > 9) ? (uint8_t)std::stoi(args[9], nullptr, 0) : 255;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_SetOhcLed, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETBELTINFORMATION: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetBeltInformation, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETBELTINFORMATION, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_CARCONFIGFACTRESTORE: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_CarConfigFactoryRestore, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_CARCONFIGFACTRESTORE, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETDIAGERRREPORT: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetDiagErrReport, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETDIAGERRREPORT, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETDIDGLOBALSNAPSHOTDATA: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetDIDGlobalSnapshotData, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETDIDGLOBALSNAPSHOTDATA, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_DTCSDETECTED: {
        vc::ReqDTCsDetected req;
        memset(&req, 0, sizeof(vc::ReqDTCsDetected));
        uint8_t dtcid = (n_args > 0) ? std::stoi(args[0], nullptr, 0) : 0;
        uint8_t value = (n_args > 1) ? std::stoi(args[1], nullptr, 0) : 0;
        if (dtcid >= LENGTH(req.status) && value > vc::DTC_DETECTED) {
            vc::LOG(vc::LOG_DEBUG, "VCTest: %s: invalid dtcid: %d or value: %d!", __FUNCTION__, dtcid, value);
            return vc::RET_ERR_INVALID_ARG;
        }
        req.status[dtcid] = (vc::DTCDetectStatus)value;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_DTCsDetected, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_DTCSDETECTED, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_PSIMDATA: {
        vc::ReqPSIMData req;
        memset(&req, 0, sizeof(vc::ReqPSIMData));
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_PSIMData, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_PSIMDATA, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_THERMALMITIGATION: {
        vc::ReqThermalMitigataion req;
        int8_t nad_temperature = (n_args > 0) ? std::stoi(args[0], nullptr, 0) : 0;
        req.nad_temperature = nad_temperature;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_ThermalMitigation, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_THERMALMITIGATION, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_WINCTRL: {
        vc::ReqWinCtrl req;
        req.mode = (vc::WinCtrlReqMode)((n_args > 0) ? std::stoi(args[0], nullptr, 0) : 0);
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_WindowsCtrl, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_SUNROOGCTRL: {
        vc::ReqRoofCurtCtrl req;
        req.mode = (vc::RoofCurtCtrlReqMode)((n_args > 0) ? std::stoi(args[0], nullptr, 0) : 0);
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_SunRoofAndCurtCtrl, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_VENTILATION: {
        vc::ReqWinVentilation req;
        req.mode = (vc::WinVentiReqMode)((n_args > 0) ? std::stoi(args[0], nullptr, 0) : 0);
        req.value = (n_args > 1) ? std::stoi(args[1], nullptr, 0) : 0;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_WinVentilation, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_PM25: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_PM25Enquire, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_VFCACTIVATE: {
        vc::ReqVFCActivate req;
        req.id = (n_args > 0) ? std::stoi(args[0], nullptr, 0) : 0;
        req.type = (n_args > 1) ? std::stoi(args[1], nullptr, 0) : 0;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_VFCActivate, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
#if 1 //add by uia9388
    case REQUEST_FEEDBACKCERTRESULT:{
        vc::Req_RMTEngineSecurityResponse req;
        req.imobVehRemReqCmd = 5;
        req.imobVehDataRemReq0 = 0;
        req.imobVehDataRemReq1 = 0;
        req.imobVehDataRemReq2 = 0;
        req.imobVehDataRemReq3 = 0;
        req.imobVehDataRemReq4 = 0;
        req.imobVehRemTmrOrSpdLim = 0;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_RMTFeedbackCertResult, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_EVENT, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 1) == vc::RET_OK);
        break;
    }
    case REQUEST_ENGINEDELAY:{
        vc::Req_DelayEngineRunngTime req;
        req.telmEngDelayTi = 17;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_EngineDelay, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_EVENT, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_CLIMATE:{
        vc::Req_OperateRMTClimate req;
        req.telmClimaReq = vc::OnOffNoReq_On;
        req.telmClimaTSetTempRange =2;
        req.hmiCmptmtTSpSpcl = vc::HmiCmptmtSpSpcl_Norm;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_Climate, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_EVENT, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_SEATHEAT:{
        vc::Req_OperateRMTSeatHeat req;
        req.telmSeatDrvHeat = vc::TelmSeatCliamtLvl_Lvl2;
        req.telmSeatPassHeat = vc::TelmSeatCliamtLvl_Lvl2;
        req.telmSeatSecLeHeat = vc::TelmSeatCliamtLvl_Lvl2;
        req.telmSeatSecRiHeat = vc::TelmSeatCliamtLvl_Lvl2;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_SeatHeat, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_EVENT, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_SEATVENTILATION:{
        vc::Req_OperateRMTSeatVenti req;
        req.telmSeatDrvVenti = vc::TelmSeatCliamtLvl_Lvl2;
        req.telmSeatPassVenti = vc::TelmSeatCliamtLvl_Lvl2;
        req.telmSeatSecLeVenti = vc::TelmSeatCliamtLvl_Lvl2;
        req.telmSeatSecRiVenti = vc::TelmSeatCliamtLvl_Lvl2;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_SeatVentilation, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_EVENT, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_PARKINGCLIMATE:{
        vc::Req_ParkingClimateOper_S req;
        req.climaRqrd1 = 1;
        req.telmClimaTmr = 20;
        req.climaTmrStsTelmTqrd = 1;
        req.telmClimaTSetTempRange = 10;
        req.hmiCmptmtSpSpcl = 1;
        req.seatHeatDurgClimaEnadFromTelm = 1;
        req.steerWhlHeatgDurgClimaEnadFromTelm = 1;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_ParkingClimate, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_EVENT, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 1) == vc::RET_OK);
        break;
    }
#endif
    // VGM
    case REQUEST_SENDPOSNFROMSATLTCON: {
        vc::ReqSendPosnFromSatltCon req;
        req.PosnLat = 0; // Latitude
        req.PosnLgt = 1; //Longitude
        req.PosnAlti = 3; // Altitide from mean sea level
        req.PosnSpd = 4; // Speed
        req.PosnVHozl = 5; // Horizontal velocity
        req.PosnVVert = 6; // Vertical velocity
        req.PosnDir = 7; // Heading from true north
        req.TiForYr = 8; // UTC year
        req.TiForMth = 9; // UTC month
        req.TiForDay = 10; // UTC day
        req.TiForHr = 11; // UTC hour
        req.TiForMins = 12; // UTC minute
        req.TiForSec = 13; // UTC second
        req.SatltSysNo1InUse = 1; // GPS used for positioning
        req.SatltSysNo2InUse = 0; // GLONASS used for positioning
        req.SatltSysNo3InUse = 1; // Galileo used for positioning
        req.SatltSysNo4InUse = 0; // SBAS used for positioning	Enum
        req.SatltSysNo5InUse = 1; // QZSS L1 used for positioning
        req.SatltSysNo6InUse = 0; // QZSS L1-SAIF used for positioning
        req.SatltPosnStsPrm1 = vc::MORE_THAN_THREE_SATELITES_SOLUTION; // Navigation solution status
        req.SatltPosnStsPrm2 = 1; // DGPS usage in solution
        req.SatltPosnStsPrm3 = 0; // Self ephemeris data usage in solution
        req.NoOfSatltForSysNo1 = 23; // Number of GPS satellites used for positioning
        req.NoOfSatltForSysNo2 = 24; // Number of GLONASS satellites used for positioning
        req.NoOfSatltForSysNo3 = 25; // Number of Galileo satellites used for positioning
        req.NoOfSatltForSysNo4 = 26; // Number of SBAS satellites used for positioning
        req.NoOfSatltForSysNo5 = 27; // Number of QZSS L1 satellites used for positioning
        req.NoOfSatltForSysNo6 = 28; // Number of QZSS L1-SAIF satellites used for positioning
        req.PrePosnDil = 29; // Position Dilution Of Precision
        req.PreHozlDil = 30; // Horizontal Dilution Of Precision
        req.PreVertDil = 31; // Vertical Dilution Of Precision
        req.PreTiDil = 32; // Time Dilution Of Precision
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_SendPosnFromSatltCon, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    
    // IHU
    
    // Data-storage
    case REQUEST_VINNUMBER: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_VINNumber, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_VINNUMBER, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_CARMODE: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_CarMode, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_CARMODE, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_CARUSAGEMODE: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_CarUsageMode, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_CARUSAGEMODE, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_ELECTENERGYLEVEL: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_ElectEnergyLevel, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_ELECTENGLVL, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETVUCPOWERMODE: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetVucPowerMode, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETVUCPOWERMODE, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETVUCWAKEUPREASON: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetVucWakeupReason, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETVUCWAKEUPREASON, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }

    // Message-processor
    case REQUEST_GETVUCSWBUILD: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetVucSwBuild, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETVUCSWBUILD, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETBLESWBUILD: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetBleSwBuild, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETBLESWBUILD, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_VUCSWVERSION: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_VucSwVersion, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_VUCSWVERSION, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_BLESWVERSION: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_BleSwVersion, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_BLESWVERSION, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_LASTBACKUPBATTERYSTATE: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_LastBackupBatteryState, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_LASTBUBSTATE, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETWIFIDATA_CARCONFIG: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetWifiData, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETWIFIDATA, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETPSIMAVAILABILITY_CARCONFIG: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetPSIMAvailability, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETPSIMAVAILABILITY, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETFUELTYPE_CARCONFIG: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetFuelType, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETFUELTYPE, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETPROPULSIONTYPE_CARCONFIG: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetPropulsionType, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETPROPULSIONTYPE, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETOHCBTNSTATE: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetOhcBtnState, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETOHCBTNSTATE, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETNUMBEROFDOORS: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetNumberOfDoors, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETNUMBEROFDOORS, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETSTEERINGWHEELPOSITION: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetSteeringWheelPosition, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETSTEERINGWHEELPOSITION, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETFUELTANKVOLUME: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetFuelTankVolume, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETFUELTANKVOLUME, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETASSISTANCESERVICES: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetAssistanceServices, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETASSISTANCESERVICES, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETAUTONOMOUSDRIVE: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetAutonomousDrive, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETAUTONOMOUSDRIVE, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETCOMBINEDINSTRUMENT: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetCombinedInstrument, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETCOMBINEDINSTRUMENT, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETTELEMATICMODULE: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetTelematicModule, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETTELEMATICMODULE, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETCONNECTIVITY: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetConnectivity, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETCONNECTIVITY, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETCONNECTEDSERVICEBOOKING: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetConnectedServiceBooking, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETCONNECTEDSERVICEBOOKING, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETTCAMHWVERSION: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetTcamHwVersion, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETTCAMHWVERSION, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETRFVERSION: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetRfVersion, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETRFVERSION, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETFBLSWBUILD: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetFblSwBuild, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETFBLSWBUILD, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETTHEFTNOTIFICATION: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetTheftNotification, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETCARCFGTHEFTNOTIFICATION, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETSTOLENVEHICLETRACKING: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetStolenVehicleTracking, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETCARCFGSTOLENVEHICLETRACKING, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETGNSS: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetGNSS, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETGNSSRECEIVER, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETDIGITALVIDEORECORDER: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetDigitalVideoRecorder, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETDIGITALVIDEORECORDER, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETDRIVERALERTCONTROL: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetDriverAlertControl, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETDRIVERALERTCONTROL, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_GETDOORSTATE: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_GetDoorState, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_GETDOORSTATE, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }

    // Test
    case REQUEST_VGMTEST: {
        vc::ReqVGMTestMsg req;
        req.id = (n_args > 0) ? std::stoi(args[0], nullptr, 0) : 127;
        req.return_ack = (n_args > 1) ? (std::stoi(args[1], nullptr, 0) > 0) : true;
        req.return_res = (n_args > 2) ? (std::stoi(args[2], nullptr, 0) > 0) : true;
        strcpy(req.company, "Conti");
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_VGMTest, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_VGMTESTMSG, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_IHUTEST: {
        vc::ReqIHUTestMsg req;
        req.id = (n_args > 0) ? std::stoi(args[0], nullptr, 0) : 127;
        req.return_ack = (n_args > 1) ? (std::stoi(args[1], nullptr, 0) > 0) : true;
        req.return_res = (n_args > 2) ? (std::stoi(args[2], nullptr, 0) > 0) : true;
        strcpy(req.city, "Lund");
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_IHUTest, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_IHUTESTMSG, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case EVENT_TEST_FROMVC: {
        vc::EventTestFromVC ev;
        ev.vc = 22;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Event_Test_FromVC, this, std::ref(ev));
        lk.unlock();
        break;
    }
    case REQUEST_TEST_TOGGLEUSAGEMODE: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_Test_ToggleUsageMode, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_TEST_TOGGLECARMODE: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_Test_ToggleCarMode, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_TEST_CRASHSTATE: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_Test_CrashState, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_TEST_OHCSTATE: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_Test_OhcState, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_TEST_SETFAKEVALUESDEFAULT: {
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_Test_SetFakeValuesDefault, this);
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_TEST_SETFAKEVALUEINT: {
        if (n_args < 3)
            break;
        vc::ReqTestSetFakeValueInt req;
#if 1 //nieyj fix klocwork 189039 & 189040
        STRNCPY(req.collection, args[0].c_str(), sizeof(req.collection));
        STRNCPY(req.name, args[1].c_str(), sizeof(req.name));
#else
        STRNCPY(req.collection, args[0].c_str(), ((args[0].size()+1) > sizeof(req.collection)) ? sizeof(req.collection) : args[0].size()+1);
        STRNCPY(req.name, args[1].c_str(), ((args[1].size()+1) > sizeof(req.name)) ? sizeof(req.name) : args[1].size()+1);
#endif
        req.value = (n_args > 2) ? std::stoi(args[2], nullptr, 0) : 0;
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_Test_SetFakeValueInt, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case REQUEST_TEST_SETFAKEVALUESTR: {
        if (n_args < 3)
            break;
        vc::ReqTestSetFakeValueStr req;
#if 1 //nieyj fix klocwork 189041 & 189042 & 189043
        STRNCPY(req.collection, args[0].c_str(), sizeof(req.collection));
        STRNCPY(req.name, args[1].c_str(), sizeof(req.name));
        STRNCPY(req.value, args[2].c_str(), sizeof(req.value));

#else
        STRNCPY(req.collection, args[0].c_str(), ((args[0].size()+1) > sizeof(req.collection)) ? sizeof(req.collection) : args[0].size()+1);
        STRNCPY(req.name, args[1].c_str(), ((args[1].size()+1) > sizeof(req.name)) ? sizeof(req.name) : args[1].size()+1);
        STRNCPY(req.value, args[2].c_str(), ((args[2].size()+1) > sizeof(req.value)) ? sizeof(req.value) : args[2].size()+1);
#endif
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_Test_SetFakeValueStr, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 2) == vc::RET_OK);
        waitfor = false;
        break;
    }
    case SEND_TEST_LOCAL_CONFIG: {
        if (n_args  < 2)
            break;
        
        vc::ReqTestSetLocalConfig req;
        req.localCfgParam = std::stoi(args[0], nullptr, 0);
        req.localCfgParamValue = std::stoi(args[1], nullptr, 0);
        
        waitfor = true;
        std::future<vc::ReturnValue> fut_req = std::async(&VCTest::Send_Request_Test_LocalConfig, this, std::ref(req));
        PrintResult(WaitForMessage(lk, std::list<TSMessage>{
                                                    TSMessage{vc::MESSAGE_RESPONSE, (vc::MessageID)vc::RES_REQUESTSENT, FALSE},
                                                  }, 1) == vc::RET_OK);
        waitfor = false;
        break;
    }
    default:
        return vc::RET_ERR_INTERNAL;
    }

    waitfor = false;

    mtx_msg.unlock();

    PrintPrompt();
    return vc::RET_OK;
}



// Public

VCTest::VCTest() :
        ivc_(vc::IVehicleComm::GetInstance()),
        igen_(vc::IVehicleComm::GetGeneralInterface()),
        ible_(vc::IVehicleComm::GetBLEInterface()),
        idiag_(vc::IVehicleComm::GetDiagnosticsInterface())
{
    DLT_REGISTER_APP("TSER", "VCTest");
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    exit_ = false;
    waitfor = false;
    m_current = NULL;
}

VCTest::~VCTest()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    DLT_UNREGISTER_APP();
}

VCTestReturnValue VCTest::Init()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);

    signal(SIGINT, SignalHandler);

    // Init TPSys
    tpSYS_ReturnCode_t tpSYS_ret = tpSYS_initIPC(E_SYS_IPC_DBUS, NULL, NULL);
    if (tpSYS_ret == E_SYS_IPC_RET_FAILURE) {
        vc::LOG(vc::LOG_ERR, "VCTest: %s: tpSYS_initIPC failed!", __FUNCTION__);
        return VCTESTRET_ERR_EXTERNAL;
    } else
        vc::LOG(vc::LOG_INFO, "VCTest: %s: tpSYS-IPC initialized.", __FUNCTION__);

    if (ivc_.Init([this](vc::MessageBase *message, vc::ReturnValue status){return VCTest::ResponseCallback(message, status);},
                [this](vc::MessageBase *message){return VCTest::EventCallback(message);}) != vc::RET_OK)
        return VCTESTRET_ERR_EXTERNAL;
    
    return VCTESTRET_OK;
}

VCTestReturnValue VCTest::Start()
{
    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);
    
    vc::ReturnValue ret;
    ret = ivc_.EnableGeneralEvents();
    if (ret != vc::RET_OK) {
        vc::LOG(vc::LOG_WARN, "VCTest: %s: Failed to enable general-events (ret = %s)!", __FUNCTION__, vc::ReturnValueStr[ret]);
        return VCTESTRET_ERR_EXTERNAL;
    }
    ret = ivc_.EnableBLEEvents();
    if (ret != vc::RET_OK) {
        vc::LOG(vc::LOG_WARN, "VCTest: %s: Failed to enable BLE-events (ret = %s)!", __FUNCTION__, vc::ReturnValueStr[ret]);
        return VCTESTRET_ERR_EXTERNAL;
    }
    ret = ivc_.EnableDiagnosticsEvents();
    if (ret != vc::RET_OK) {
        vc::LOG(vc::LOG_WARN, "VCTest: %s: Failed to enable diagnostics-events (ret = %s)!", __FUNCTION__, vc::ReturnValueStr[ret]);
        return VCTESTRET_ERR_EXTERNAL;
    }
    
    std::string input;

    PrintMenu();

    char buffer[256];
    unsigned int idx = 0;

    static struct termios newt;
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);
    newt.c_lflag &= ~ECHO;
    newt.c_cc[VMIN] = 1;
    newt.c_cc[VTIME] = 0;
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);

    char c;
    unsigned int j;
    bool history_active;

    std::string cmd_cur;
    std::list<std::string> history;
    std::list<std::string>::iterator i_history;

    while (!exit_) {

        cmd_cur.clear();
        ClearBuffer(buffer, idx);
        for (j = 0; j < nc; j++)
            match[j] = 1;
        history_active = false;
        std::cout << "\b\b\b   \b\b\b";
        PrintPrompt();

        bool esc = false;
        while (!esc) {
            c = getchar();
            switch (c) {
            case 27:
                 c = getchar();
                 c = getchar();
                 if (c == 65) {
                    if (history.empty())
                        continue;

                    if (history_active) {
                        if (i_history != --history.end())
                            i_history++;
                    } else {
                        if (idx > 0)
                            cmd_cur = std::string(buffer, idx);
                        else
                            cmd_cur.clear();
                        i_history = history.begin();
                        history_active = true;
                    }

                    ClearBuffer(buffer, idx);
#if 1 //nieyj fix klocwork 189064
                    if (i_history != history.end()) {
                        std::cout << *i_history;
                        (*i_history).copy(buffer, (*i_history).length(), 0);
                        idx = (*i_history).length();
                    }
#else
                    std::cout << *i_history;
                    (*i_history).copy(buffer, (*i_history).length(), 0);
                    idx = (*i_history).length();
#endif
                 } else if (c == 66) {
                    if (!history_active)
                        continue;

                    if (i_history == history.begin()) {
                        ClearBuffer(buffer, idx);
                        if (!cmd_cur.empty()) {
                            std::cout << cmd_cur;
                            cmd_cur.copy(buffer, cmd_cur.length(), 0);
                            idx = cmd_cur.length();
                        } else
                            idx = 0;
                        history_active = false;
                    } else {
                        i_history--;
                        ClearBuffer(buffer, idx);
                        std::cout << *i_history;
                        (*i_history).copy(buffer, (*i_history).length(), 0);
                        idx = (*i_history).length();
                    }
                 }
                 continue;
            case '\t':
                TabComplete(buffer, idx);
                continue;
            case '\n':
                esc = true;
                //std::cout << (char)c;
                continue;
            case '\b':
            case 0x7F:
                if (idx > 0) {
                    if (tabparam_active)
                        ClearParam(buffer, idx);
                    std::cout << "\b \b";
                    idx--;
                    history_active = false;
                    tablookup_active = false;
                }
                continue;
            }

            if (idx >= 256)
                continue;

            if (tabparam_active)
                ClearParam(buffer, idx);

            std::cout << (char)c;
            buffer[idx] = c;
            idx++;
            history_active = false;
            tablookup_active = false;
        }

        if (idx == 0) {
            std::cout << "\033[2J\033[1;1H"; // Clear screen and position cursor at 1,1
            PrintMenu();
            PrintPrompt();
            //for (unsigned int i = 0; i < idx; i++)
            //    std::cout << buffer[i];
            continue;
        }

        buffer[idx] = 0;

        std::string cmd_line(buffer);
        if (ExecuteCommand(cmd_line) != vc::RET_OK)
            vc::LOG(vc::LOG_ERR, "VCTest: %s: Failed to execute command '%s'!", __FUNCTION__, cmd_line.c_str());
        else
            history.push_front(buffer);
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    tpSYS_deinitIPC();
    vc::LOG_DEINIT();
    vc::LOG(vc::LOG_INFO, "VCTest: %s: Quitting...", __FUNCTION__);
    return VCTESTRET_OK;
}

vc::ReturnValue VCTest::RunFile(const std::string &fname)
{
    vc::ReturnValue ret;

    ret = ParseFile(fname, cmd_list);
    if (ret != vc::RET_OK) {
        vc::LOG(vc::LOG_WARN, "DataStorage: %s: Failed to parse test-file '%s'!", __FUNCTION__, fname.c_str());
        return ret;
    }

    std::vector<std::string> args;
    for (std::list<std::string>::iterator it=cmd_list.begin(); it != cmd_list.end(); ++it)
        if (ExecuteCommand(*it) != vc::RET_OK)
            ret = vc::RET_ERR_INTERNAL;

    return ret;
}

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    vc::LOG(vc::LOG_DEBUG, "VCTest: %s.", __FUNCTION__);

    VCTest tss;
    VCTestReturnValue ret = tss.Init();
    if (ret != VCTESTRET_OK) {
        vc::LOG(vc::LOG_DEBUG, "VCTest: %s: Init failed: %s.", __FUNCTION__, VCTestReturnValueStr[ret]);
        return ret;
    }

    if (argc > 1) {
        vc::LOG(vc::LOG_DEBUG, "VCTest: %s: Running test-file '%s'", __FUNCTION__, argv[1]);
        return tss.RunFile(argv[1]);
    }

    return tss.Start();
}
