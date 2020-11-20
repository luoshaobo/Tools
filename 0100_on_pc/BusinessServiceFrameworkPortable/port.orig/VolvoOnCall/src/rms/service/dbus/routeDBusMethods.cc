////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file routeDBusMethods.cc
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
//! \fn  routeDBusPeriodicData(const PeriodicDBUSData* data)
//! \brief route setPeriodicData d-bus call to xevcdm by sending EVT_DBUS_PERODIC_DATA event
//! \param[in] data A Pointer to PeriodicDBUSData structure
//! \return void
////////////////////////////////////////////////////////////////////////////////
void routeDBusPeriodicData(const PeriodicDBUSData* data)
{
    boost::shared_ptr<Event> messageTosend = boost::make_shared<Event>(NotifyId::EVT_DBUS_PERODIC_DATA,
            Payload((uint8_t*)data,(uint8_t*)data + sizeof(PeriodicDBUSData)));

    //todo store gps time
    Settings::getInstance()->setUTCTime(data->time);
    DBusManager::getInstance()->sendEventWrapper(messageTosend);
}

///////////////////////////////////////////////////////////////////////////////
//! \fn  routeConnectionParams(const ServerConnectionParams* data)
//! \brief route setConnectionParam d-bus call to xevcdm by sending events
//! \param[in] data A Pointer to ServerConnectionParams structure
//! \return void
////////////////////////////////////////////////////////////////////////////////
void routeConnectionParams(const ServerConnectionParams* data)
{
    Settings::getInstance()->setIccid( std::string(data->ICCID, VLL_ICCID_LENGTH) );
    Settings::getInstance()->setServerUrl( std::string(data->serverURL, XEVSL_SERVER_URL_LENGTH) );
    Settings::getInstance()->setVin( std::string(data->VIN, GSL_VIN_LENGTH) );
    Settings::getInstance()->setUTCTime( data->time );

    Time time = Settings::getInstance()->getChinaTime();
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: ICCID = '%s' serverURL = '%s' VIN = '%s' time(mm/dd/yy hh:mm:ss)=%02d/%02d/%02d %02d:%02d:%02d"
         , Settings::getInstance()->getIccid().c_str()
         , Settings::getInstance()->getServerUrl().c_str()
         , Settings::getInstance()->getVin().c_str()
         , time.month, time.day,    time.year
         , time.hour,  time.minute, time.second
         );
}
///////////////////////////////////////////////////////////////////////////////
//! \fn  routeConfigurationParams(const xEVConfigurations* data)
//! \brief route setConfigurationParam d-bus call to xevcdm by sending events
//! \param[in] data A Pointer to xEVConfigurations structure
//! \return void
////////////////////////////////////////////////////////////////////////////////
void routeConfigurationParams(const xEVConfigurations* data)
{
    Settings::getInstance()->setDataCollectionParam(*data);

    boost::shared_ptr<Event> eventToSend = boost::make_shared<Event>(NotifyId::EVT_DBUS_SETTINGS_UPDATE);
    DBusManager::getInstance()->sendEventWrapper(eventToSend);
}

///////////////////////////////////////////////////////////////////////////////
//! \fn  routeStartXEVCDM()
//! \brief route startXEVCDM d-bus call to xevcdm by sending event EVT_DBUS_START
//! \return void
////////////////////////////////////////////////////////////////////////////////
void routeStartXEVCDM()
{
    boost::shared_ptr<Event> eventToSend = boost::make_shared<Event>(NotifyId::EVT_DBUS_START);
    DBusManager::getInstance()->sendEventWrapper(eventToSend);
}

///////////////////////////////////////////////////////////////////////////////
//! \fn  routeStopXEVCDM()
//! \brief route stopXEVCDM d-bus call to xevcdm by sending event EVT_DBUS_STOP
//! \return void
////////////////////////////////////////////////////////////////////////////////
void routeStopXEVCDM()
{
    boost::shared_ptr<Event> eventToSend = boost::make_shared<Event>(NotifyId::EVT_DBUS_STOP);
    DBusManager::getInstance()->sendEventWrapper(eventToSend);
}

///////////////////////////////////////////////////////////////////////////////
//! \fn  routeEnterWarningMode()
//! \brief route enterWarningMode d-bus call to xevcdm by sending event EVT_DBUS_ENTER_WARNING_MODE
//! \return void
////////////////////////////////////////////////////////////////////////////////
void routeEnterWarningMode()
{
    boost::shared_ptr<Event> eventToSend = boost::make_shared<Event>(NotifyId::EVT_DBUS_ENTER_WARNING_MODE);
    DBusManager::getInstance()->sendEventWrapper(eventToSend);
}

///////////////////////////////////////////////////////////////////////////////
//! \fn  routeWarningData(const WarningDbusData *pWarningData)
//! \brief route setWarningData d-bus call to xevcdm by sending event EVT_DBUS_WARNING_DATA
//! \param[in] pWarningData A Pointer to WarningDbusData structure
//! \return void
////////////////////////////////////////////////////////////////////////////////
void routeWarningData(const WarningDbusData *pWarningData)
{
    boost::shared_ptr<Event> eventToSend = boost::make_shared<Event>(NotifyId::EVT_DBUS_WARNING_DATA,
            Payload((uint8_t*)pWarningData, (uint8_t*)pWarningData + sizeof(WarningDbusData)));
    DBusManager::getInstance()->sendEventWrapper(eventToSend);
}

///////////////////////////////////////////////////////////////////////////////
//! \fn  routeEnterWarningMode()
//! \brief route getNextCableData d-bus call to xevcdm by sending event MSG_AM_READ_REQ
//! \return void
////////////////////////////////////////////////////////////////////////////////
void routeGetNextCableData()
{
    boost::shared_ptr<Message> eventToSend = boost::make_shared<Message>(NotifyId::MSG_AM_READ_REQ);
    DBusManager::getInstance()->sendMessageWrapper(
            Mailbox::SERVICE_ALERT_MANAGER, eventToSend);
}

///////////////////////////////////////////////////////////////////////////////
//! \fn  routeEnterWarningMode()
//! \brief route setCableDataAckStatus d-bus call to xevcdm by sending event MSG_AM_READ_ACK
//! \param[in] status ACK status
//! \return void
////////////////////////////////////////////////////////////////////////////////
void routeSetCableACKStatusData(char status)
{
    boost::shared_ptr<Message> eventToSend = boost::make_shared<Message>(NotifyId::MSG_AM_READ_ACK);
    DBusManager::getInstance()->sendMessageWrapper(
            Mailbox::SERVICE_ALERT_MANAGER, eventToSend);
}

///////////////////////////////////////////////////////////////////////////////
//! \fn  routeConvertCableData(WarningDbusData *warningData, char* convertedData, uint32_t *size)
//! \brief handler of getConvertToCableData d-bus call performing conversion of warning data to 
//!        cable data (data unit part of GBT packet)
//! \param[in] warningData pointer to warning data
//! \param[in, out] convertedData pointer to array where result will be stored
//! \param[in, out] size pointer to variable where size of result will be stored
//! \return void
////////////////////////////////////////////////////////////////////////////////
void routeConvertCableData(WarningDbusData *warningData, char* convertedData, uint32_t *size)
{
    std::vector<uint8_t> buf;

    warningData->periodicData.time = Settings::getInstance()->getChinaTime( warningData->periodicData.time );

    convertToDataUnitOfGbtPacket(buf, &warningData->periodicData, warningData->dtc);
    for(uint32_t i=0; i<buf.size(); i++)
    {
        convertedData[i]=buf[i];
    }
    *size = buf.size();
}
