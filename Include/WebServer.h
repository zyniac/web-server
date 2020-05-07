#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <sstream>
#include <FileReader.h>
#include "HTTPResolver.h"
#include <HeaderValue.h>
#include <sys/stat.h>
#include <WinLin.h>
#include "WebConfig.h"

using boost::asio::ip::tcp;

class WebServer
{
public:
    WebServer(const std::map<const char*, const char*, WebConfig::cmp_str>& types);
    WebServer() = delete;

    bool start(const std::string& dir, unsigned short port);
    bool stop();
    static void serve(std::shared_ptr<tcp::socket> socket, const boost::system::error_code code, std::string& webDir,
                        const std::map<const char*, const char*, WebConfig::cmp_str>& types, std::string& defaultFile);
    void serveAccept();
    void setDir(const std::string& path);
    void setDefaultFile(std::string& name);
    static void createFileResponse(HTTPResolver::HTTPResponse& response, const HTTPResolver::HTTPRequestData& data, const char* path,
                                    const std::map<const char*, const char*, WebConfig::cmp_str>& types);

private:
    bool listening = false;
    const std::map<const char*, const char*, WebConfig::cmp_str>& types;
    std::string defaultFile = "index.html";
    std::string webDir;
    std::shared_ptr<tcp::endpoint> endpoint;
    std::shared_ptr<tcp::acceptor> acceptor;
    std::shared_ptr<tcp::socket> socket;
    boost::asio::io_service ioService;
};