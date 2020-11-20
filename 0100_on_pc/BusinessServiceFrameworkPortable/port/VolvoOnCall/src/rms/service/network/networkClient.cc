////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file networkClient.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Jiaojiao Shen
// @Init date   12-Sep-2018
////////////////////////////////////////////////////////////////////////////

#include <stddef.h>
#include <boost/bind.hpp>

#include <rms/util/utils.h>
#include <rms/service/network/networkClient.h>
#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace asio = boost::asio;
namespace ip = boost::asio::ip;

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkClient::NetworkClient():
//! \brief Constructor
//! \return  void
////////////////////////////////////////////////////////////////////////////////
NetworkClient::NetworkClient(): mSocket(mIoService), mQuery("") {
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkClient::~NetworkClient():
//! \brief destructor
//! \return  void
////////////////////////////////////////////////////////////////////////////////
NetworkClient::~NetworkClient() {
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkClient::connect(const std::string& host, const std::string& port)
//! \brief establish connection to server
//! \param[in] host Server host address
//! \param[in] port Server port
//! \return true if success
////////////////////////////////////////////////////////////////////////////////
bool NetworkClient::connect(const std::string& host,
        const std::string& port) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Connect to url \"%s:%s\"",host.c_str(), port.c_str());
    mQuery = ip::tcp::resolver::query(host, port);
    return reconnect();
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkClient::connect(const std::string& url)
//! \brief establish connection to server
//! \param[in] url Server URL
//! \return true if success
////////////////////////////////////////////////////////////////////////////////
bool NetworkClient::connect(const std::string& url) {
    std::string::const_iterator it = std::find(url.begin(), url.end(), ':');
    std::string host = "";
    std::string port = "";
    if (it != url.end()) {
        host = url.substr(0, std::distance(url.begin(), it));
        port = url.substr(std::distance(url.begin(), it + 1),
                std::distance(it + 1, url.end()));
    }
    return connect(host, port);
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkClient::reconnect()
//! \brief establish reconnect to server
//! \return true if success
////////////////////////////////////////////////////////////////////////////////
bool NetworkClient::reconnect() {
    bool rc = false;
    if (!mQuery.host_name().empty()) {
        ip::tcp::resolver resolver(mIoService);
        ip::tcp::resolver::iterator endpointIt = resolver.resolve(mQuery);
        ip::tcp::resolver::iterator end;
        boost::system::error_code error = asio::error::host_not_found;
        while(error && endpointIt != end) {
            mSocket.close();
            mSocket.connect(*endpointIt++, error);
        }
        if (!error) {
            rc = true;
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: connect success, endpointIt=%u",
                endpointIt);
        }
        else
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMS: connect error, endpointIt=%u, error=%u",
                endpointIt,
                error);
        }
    }
    return rc;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkClient::disconnect()
//! \brief make disconnect from server
//! \return void
////////////////////////////////////////////////////////////////////////////////
void NetworkClient::disconnect() {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    mSocket.close();
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkClient::isConnected()
//! \brief check current connection
//! \return true connection is available
////////////////////////////////////////////////////////////////////////////////
bool NetworkClient::isConnected() const {
    return mSocket.is_open();
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkClient::write(const std::vector<uint8_t>& msg)
//! \brief send data to server
//! \param[in] msg Message to be send
//! \return size of written data in bytes
////////////////////////////////////////////////////////////////////////////////
size_t NetworkClient::write(const std::vector<uint8_t>& msg) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    size_t size = 0;
    try {
        size = mSocket.write_some(asio::buffer(msg.data(), msg.size()));
    } catch (boost::system::system_error& e) {
        if (e.code() == boost::asio::error::eof) {
            disconnect();
        }
    }
    printDump(msg.data(), msg.size());
    return size;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkClient::read(std::vector<uint8_t>& msg, const size_t maxSize)
//! \brief read data from server
//! \param[out] msg Message where data should be stored
//! \param[in]  mazSize Maximum size of message
//! \return size read data
////////////////////////////////////////////////////////////////////////////////
size_t NetworkClient::read(std::vector<uint8_t>& msg, const size_t maxSize) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    size_t size = 0;
    try {
        if (msg.size() < maxSize) {
            msg.resize(maxSize);
        }
        size = mSocket.read_some(boost::asio::buffer(msg.data(), maxSize));
    } catch (boost::system::system_error& e) {
        if (e.code() == boost::asio::error::eof) {
            disconnect();
        }
    }
    msg.resize(size);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    printDump(msg.data(), msg.size());
    return msg.size();
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkClient::read(std::vector<uint8_t>& msg, const size_t maxSize, const uint32_t& timeout)
//! \brief Blocking read data from server
//! \param[out] msg Message where data should be stored
//! \param[in]  mazSize Maximum size of message
//! \param[in]  timeout Timeout in seconds for reading data
//! \return size read data
////////////////////////////////////////////////////////////////////////////////
size_t NetworkClient::read(std::vector<uint8_t>& msg, const size_t maxSize,
        const uint32_t& timeout) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    ReadHandler handler(mSocket);
    if (msg.size() < maxSize) {
        msg.resize(maxSize);
    }
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: Start reading with timeout %d", timeout);
    handler.startTimeout(timeout);
    mSocket.async_read_some(boost::asio::buffer(msg.data(), maxSize),
            boost::bind(&ReadHandler::readHandler, &handler,
                    asio::placeholders::error,
                    asio::placeholders::bytes_transferred));

    mSocket.get_io_service().reset();
    while (mSocket.get_io_service().run_one()) {
        if (handler.readResult) {
            handler.timer.cancel();
        }
        if (handler.timerResult && !handler.readResult) {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: Timeout %d has been expired", timeout);
            mSocket.cancel();
        }
    }
    msg.resize(handler.readBytes);
    printDump(msg.data(), msg.size());
    return msg.size();
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkClient::ReadHandler::ReadHandler(boost::asio::ip::tcp::socket& socket)
//! \brief Constructor
//! \param[in] socket Boost socket for read data from
//! \return  void
////////////////////////////////////////////////////////////////////////////////
NetworkClient::ReadHandler::ReadHandler(boost::asio::ip::tcp::socket& socket):
            socket(socket), readResult(), timerResult(),
            timer(socket.get_io_service()),
            readBytes(0) {
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkClient::ReadHandler::startTimeout(const uint32_t& timeout)
//! \brief Start timer for reading data. Reading will be interrupted in timeout seconds
//! \param[in] timeout. Timeout in second when reading will be interrupted
//! \return void
////////////////////////////////////////////////////////////////////////////////
void NetworkClient::ReadHandler::startTimeout(
        const uint32_t& timeout) {
    timer.expires_from_now(boost::posix_time::seconds(timeout));
    timer.async_wait(boost::bind(setResult, &timerResult, _1));
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkClient::ReadHandler::readHandler(const boost::system::error_code& ec, std::size_t bytes)
//! \brief Will beaclled when reading is finished
//! \param ec Result of read operation
//! \param bytes Count of bytes which were read
//! \return void
////////////////////////////////////////////////////////////////////////////////
void NetworkClient::ReadHandler::readHandler(const boost::system::error_code& ec, std::size_t bytes)
{
    readResult.reset(ec);
    readBytes = bytes;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkClient::ReadHandler::setResultboost::optional<boost::system::error_code>* a,
///                       boost::system::error_code b)
//! \brief
//! \param[in] a TODO
//! \param[in] b TODO
//! \return void
////////////////////////////////////////////////////////////////////////////////
void NetworkClient::ReadHandler::setResult(
        boost::optional<boost::system::error_code>* a,
        boost::system::error_code b) {
    a->reset(b);
}


