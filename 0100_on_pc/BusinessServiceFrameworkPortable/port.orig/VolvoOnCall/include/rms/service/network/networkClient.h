////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file networkClient.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   21-Sep-2018
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <boost/asio.hpp>
#include <boost/chrono.hpp>
#include <boost/optional.hpp>
#include <vector>

//! \class NetworkClient
//! \brief Implementation of client connection to server
class NetworkClient {
public:
    ///Constructor
    NetworkClient();

    bool connect(const std::string& host, const std::string& port);

    bool connect(const std::string& url);

    bool reconnect();

    void disconnect();

    bool isConnected() const;

    size_t write(const std::vector<uint8_t>& msg);

    size_t read(std::vector<uint8_t>& msg, const size_t maxSize);
    /*!
    */
    size_t read(std::vector<uint8_t>& msg, const size_t maxSize,
            const uint32_t& timeout);
    ~NetworkClient();

private:

    //! \struct ReadHandler
    //! \brief implementation control of blocking reading
    struct ReadHandler
    {
        ReadHandler(boost::asio::ip::tcp::socket& socket);
        void startTimeout(const uint32_t& timeout);
        void readHandler(const boost::system::error_code& ec, std::size_t bytes);
        static void setResult(boost::optional<boost::system::error_code>* a,
                    boost::system::error_code b);

        boost::asio::ip::tcp::socket& socket;
        boost::optional<boost::system::error_code> readResult;
        boost::optional<boost::system::error_code> timerResult;
        boost::asio::deadline_timer timer;
        size_t readBytes;
    };


    boost::asio::io_service mIoService;
    boost::asio::ip::tcp::socket mSocket;
    boost::asio::ip::tcp::resolver::query mQuery;
};

