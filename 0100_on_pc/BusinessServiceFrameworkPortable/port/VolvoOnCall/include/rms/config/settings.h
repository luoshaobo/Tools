////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file settings.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   21-Sep-2018
////////////////////////////////////////////////////////////////////////////

#pragma once
#include <boost/chrono.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <rms/sys/semaphores.h>
#include <rms/dbus_interface/xEVDBusStuctures.h>

//#if defined(BUILD_CAS)
//#include <json/json.h>
//#include <json-glib-1.0/json-glib/json-glib.h>
//#elif defined(BUILD_X86)
//#include <json-c/json.h>
//#endif

//! \class Settings
//! \brief Store settings for xevcdm application
class Settings {
    //! \enum Settings::Constants
    //! \brief Store list of constants for xevcdm
    typedef enum constantId {
        PERIOD_RETRY_LOGIN_SHORT_S = 60,//60,     // TODO should be 60 (1 minutes)
        PERIOD_RETRY_LOGIN_LONG_S = 1800,//1800,    // TODO should be 1800 (30 minutes)
        PERIOD_RETRY_PUBLISH_SHORT_S = 60,//60,   // TODO should be 60 (1 minutes)
        PERIOD_RETRY_PUBLISH_LONG_S = 1800,//1800,  // TODO should be 1800 (30 minutes)
        PERIOD_HEARTBEAT_S = 10,
        PERIOD_PUBLISH_S = 10,
        TIMEOUT_HEARTBEAT_MIN_S = 1,
        TIMEOUT_HEARTBEAT_S = 5,
        TIMEOUT_PUBLISH_S = 5,
        TIMEOUT_PUBLISH_LOGIN_S = 5,
        TIMEOUT_PUBLISH_LOGOUT_S = 5,
        TIMEOUT_SOCKET_KEEP_ALIVE_HB = 2,
        TIMEOUT_SOCKET_KEEP_ALIVE_MIN_HB = 1,
        TIMEOUT_SOCKET_KEEP_ALIVE_MAX_HB = 60,
        MAX_SEMAPHORE_LOCKS = 10,
        MAX_WARNING_ALERTS = 10,
        MAX_TOTAL_ALERTS = 60480,
        CALENDAR_BASE_YEAR = 2000
    } Constants;

    typedef boost::date_time::local_adjustor<boost::posix_time::ptime, +8,
            boost::posix_time::no_dst> ChinaTZ;

public:
    //! \class Config
    //! \brief class responsible for reading and parsing configuration from file
    class Config {
    public:
        Config(Settings& stg);
        ~Config();
        bool load();
    private:
        Settings& mParent;
    };

public:
    ~Settings();
private:
    Settings();

    //! \var mSm
    //! \brief Semaphores object to manage reading and writing from multiple threads
    mutable Semaphores mSm;

    //! \class GuardWrite
    //! \brief class holding writing right on passed to constructor Semaphores from the moment of construction until destruction
    class GuardWrite {
        GuardWrite(GuardWrite& sm);
        GuardWrite& operator=(const GuardWrite& sm);
    public:
        explicit GuardWrite(Semaphores& sm): mSm(sm) {
            mSm.lockWrite();
        }
        ~GuardWrite() {
            mSm.unlockWrite();
        }
    private:
        Semaphores& mSm;
    };
    //! \class GuardRead
    //! \brief class holding reading right on passed to constructor Semaphores from the moment of construction until destruction
    class GuardRead {
        GuardRead(GuardRead& sm);
        GuardRead& operator=(const GuardRead& sm);
    public:
        explicit GuardRead(Semaphores& sm): mSm(sm) {
            mSm.lockRead();
        }
        ~GuardRead() {
            mSm.unlockRead();
        }
    private:
        Semaphores& mSm;
    };


public:

    //! \struct Timeouts
    //! \brief list of timeouts
    struct Timeouts {
        uint32_t login;
        uint32_t logout;
        uint32_t message;
        uint32_t heartbeat;
        uint32_t keepaliveHB;
    };

    //! \struct LogProp
    //! \brief packet logging properties
    struct LogProp {
        bool enabled;
        std::string path;
    };

    //! \struct RetryPeriod
    //! \brief packet retry period
    struct RetryPeriod {
        uint32_t periodRetryLoginShort;
        uint32_t periodRetryLoginLong;
        uint32_t periodRetryPublishShort;
        uint32_t periodRetryPublishLong;
    };

    static Settings* getInstance();
    std::string getIccid() const;
    void setIccid(const std::string& iccid);

    Timeouts getTimeouts() const;
    void setTimeouts(const Timeouts& tm);

    LogProp getLogProp() const;
    void setLogProp(const LogProp& lp);

    std::string getServerUrl() const;
    void setServerUrl(const std::string& serverUrl);
    std::string getVin() const;
    void setVin(const std::string& vin);
    uint32_t getMaxWarningAlerts() const;
    uint32_t getMaxTotalAlerts() const;
    void setMaxTotalAlerts(const uint32_t& maxTotalAlerts);
    uint32_t getRetryLoginLong() const;
    uint32_t getRetryLoginShort() const;
    uint32_t getRetryPublishLong() const;
    uint32_t getRetryPublishShort() const;
    Time getUTCTime() const;
    Time getChinaTime() const;
    Time getChinaTime(const Time& base) const;
    void setUTCTime(Time time);
    std::string getWorkdir() const;
    xEVConfigurations getDataCollectionParam() const;
    void setDataCollectionParam(const xEVConfigurations& newDcp);
    RetryPeriod getRetryPeriod() const;
    void setRetryPeriod(const RetryPeriod& retryPeriod);

    //! \var config
    //! \brief object responsible for reading and parsing configuration from file
    Config config;

private:
    //! \var serverUrl
    //! \brief string storing server URL
    std::string serverUrl;

    //! \var iccid
    //! \brief string storing ICCID
    std::string iccid;

    //! \var vin
    //! \brief string storing VIN
    std::string vin;

    //! \var dataCollectionParam
    //! \brief struct storing settings common with PHEV
    xEVConfigurations dataCollectionParam;

    //! \var time
    //! \brief variable with current time
    Time time;

    //! \var tm
    //! \brief string storing set of timeouts
    Timeouts tm;

    //! \var lp
    //! \brief struct storing packet logging properties
    LogProp lp;

    //! \var retryPeriod
    //! \brief struct storing packet retry period
    RetryPeriod retryPeriod;

    //! \var maxTotalAlerts
    //! \brief struct storing packet count
    uint32_t maxTotalAlerts;
};
