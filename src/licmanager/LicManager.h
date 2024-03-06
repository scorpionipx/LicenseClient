#pragma once

#include <iostream>
#include <boost/asio.hpp>

#include "../security/Security.h"

using boost::asio::ip::tcp;
using namespace CryptoPP;

class LicManager {
public:
    LicManager(const std::string& applicationName, const std::string& applicationVersion, Security* security);
    ~LicManager();

    const std::string commandAcquire = "acquire";
    const std::string commandRelease = "release";
    const std::string commandExchange = "exchange";

    std::string acquiredSN;
    
    void connect(std::string host, std::string port);
    std::string acquire();
    void release();
private:
    Security* security;

    boost::asio::io_context io_context;
    boost::asio::any_io_executor executor;
    tcp::resolver resolver;
    tcp::socket socket;

    std::string application;
    std::string version;

    std::string exchange();
    void send(const std::string& message);
    std::string receive();
};

