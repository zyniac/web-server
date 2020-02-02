#pragma once
#include <string>
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

    struct HTTPHead
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

    struct HTTPData
    {
        HTTPHead head;
        std::map<HTTPArgument, HeaderValue> arguments;
    };

    HTTPResolver() = default;

    static bool httpd(HTTPData& httpData, const std::string& http);
    static bool rhead(HTTPData& httpData, std::string head);
    static bool cpath(HTTPData& data);
    static bool rargument(HTTPData& data, const std::string& argument);

    using ValueReader = bool (*)(HeaderValue& value, const std::string& strValue);

    // r---a = read name argument

    static bool rconnectiona(HeaderValue& value, const std::string& strValue);
    static bool rdnta(HeaderValue& value, const std::string& strValue);
    static bool rhosta(HeaderValue& value, const std::string& strValue);
    static bool rupdateinsecurerequestsa(HeaderValue& value, const std::string& strValue);
    static bool rkeepalivea(HeaderValue& value, const std::string& strValue);
};