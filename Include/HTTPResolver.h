#pragma once
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <ZMap.h>
#include <HeaderValue.h>

class HTTPResolver
{
public:
    enum class Method
    {
        GET, POST, PUT
    };

    enum class ProtoVersion
    {
        HTTP10, HTTP11
    };

    enum class HTTPArgument
    {
        USER_AGENT, DNT, REFERER, FROM, ACCEPT, KEEP_ALIVE, ACCEPT_ENCODING, ACCEPT_LANGUAGE,
        CONNECTION, UPGRADE_INSECURE_REQUESTS, HOST, UNDETECTED
    };

    struct HTTPRequestHead
    {
        ProtoVersion version;
        Method method;
        struct Path
        {
            std::string raw;
            bool hasVariables;
            struct File
            {
                std::string ext;
                std::string name;
                bool valid;
            } file;
            struct PathSlash
            {
                bool bStart;
                bool bEnd;
            } slash;
        } path;
    };

    struct HTTPValue
    {
        enum class Connection
        {
            KEEP_ALIVE, CLOSE
        };
        enum class DNT
        {
            TRACKING_ALLOWED, TRACKING_PROHIBITED
        };
        enum class UpdateInsecureRequests
        {
            YES, NO
        };
        enum class KeepAlive
        {
            TIMEOUT, MAX
        };
    };

    struct HTTPRequestData
    {
        HTTPRequestHead head;
        std::map<HTTPArgument, HeaderValue> arguments;
    };

    struct HTTPResponse
    {
        std::stringstream document;
    };

    HTTPResolver() = default;

    static bool dhttph(const HTTPRequestData& data, HTTPResponse& http, unsigned short statusCode, const char* statusMessage); // document to http header
    static bool dhttpa(HTTPResponse& http, const char* title, const char* value); // data to http add argument
    static bool dhttpd(HTTPResponse& http, const char* data, size_t length);

    static bool httpd(HTTPRequestData& httpData, const std::string& http);
    static bool rhead(HTTPRequestData& httpData, std::string head);
    static bool cpath(HTTPRequestData& data);
    static bool rargument(HTTPRequestData& data, const std::string& argument);

    using ValueReader = bool (*)(HeaderValue& value, const std::string& strValue);

    // r---a = read name argument

    static bool rconnectiona(HeaderValue& value, const std::string& strValue);
    static bool rdnta(HeaderValue& value, const std::string& strValue);
    static bool rhosta(HeaderValue& value, const std::string& strValue);
    static bool rupdateinsecurerequestsa(HeaderValue& value, const std::string& strValue);
    static bool rkeepalivea(HeaderValue& value, const std::string& strValue);
};