#include <iostream>
#include "WebServer.h"
#include <chrono>
#include "WebConfig.h"

int main(int argc, char** argv)
{
#if defined util_INCLUDED
    std::cout << "Starting..." << std::endl;
    WebConfig wc("web_config.json");
    if (wc.getStatus() == WebConfig::ConfigStatus::SUCCESS)
    {
        const WebConfig::Config& config = wc.getConfig();
        WebServer ws(config.types);
        ws.start(config.directory_path, config.port);

        while (true)
        {
            ws.serveAccept();
        }
    }
    else
    {
        std::cout << "Config has errors. Please check: " << static_cast<int>(wc.getStatus()) << std::endl;
    }
#else
    std::cout << "Package 'util' is required to run package 'webserver' correctly" << std::endl;
    std::cin.get();
#endif
}