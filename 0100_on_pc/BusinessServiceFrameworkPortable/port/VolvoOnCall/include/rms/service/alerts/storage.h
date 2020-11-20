////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file Storage.h
//  This file handles the communication with the VuC, using the OTP VCS-interface

// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   20-Sep-2018
////////////////////////////////////////////////////////////////////////////

#pragma once


#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <map>

#include <rms/config/settings.h>
#include "rms/util/utils.h"
#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace nvm {

//! \class Data
//! \brief base class for strong data on files system
class Data {
public:
    virtual ~Data() {}

     //! \enum Type
     //! \brief data type for storing
    typedef enum type {
        RealTtime = 1,
        Supplementary,
        Warning,
    } Type;

    //! \fn Data::getType()
    //! \brief getter for data type
    //! \return data type
    virtual Type getType() const = 0;

    //! \fn Data::getTypeName()
    //! \brief getter for data type in sting format
    //! \return data type in string
    virtual const char* getTypeName() const = 0;

    //! \fn Data::getTimeout()
    //! \brief getter for timeout TODO
    //! \return timeout in seconds
    virtual uint32_t getTimeout() const = 0;

    //! \fn Data::getData()()
    //! \brief getter for row data to be saved
    //! \return byte vector of row data
    inline const std::vector<uint8_t>& getData() const {
        return mData;
    }

    //! \fn Data::setData(const std::vector<uint8_t>& raw)
    //! \brief setter for data to be stored
    //! \param[in] raw Byte vector with row data to saving
    //! \return void
    inline void setData(const std::vector<uint8_t>& raw) {
        mData = raw;
    }

    //! \fn Data::getPath()
    //! \brief getter for file path of data
    //! \return file path of data
    inline const boost::filesystem::path& getPath() const {
        return mPath;
    }

    //! \fn Data::setPath(const boost::filesystem::path& path)
    //! \brief setter for file path where data should be stored
    //! \param[in] path File path where data should be stored
    //! \return void
    inline void setPath(const boost::filesystem::path& path) {
        mPath = path;
    }

    //! \fn Data::getId()
    //! \brief getter for unique ID of the data
    //! \return uniqueID of the data
    inline const uint64_t& getId() const {
        return mUniqueId;
    }
    inline bool operator==(const Data& b) const {
        return this->getId() == b.getId();
    }

protected:
    Data(): mUniqueId(DBG_getTicks(true)) {
        mUniqueId = (mUniqueId << 32) | mSeqId++;
    }
    explicit Data(const std::vector<uint8_t>& raw): mUniqueId(DBG_getTicks(true)),
            mData(raw) {
        mUniqueId = (mUniqueId << 32) | mSeqId++;
    }

protected:
    uint64_t mUniqueId;
    std::vector<uint8_t> mData;
    boost::filesystem::path mPath;
private:
    static uint32_t mSeqId;
};

/*! \class DataRealTime
    \brief Class for storing Real time data
*/
class DataRealTime: public Data {
public:
    DataRealTime() {}
    DataRealTime(const std::vector<uint8_t>& raw):
        Data(raw) {}
    virtual ~DataRealTime() {}
    inline virtual uint32_t getTimeout() const {
        return timeout();
    }
    inline virtual Type getType() const {
        return type();
    }
    inline virtual const char* getTypeName() const {
        return typeName();
    }
    static Type type() {
        return Type::RealTtime;
    }
    static const char* typeName() {
        return "realtime";
    }
    static uint32_t timeout() {
        return Settings::getInstance()->getTimeouts().message;
    }

};

//! \class DataSupplementary
//! \brief Class for storing Supplementary data
class DataSupplementary: public Data {
public:
    DataSupplementary() {}
    DataSupplementary(const std::vector<uint8_t>& raw):
        Data(raw) {}
    virtual ~DataSupplementary() {}
    inline virtual uint32_t getTimeout() const {
        return timeout();
    }
    inline virtual Type getType() const {
        return type();
    }
    static Type type() {
        return Type::Supplementary;
    }
    inline virtual const char* getTypeName() const {
        return DataSupplementary::typeName();
    }
    static const char* typeName() {
        return "supplementary";
    }
    static uint32_t timeout() {
        return Settings::getInstance()->getTimeouts().message;
    }
};

//! \class DataWarning
//! \brief Class for storing Warning data
class DataWarning: public Data {
public:
    DataWarning() {}
    DataWarning(const std::vector<uint8_t>& raw):
        Data(raw) {}
    virtual ~DataWarning() {}
    inline virtual uint32_t getTimeout() const {
        return timeout();
    }
    inline virtual Type getType() const {
        return type();
    }
    static Type type() {
        return Type::Warning;
    }
    inline virtual const char* getTypeName() const {
        return DataWarning::typeName();
    }
    static const char* typeName() {
        return "warning";
    }
    static uint32_t timeout() {
        return Settings::getInstance()->getTimeouts().message;
    }
};

boost::shared_ptr<Data> convert2Supplementary(boost::shared_ptr<Data>& xevinfo);

//! \class Storage
//! \brief This class implements working with file system based on boost lib
class Storage {

public:
    typedef std::deque<boost::shared_ptr<nvm::Data> > AlertQueue;
public:

    Storage(const std::string& workFolder);
    ~Storage();

    bool push(const boost::shared_ptr<Data>& xevinfo) const;

    boost::shared_ptr<Data> front() const;

    bool erase(const boost::shared_ptr<Data>& xevinfo) const;

    size_t size() const;

    bool sync(std::map<nvm::Data::Type, nvm::Storage::AlertQueue >& alerts) const;

private:
    enum Constants {
        LOG_LOAD_FILES = 10
    };

    void init() const;

    size_t size(const boost::filesystem::path& dir) const;

    bool save(const boost::shared_ptr<Data>& xevinfo) const;

    boost::shared_ptr<Data> load(const boost::filesystem::path& file) const;

    std::string genFileName() const;

    boost::filesystem::directory_entry mDirectory;
    boost::filesystem::path mLastLoaded;
};

}
