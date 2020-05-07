#include "WebServer.h"

WebServer::WebServer(const std::map<const char*, const char*, WebConfig::cmp_str>& types)
    : types(types)
{}

bool WebServer::start(const std::string& dir, unsigned short port)
{
    this->listening = true;
    this->webDir = dir;

    try
    {
        ioService.run();
        this->endpoint = std::shared_ptr<tcp::endpoint>(new tcp::endpoint(tcp::v4(), port));
        this->acceptor = std::shared_ptr<tcp::acceptor>(new tcp::acceptor(ioService));
        this->acceptor->open(this->endpoint->protocol());
        this->acceptor->set_option(tcp::acceptor::reuse_address(true));
        this->acceptor->bind(*endpoint);
        this->acceptor->listen();
    }
    catch(...)
    {
        std::cout << "Cannot open Connection" << std::endl;
        return false;
    }
    
    std::cout << "Accepting Connections now" << std::endl;

    return true;
}

void WebServer::serveAccept()
{
    this->socket = std::shared_ptr<tcp::socket>(new tcp::socket(ioService));
    boost::system::error_code code;
    acceptor->accept(*this->socket, code);
    std::thread thr(WebServer::serve, socket, code, std::ref(this->webDir), std::ref(this->types), std::ref(this->defaultFile));
    thr.detach();
    //acceptor->async_accept(*socket, boost::bind(&WebServer::serve, this, socket, boost::asio::placeholders::error));
}

void WebServer::serve(std::shared_ptr<tcp::socket> socket, const boost::system::error_code code, std::string& webDir,
    const std::map<const char*, const char*, WebConfig::cmp_str>& types, std::string& defaultFile)
{
    if(code)
    {
        std::cout << "Error thrown by opening connection " << code.message() << std::endl;
        return;
    }

    char buf[BUFLEN+1];
    
    // Loop 

    while(1)
    {
        memset(buf, 0, 4001 * sizeof(char));
        std::string netData;
        boost::system::error_code err;
        socket->read_some(boost::asio::buffer(buf, 4000), err);
        if(err)
        {
            socket->shutdown(boost::asio::socket_base::shutdown_both);
            return;
        }

        if(code)
        {
            std::cout << "Error while reading net data" << std::endl;
            return;
        }

        HTTPResolver::HTTPRequestData data;
        bool readSuccess = HTTPResolver::httpd(data, buf);
        HTTPResolver::HTTPResponse response;

        if(readSuccess)
        {
            FileReader fr;
            std::string realPath = data.head.path.raw;
            if(data.head.path.slash.bStart) {
                realPath.erase(realPath.begin());
            }
            std::string fullPath = webDir + "/" + realPath;
            struct stat buffer;
            int found = stat(fullPath.c_str(), &buffer);

            if(S_ISREG(buffer.st_mode) && found == 0)
            {
                // Read File from fullPath
                createFileResponse(response, data, fullPath.c_str(), types);
            }
            else
            {
                if(!data.head.path.slash.bEnd) {
                    fullPath.append("/");
                }
                fullPath.append(defaultFile); // Root File Name
                found = stat(fullPath.c_str(), &buffer);
                if(S_ISREG(buffer.st_mode) && found == 0)
                {
                    createFileResponse(response, data, fullPath.c_str(), types);
                }
                else
                {
                    // 404 not found
                    HTTPResolver::dhttph(data, response, 404, "NOT FOUND");
                    HTTPResolver::dhttpd(response, "NOTHING FOUND", 13);
                }
            }
        }
        else
        {
            HTTPResolver::dhttph(data, response, 503, "SERVER ERROR");
            HTTPResolver::dhttpd(response, "HTTP PROTOCOL INVALID", 22);
        }

        socket->send(boost::asio::buffer(response.document.str()));

        // Get if connection should keep alive
        std::map<HTTPResolver::HTTPArgument, HeaderValue>::iterator it = data.arguments.find(HTTPResolver::HTTPArgument::CONNECTION);
        if(it != data.arguments.end())
        {
            HTTPResolver::HTTPValue::Connection connectionStatus = *reinterpret_cast<HTTPResolver::HTTPValue::Connection*>(it->second.get(0));
            if(connectionStatus != HTTPResolver::HTTPValue::Connection::KEEP_ALIVE)
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    socket->shutdown(boost::asio::socket_base::shutdown_both);
}

void WebServer::createFileResponse(HTTPResolver::HTTPResponse& response, const HTTPResolver::HTTPRequestData& data, const char* path,
                                    const std::map<const char*, const char*, WebConfig::cmp_str>& types)
{
    std::stringstream ss;
    HTTPResolver::dhttph(data, response, 200, "OK");
    auto itr = types.find(data.head.path.file.ext.c_str());
    if(itr != types.end())
    {
        HTTPResolver::dhttpa(response, "Content-Type", itr->second);
    }
    FileReader::append(path, ss);
    std::string content = ss.str();
    HTTPResolver::dhttpd(response, content.c_str(), content.size());
}

void WebServer::setDir(const std::string& path)
{
    this->webDir = path;
}

void WebServer::setDefaultFile(std::string& name)
{
    this->defaultFile = name;
}