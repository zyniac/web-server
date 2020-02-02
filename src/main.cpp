#include <iostream>
#include <chrono>
#include <ZMap.h>
#include <HTTPResolver.h>
#include <HeaderValue.h>

void coutKeepAlive(HTTPResolver::HTTPValue::KeepAlive title, int value)
{
    switch(title)
    {
        case HTTPResolver::HTTPValue::KeepAlive::MAX:
            std::cout << "Keep Alive max value is: " << value << std::endl;
        break;

        case HTTPResolver::HTTPValue::KeepAlive::TIMEOUT:
            std::cout << "Keep Alive timeout value is: " << value << std::endl;
        break;
    }
}

int main(int argc, char** argv)
{
    std::string line("GET /http/index.html?test=1 HTTP/1.1\r\nConnection: keep-alive\r\nDNT: 1\r\nHost: zyniac.com\r\nUpgrade-Insecure-Requests: 1\r\nKeep-Alive: max=3000, timeout=7");

    HTTPResolver::HTTPData data;
    std::chrono::time_point before = std::chrono::high_resolution_clock::now();
    bool success = HTTPResolver::httpd(data, line);
    std::chrono::time_point after = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds time = std::chrono::duration_cast<std::chrono::microseconds>(after - before);
    std::cout << "Time needed: " << time.count() << " microseconds" << std::endl;

    if(success)
    {
        std::cout << "Success" << std::endl;
        if(data.head.path.file.valid)
        {
            std::cout << "File name: " << data.head.path.file.name << std::endl;
            std::cout << "File extension: " << data.head.path.file.ext << std::endl;
        }
        if(data.arguments.size() > 0)
        {
            auto connectionTitleItr = data.arguments.find(HTTPResolver::HTTPArgument::CONNECTION);
            if(connectionTitleItr != data.arguments.end())
            {
                HTTPResolver::HTTPValue::Connection connection = *reinterpret_cast<HTTPResolver::HTTPValue::Connection*>(connectionTitleItr->second.get(0));
                switch(connection)
                {
                    case HTTPResolver::HTTPValue::Connection::KEEP_ALIVE:
                        std::cout << "Connection is set to keep alive" << std::endl;
                    break;

                    case HTTPResolver::HTTPValue::Connection::CLOSE:
                        std::cout << "Connection is set to close" << std::endl;
                    break;
                }
            }
            else
            {
                std::cout << "No Connection argument given" << std::endl;
            }
            

            auto dntTitleItr = data.arguments.find(HTTPResolver::HTTPArgument::DNT);
            if(dntTitleItr != data.arguments.end())
            {
                HTTPResolver::HTTPValue::DNT dnt = *reinterpret_cast<HTTPResolver::HTTPValue::DNT*>(dntTitleItr->second.get(0));
                switch(dnt)
                {
                    case HTTPResolver::HTTPValue::DNT::TRACKING_ALLOWED:
                        std::cout << "Tracking is allowed." << std::endl;
                    break;

                    case HTTPResolver::HTTPValue::DNT::TRACKING_PROHIBITED:
                        std::cout << "Tracking is forbidden." << std::endl;
                    break;
                }
            }
            else
            {
                std::cout << "No DNT argument given" << std::endl;
            }

            auto hostTitleItr = data.arguments.find(HTTPResolver::HTTPArgument::HOST);
            if(hostTitleItr != data.arguments.end())
            {
                char* cSource = reinterpret_cast<char*>(hostTitleItr->second.get(0));
                size_t cLength = *reinterpret_cast<size_t*>(hostTitleItr->second.get(1));
                std::string hostValue(cSource, cLength);
                std::cout << "The Host: " << hostValue << std::endl;
            }
            else
            {
                std::cout << "No Host found." << std::endl;
            }

            auto uirTitleItr = data.arguments.find(HTTPResolver::HTTPArgument::UPGRADE_INSECURE_REQUESTS);
            if(uirTitleItr != data.arguments.end())
            {
                HTTPResolver::HTTPValue::UpdateInsecureRequests uir = *reinterpret_cast<HTTPResolver::HTTPValue::UpdateInsecureRequests*>(uirTitleItr->second.get(0));
                switch(uir)
                {
                    case HTTPResolver::HTTPValue::UpdateInsecureRequests::YES:
                        std::cout << "Requests should be upgraded to 'secure'" << std::endl;
                    break;

                    case HTTPResolver::HTTPValue::UpdateInsecureRequests::NO:
                        std::cout << "Requests are secure or should not be upgraded" << std::endl;
                    break;
                }
            }
            else
            {
                std::cout << "No Upgrade Insecure Requests header found." << std::endl;
            }

            auto keepAliveTitleItr = data.arguments.find(HTTPResolver::HTTPArgument::KEEP_ALIVE);

            if(keepAliveTitleItr != data.arguments.end())
            {
                HTTPResolver::HTTPValue::KeepAlive title1 = *reinterpret_cast<HTTPResolver::HTTPValue::KeepAlive*>(keepAliveTitleItr->second.get(0));
                int titleValue1 = *reinterpret_cast<int*>(keepAliveTitleItr->second.get(1));
                coutKeepAlive(title1, titleValue1);
                if(keepAliveTitleItr->second.size() == 4)
                {
                    HTTPResolver::HTTPValue::KeepAlive title2 = *reinterpret_cast<HTTPResolver::HTTPValue::KeepAlive*>(keepAliveTitleItr->second.get(2));
                    int titleValue2 = *reinterpret_cast<int*>(keepAliveTitleItr->second.get(3));
                    coutKeepAlive(title2, titleValue2);
                }
            }
            else
            {
                std::cout << "Keep Alive not found" << std::endl;
            }
            

            std::cout << "Process exit" << std::endl;
        }
        else
        {
            std::cout << "Argument not placed" << std::endl;
        }
        
    }
    else
    {
        std::cout << "Failed" << std::endl;
    }
    
}