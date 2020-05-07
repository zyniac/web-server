#pragma once
#include <string>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <sys/stat.h>
#include <WinLin.h> // S_ISDIR S_ISREG for Windows
#include <fstream>
#include <map>
#include <iostream>

class WebConfig
{
public:
    WebConfig(const char* path);
    WebConfig() = delete;
    struct cmp_str
    {
        bool operator()(const char *a, const char *b) const
        {
            return std::strcmp(a, b) < 0;
        }
    };
    struct Config
    {
        std::string domain;
        size_t port;
        std::string default_file;
        std::string directory_path;
        std::map<const char*, const char*, cmp_str> types;  
    };
    enum class ConfigStatus
    {
        SUCCESS, FAILED_READING, FAILED_PARSING, FAILED_PREPARING
    };
    const WebConfig::Config& getConfig() const;
    ConfigStatus getStatus() const;

private:
    ConfigStatus status;
    Config config;
    rapidjson::Document document;
    bool getVars();
    bool getMimeProp(const char* propName);
    bool getServeProp(const char* parentName);
    bool getFileProp(const char* parentName);
};