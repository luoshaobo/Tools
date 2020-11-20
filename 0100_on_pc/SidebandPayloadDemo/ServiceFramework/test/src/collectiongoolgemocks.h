/**
 * @file
 *          collectiongoolgemocks.h
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

#ifndef COLLECTION_GOOGLE_MOCKS_H__
#define ISTATE_MACHINE_MASTER_H__

#include <gmock/gmock.h>

#include "itimermaster.h"
#include "istatemachinemaster.h"
#include "nadif/inadifdataservices.h"
#include "datacommunication/idatacommunicationmqtt.h"


class MockTimerMaster : public ITimerMaster < IStatemachine, Event >
{
public:

  MOCK_METHOD3(startTimer, Timer(IStatemachine *, TimeElapse::Difference, Event&));
  MOCK_METHOD2(stopTimer, bool(IStatemachine *, Timer::TimerId));
  MOCK_CONST_METHOD0(getUtcTimestamp, long long());
};

class MockStatemachineMaster : public IStatemachineMaster
{
public:

  MOCK_METHOD3(startStatemachine, void(IStatemachine *, TimeElapse::Difference, IStatemachine::StatemachineId));
  MOCK_METHOD2(startStatemachine, void(IStatemachine *, TimeElapse::Difference));
  MOCK_METHOD1(abortStatemachine, void(IStatemachine::StatemachineId));
  MOCK_METHOD2(fireEvent, void(IStatemachine::StatemachineId, Event &));
  MOCK_METHOD3(fireStatemachineResult, void(IStatemachine::StatemachineId, StatemachineType, IStatemachine::StatemachineResult));
  MOCK_METHOD1(getStatemachine, IStatemachine*(IStatemachine::StatemachineId));
  MOCK_METHOD0(getNewStatemachineId, IStatemachine::StatemachineId());
  MOCK_METHOD1(isStatemachineStillExisting, bool(IStatemachine *));
  MOCK_METHOD2(isStatemachineStillExisting, bool(StatemachineType, IStatemachine *&));
};

class MockDataCommunicationMqtt : public IDataCommunicationMqtt
{
  public:

    MOCK_CONST_METHOD0(isMqttConnected, bool());
    MOCK_CONST_METHOD0(getBrokerUrl, std::string());
    MOCK_METHOD7(startSession, JobId(std::string, std::string, std::string, std::string, MqttSSLOptions *,int, std::string));
    MOCK_METHOD1(stopSession, JobId(int));
    MOCK_METHOD5(publish, JobId(std::string, StringPtr, int, bool, unsigned long));
    MOCK_METHOD2(subscribe, JobId(std::string , int ));
    MOCK_METHOD1(unsubscribe, bool(std::string));
};

class MockNadIfDataServices : public INadIfDataServices
{
public:
  MOCK_METHOD2(NADIF_data_set_pd_configuration, NADIF_STATUS(PacketDataConfigurationAPN, NADIF_PD_CONFIG_DATA*));
  MOCK_METHOD2(NADIF_data_get_pd_configuration, NADIF_STATUS(NOTIFICATION_REQUEST_DATA *, NADIF_CONTEXT_ID *));
  MOCK_METHOD1(NADIF_data_start, NADIF_STATUS(PacketDataConfigurationAPN));
  MOCK_METHOD2(NADIF_data_stop, NADIF_STATUS(NOTIFICATION_REQUEST_DATA *, NADIF_DATA_STOP_RQST *));
  MOCK_METHOD2(NADIF_data_get_status, NADIF_STATUS(PacketDataConfigurationAPN, NADIF_DATA_STATUS_DATA *data_status));
  MOCK_CONST_METHOD1(getNadifDataStatusData, NADIF_DATA_STATUS_DATA(PacketDataConfigurationAPN));
  MOCK_CONST_METHOD1(isPacketDataConfigurationValid, bool(PacketDataConfigurationAPN));

};


#endif //COLLECTION_GOOGLE_MOCKS_H__
