////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file settings.cc
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   12-Sep-2018
////////////////////////////////////////////////////////////////////////////

#include <boost/date_time/gregorian_calendar.hpp>
#include <boost/filesystem.hpp>

#include <rms/config/settings.h>
#include "dlt/dlt.h"
#include "local_config_interface.hpp"

#define WORKDIR "/data/telematics/xev"

DLT_IMPORT_CONTEXT(dlt_voc);

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::getInstance()
//! \brief Provide instance of Settings. If it doesn't exist create one
//! \return pointer to Settings object
////////////////////////////////////////////////////////////////////////////////
Settings* Settings::getInstance() {
    static Settings s;
    return &s;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::Settings()
//! \brief constructor of Settings class
//! \return void
////////////////////////////////////////////////////////////////////////////////
Settings::Settings(): mSm(Constants::MAX_SEMAPHORE_LOCKS), config(*this),
        time { 0, 1, 1, 0, 0, 0 } {
    dataCollectionParam.directConnectionEnable = true;
    dataCollectionParam.dataPublishingPeriod = Constants::PERIOD_PUBLISH_S;
    dataCollectionParam.heartBeatPeriod = Constants::PERIOD_HEARTBEAT_S;
    dataCollectionParam.previousDataBufferingPeriod = 30;
    dataCollectionParam.warningModeDetectionDuringIgnitionOn = true;
    dataCollectionParam.warningModeDetectionDuringCharging = true;
    dataCollectionParam.warningModeLastingPeriod = 30;
    dataCollectionParam.periodOfDataCollectionAfterChargingCompletion = 100;
    tm.heartbeat = Constants::TIMEOUT_HEARTBEAT_S;
    tm.login = Constants::TIMEOUT_PUBLISH_LOGIN_S;
    tm.logout = Constants::TIMEOUT_PUBLISH_LOGOUT_S;
    tm.message = Constants::TIMEOUT_PUBLISH_S;
    tm.keepaliveHB = Constants::TIMEOUT_SOCKET_KEEP_ALIVE_HB;
    lp.enabled = false;
    lp.path = "";
    retryPeriod.periodRetryLoginShort = Constants::PERIOD_RETRY_LOGIN_SHORT_S;
    retryPeriod.periodRetryLoginLong = Constants::PERIOD_RETRY_LOGIN_LONG_S;
    retryPeriod.periodRetryPublishShort = Constants::PERIOD_RETRY_PUBLISH_SHORT_S;
    retryPeriod.periodRetryPublishLong = Constants::PERIOD_RETRY_PUBLISH_LONG_S;
    maxTotalAlerts = Constants::MAX_TOTAL_ALERTS;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::~Settings()
//! \brief destructor of Settings class
//! \return void
////////////////////////////////////////////////////////////////////////////////
Settings::~Settings(){

}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::getIccid()
//! \brief getter for ICCID string
//! \return ICCID string
////////////////////////////////////////////////////////////////////////////////
std::string Settings::getIccid() const {
    GuardRead lock(mSm);
    return iccid;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::setIccid(const std::string& iccid)
//! \brief setter for ICCID string
//! \param[in] iccid new value of ICCID string
//! \return void
////////////////////////////////////////////////////////////////////////////////
void Settings::setIccid(const std::string& iccid) {
    GuardWrite lock(mSm);
    this->iccid = iccid;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::getTimeouts()
//! \brief getter for Settings::Timeouts structure with all timeouts
//! \return Settings::Timeouts structure
////////////////////////////////////////////////////////////////////////////////
Settings::Timeouts Settings::getTimeouts() const {
    GuardRead lock(mSm);
    return tm;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::setTimeouts(const Settings::Timeouts& tm)
//! \brief setter for Settings::Timeouts structure with all timeouts
//! \param[in] tm new value of Settings::Timeouts structure
//! \return void
////////////////////////////////////////////////////////////////////////////////
void Settings::setTimeouts(const Settings::Timeouts& tm) {
    GuardWrite lock(mSm);
    this->tm = tm;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::getLogProp()
//! \brief getter for Settings::LogProp structure with packet logging properties
//! \return Settings::LogProp structure
////////////////////////////////////////////////////////////////////////////////
Settings::LogProp Settings::getLogProp() const {
    GuardRead lock(mSm);
    return lp;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::setLogProp(const Settings::LogProp& lp)
//! \brief setter for Settings::LogProp structure with packet logging properties
//! \param[in] lp new value of Settings::LogProp structure
//! \return void
////////////////////////////////////////////////////////////////////////////////
void Settings::setLogProp(const Settings::LogProp& lp) {
    GuardWrite lock(mSm);
    this->lp = lp;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::getServerUrl()
//! \brief getter for server URL
//! \return string with server URL
////////////////////////////////////////////////////////////////////////////////
std::string Settings::getServerUrl() const {
    GuardRead lock(mSm);
    return serverUrl;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::setServerUrl(const std::string& serverUrl)
//! \brief setter for server URL
//! \param[in] serverUrl new value of server URL
//! \return void
////////////////////////////////////////////////////////////////////////////////
void Settings::setServerUrl(const std::string& serverUrl) {
    GuardWrite lock(mSm);
    this->serverUrl = serverUrl;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::getVin()
//! \brief getter for VIN
//! \return string with VIN
////////////////////////////////////////////////////////////////////////////////
std::string Settings::getVin() const {
    GuardRead lock(mSm);
    return vin;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::setVin(const std::string& vin)
//! \brief setter for VIN
//! \param[in] vin new value of VIN
//! \return void
////////////////////////////////////////////////////////////////////////////////
void Settings::setVin(const std::string& vin) {
    GuardWrite lock(mSm);
    this->vin = vin;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::getMaxWarningAlerts()
//! \brief getter for MAX_WARNING_ALERTS constant
//! \return MAX_WARNING_ALERTS value
////////////////////////////////////////////////////////////////////////////////
uint32_t Settings::getMaxWarningAlerts() const {
    return Constants::MAX_WARNING_ALERTS;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::getMaxTotalAlerts()
//! \brief getter for MAX_TOTAL_ALERTS constant
//! \return MAX_TOTAL_ALERTS value
////////////////////////////////////////////////////////////////////////////////
uint32_t Settings::getMaxTotalAlerts() const {
    GuardRead lock(mSm);
    return this->maxTotalAlerts;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::setMaxTotalAlerts(const uint32_t& maxTotalAlerts)
//! \brief setter for MaxTotalAlerts
//! \param[in] MaxTotalAlerts new value of max total alerts count
//! \return void
////////////////////////////////////////////////////////////////////////////////
void Settings::setMaxTotalAlerts(const uint32_t& maxTotalAlerts) {
    GuardWrite lock(mSm);
    this->maxTotalAlerts = maxTotalAlerts;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::getRetryLoginLong()
//! \brief getter for PERIOD_RETRY_LOGIN_LONG_S constant
//! \return PERIOD_RETRY_LOGIN_LONG_S value
////////////////////////////////////////////////////////////////////////////////
uint32_t Settings::getRetryLoginLong() const {
    GuardRead lock(mSm);
    return this->retryPeriod.periodRetryLoginLong;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::getRetryLoginShort()
//! \brief getter for PERIOD_RETRY_LOGIN_SHORT_S constant
//! \return PERIOD_RETRY_LOGIN_SHORT_S value
////////////////////////////////////////////////////////////////////////////////
uint32_t Settings::getRetryLoginShort() const {
    GuardRead lock(mSm);
    return this->retryPeriod.periodRetryLoginShort;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::getRetryPublishLong()
//! \brief getter for PERIOD_RETRY_PUBLISH_LONG_S constant
//! \return PERIOD_RETRY_PUBLISH_LONG_S value
////////////////////////////////////////////////////////////////////////////////
uint32_t Settings::getRetryPublishLong() const {
    GuardRead lock(mSm);
    return this->retryPeriod.periodRetryPublishLong;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::getRetryPublishShort()
//! \brief getter for PERIOD_RETRY_PUBLISH_SHORT_S constant
//! \return PERIOD_RETRY_PUBLISH_SHORT_S value
////////////////////////////////////////////////////////////////////////////////
uint32_t Settings::getRetryPublishShort() const {
    GuardRead lock(mSm);
    return this->retryPeriod.periodRetryPublishShort;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::getUTCTime()
//! \brief getter for current UTC time
//! \return Time object with current UTC time
////////////////////////////////////////////////////////////////////////////////
Time Settings::getUTCTime() const {
    GuardRead lock(mSm);
    return time;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::getChinaTime(const Time& base)
//! \brief function for converting passed base time to China time zone
//! \return Time structure with converted time
////////////////////////////////////////////////////////////////////////////////
Time Settings::getChinaTime(const Time& base) const {
    GuardRead lock(mSm);
    if ((base.month > 12) || (base.month < 1)) {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMS: Wrong month should be 1..12, but actually is %d", base.month);
        return base;
    }

    if ((base.day > 31 ) || (base.day < 1)) {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMS: Wrong day should be 1..31, but actually is %d", base.day);
        return base;
    }

    boost::posix_time::ptime utc(
            boost::gregorian::date(CALENDAR_BASE_YEAR + base.year, base.month,
                    base.day),
            boost::posix_time::time_duration(base.hour, base.minute,
                    base.second));
    boost::posix_time::ptime china = ChinaTZ::utc_to_local(utc);

    Time chinaTime = {
        static_cast<uint8_t>(china.date().year()-2000),
        static_cast<uint8_t>(china.date().month()),
        static_cast<uint8_t>(china.date().day()),
        static_cast<uint8_t>(china.time_of_day().hours()),
        static_cast<uint8_t>(china.time_of_day().minutes()),
        static_cast<uint8_t>(china.time_of_day().seconds())
    };
    return chinaTime;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::getChinaTime()
//! \brief getter for current China time zone time
//! \return Time structure
////////////////////////////////////////////////////////////////////////////////
Time Settings::getChinaTime() const {
    return getChinaTime(time);
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::setUTCTime(Time time)
//! \brief setter for current UTC time
//! \param[in] time new value of current UTC time
//! \return void
////////////////////////////////////////////////////////////////////////////////
void Settings::setUTCTime(Time time) {
    GuardWrite lock(mSm);
    this->time = time;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::getWorkdir()
//! \brief getter for WORKDIR string
//! \return string with WORKDIR value
////////////////////////////////////////////////////////////////////////////////
std::string Settings::getWorkdir() const {
    return WORKDIR;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::getDataCollectionParam()
//! \brief getter for xEVConfigurations structure with settings common with PHEV
//! \return xEVConfigurations structure
////////////////////////////////////////////////////////////////////////////////
xEVConfigurations Settings::getDataCollectionParam() const {
    GuardRead lock(mSm);
    return dataCollectionParam;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::setDataCollectionParam(const xEVConfigurations& newDcp)
//! \brief setter for xEVConfigurations structure with settings common with PHEV
//! \param[in] newDcp new value of xEVConfigurations settings
//! \return void
////////////////////////////////////////////////////////////////////////////////
void Settings::setDataCollectionParam(const xEVConfigurations& newDcp) {
    GuardWrite lock(mSm);
    this->dataCollectionParam = newDcp;

//    resposeNormalTm = dataCollectionParam.dataPublishingPeriod;
    // dataPublishingPeriod divide by 2
//    resposeNormalTm = resposeNormalTm > 1 ? resposeNormalTm >> 1 : resposeNormalTm;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::getRetryPeriod()
//! \brief getter for RetryPeriod structure with settings common with PHEV
//! \return RetryPeriod structure
////////////////////////////////////////////////////////////////////////////////
Settings::RetryPeriod Settings::getRetryPeriod() const {
    GuardRead lock(mSm);
    return retryPeriod;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::setRetryPeriod(const RetryPeriod& retryPeriod)
//! \brief setter for RetryPeriod structure with settings common with PHEV
//! \param[in] retryPeriod new value of RetryPeriod settings
//! \return void
////////////////////////////////////////////////////////////////////////////////
void Settings::setRetryPeriod(const RetryPeriod& retryPeriod) {
    GuardWrite lock(mSm);
    this->retryPeriod = retryPeriod;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::Config::Config(Settings& stg)
//! \brief constructor of Config class
//! \param[in] stg reference to parent Settings object where parsed values will be stored
//! \return void
////////////////////////////////////////////////////////////////////////////////
Settings::Config::Config(Settings& stg): mParent(stg) {
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::Config::~Config()
//! \brief destructor of Config class
//! \return void
////////////////////////////////////////////////////////////////////////////////
Settings::Config::~Config(){

}

////////////////////////////////////////////////////////////////////////////////
//! \fn Settings::Config::load()
//! \brief for loading data from file 
//! \return loading result
////////////////////////////////////////////////////////////////////////////////
bool Settings::Config::load() {
    bool rc = false;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    
    lcfg::ILocalConfig* lcfg = lcfg::ILocalConfig::GetInstance();
    
    std::string cspIp = "198.18.101.67";
    rc = lcfg->GetString("VolvoOnCall", "RMS_CSP_IP", cspIp);
    int lPort = 19006;
    rc = lcfg->GetInt("VolvoOnCall", "RMS_CSP_PORT", lPort);
    std::string serverUrl = cspIp + ":" + std::to_string(lPort);
    mParent.setServerUrl(serverUrl);

    // To Do delete
    std::string vin = "LVSHFFAC4DF554906";
    rc = lcfg->GetString("VolvoOnCall", "RMS_VIN", vin);
    mParent.setVin(vin);

    // To Do delete
    std::string iccid = "89860616010045220858";
    rc = lcfg->GetString("VolvoOnCall", "RMS_ICCID", iccid);
    mParent.setIccid(iccid);

    // Load ack tiomeout
    int message = TIMEOUT_PUBLISH_S;
    int heartbeat = TIMEOUT_HEARTBEAT_S;
    int login = TIMEOUT_PUBLISH_LOGIN_S;
    int logout = TIMEOUT_PUBLISH_LOGOUT_S;
    int keepaliveHB = TIMEOUT_SOCKET_KEEP_ALIVE_HB;
    rc = lcfg->GetInt("VolvoOnCall", "RMS_MESSAGE_ACK_TM", message);
    rc = lcfg->GetInt("VolvoOnCall", "RMS_HEARTBEAT_ACK_TM", heartbeat);
    rc = lcfg->GetInt("VolvoOnCall", "RMS_LOGIN_ACK_TM", login);
    rc = lcfg->GetInt("VolvoOnCall", "RMS_LOGOUT_ACK_TM", logout);
    rc = lcfg->GetInt("VolvoOnCall", "RMS_KEEP_ALIVE_ACK_TM", keepaliveHB);
    Timeouts tm = mParent.getTimeouts();
    tm.message = message;
    tm.heartbeat = heartbeat;
    tm.login = login;
    tm.logout = logout;
    tm.keepaliveHB = keepaliveHB;
    if (tm.heartbeat < TIMEOUT_HEARTBEAT_MIN_S) {
        tm.heartbeat = TIMEOUT_HEARTBEAT_MIN_S;
    }
    if (tm.keepaliveHB > TIMEOUT_SOCKET_KEEP_ALIVE_MAX_HB) {
        tm.keepaliveHB = TIMEOUT_SOCKET_KEEP_ALIVE_MAX_HB;
    } else if (tm.keepaliveHB < TIMEOUT_SOCKET_KEEP_ALIVE_MIN_HB) {
        tm.keepaliveHB = TIMEOUT_SOCKET_KEEP_ALIVE_MIN_HB;
    }
    mParent.setTimeouts(tm);

    // Load period
    int heartBeatPeriod = PERIOD_HEARTBEAT_S;
    int dataPublishingPeriod = PERIOD_PUBLISH_S;
    rc = lcfg->GetInt("VolvoOnCall", "RMS_PERIOD_HEARTBEAT", heartBeatPeriod);
    rc = lcfg->GetInt("VolvoOnCall", "RMS_PERIOD_DATA_PUBLISHING", dataPublishingPeriod);
    xEVConfigurations dataCollectionParam = mParent.getDataCollectionParam();
    dataCollectionParam.heartBeatPeriod = heartBeatPeriod;
    dataCollectionParam.dataPublishingPeriod = dataPublishingPeriod;
    mParent.setDataCollectionParam(dataCollectionParam);

    int maxTotalAlerts = MAX_TOTAL_ALERTS;
    rc = lcfg->GetInt("VolvoOnCall", "RMS_MAX_TOTAL_ALERTS", maxTotalAlerts);
    uint32_t uMaxTotalAlerts = mParent.getMaxTotalAlerts();
    uMaxTotalAlerts = maxTotalAlerts;
    mParent.setMaxTotalAlerts(uMaxTotalAlerts);

    int periodRetryLoginShort = PERIOD_RETRY_LOGIN_SHORT_S;
    int periodRetryLoginLong = PERIOD_RETRY_LOGIN_LONG_S;
    int periodRetryPublishShort = PERIOD_RETRY_PUBLISH_SHORT_S;
    int periodRetryPublishLong = PERIOD_RETRY_PUBLISH_LONG_S;
    rc = lcfg->GetInt("VolvoOnCall", "RMS_PERIOD_RETRY_LOGIN_SHORT", periodRetryLoginShort);
    rc = lcfg->GetInt("VolvoOnCall", "RMS_PERIOD_RETRY_LOGIN_LONG", periodRetryLoginLong);
    rc = lcfg->GetInt("VolvoOnCall", "RMS_PERIOD_RETRY_PUBLISH_SHORT", periodRetryPublishShort);
    rc = lcfg->GetInt("VolvoOnCall", "RMS_PERIOD_RETRY_PUBLISH_LONG", periodRetryPublishLong);
    RetryPeriod retryPeriod = mParent.getRetryPeriod();
    retryPeriod.periodRetryLoginShort = periodRetryLoginShort;
    retryPeriod.periodRetryLoginLong = periodRetryLoginLong;
    retryPeriod.periodRetryPublishShort = periodRetryPublishShort;
    retryPeriod.periodRetryPublishLong = periodRetryPublishLong;
    mParent.setRetryPeriod(retryPeriod);

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: serverUrl = %s", mParent.serverUrl.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: vin = %s", mParent.vin.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: iccid = %s", mParent.iccid.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: tm.message = %u", mParent.tm.message);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: tm.heartbeat = %u", mParent.tm.heartbeat);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: tm.login = %u", mParent.tm.login);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: tm.logout = %u", mParent.tm.logout);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: tm.keepaliveHB = %u", mParent.tm.keepaliveHB);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: heartBeatPeriod = %u", mParent.dataCollectionParam.heartBeatPeriod);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: dataPublishingPeriod = %u", mParent.dataCollectionParam.dataPublishingPeriod);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: maxTotalAlerts = %u", mParent.maxTotalAlerts);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: periodRetryLoginShort = %u", mParent.retryPeriod.periodRetryLoginShort);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: periodRetryLoginLong = %u", mParent.retryPeriod.periodRetryLoginLong);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: periodRetryPublishShort = %u", mParent.retryPeriod.periodRetryPublishShort);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: periodRetryPublishLong = %u", mParent.retryPeriod.periodRetryPublishLong);

    return rc;
}

