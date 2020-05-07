#include "HTTPResolver.h"
#include "HTTPProtocolException.h"

constexpr Zyniac::MapArray<HTTPResolver::Method, 3> MethodTypes = {{"GET", HTTPResolver::Method::GET}, {"POST", HTTPResolver::Method::POST}, {"PUT", HTTPResolver::Method::PUT}};
constexpr Zyniac::MapArray<HTTPResolver::ProtoVersion, 2> ProtocolVersionTypes = {{"HTTP/1.0", HTTPResolver::ProtoVersion::HTTP10}, {"HTTP/1.1", HTTPResolver::ProtoVersion::HTTP11}};
constexpr Zyniac::MapArray<HTTPResolver::HTTPArgument, 6> ArgumentTypes = {{"Accept", HTTPResolver::HTTPArgument::ACCEPT}, {"Keep-Alive", HTTPResolver::HTTPArgument::KEEP_ALIVE},
                                                                                    {"DNT", HTTPResolver::HTTPArgument::DNT}, {"Upgrade-Insecure-Requests", HTTPResolver::HTTPArgument::UPGRADE_INSECURE_REQUESTS},
                                                                                    {"Connection", HTTPResolver::HTTPArgument::CONNECTION}, {"Host", HTTPResolver::HTTPArgument::HOST}};

bool HTTPResolver::rhead(HTTPRequestData& httpData, std::string head)
{
    // METHOD
    size_t gCut = head.find(' ');
    if(gCut != std::string::npos)
    {
        std::string strType = head.substr(0, gCut);
        head.erase(0, gCut + 1);
        Zyniac::MapReturn<HTTPResolver::Method> type = Zyniac::map_get(MethodTypes, strType.c_str());

        if(type != nullptr)
        {
            httpData.head.method = *type;

            size_t pCut = head.find(' ');
            httpData.head.path.raw = head.substr(0, pCut); // TODO Check if path has valid syntax
            head.erase(0, pCut + 1);

            if(cpath(httpData))
            {
                Zyniac::MapReturn<HTTPResolver::ProtoVersion> version = Zyniac::map_get(ProtocolVersionTypes, head.c_str()); // Rest of head must be the version

                if(version != nullptr)
                {
                    httpData.head.version = *version;
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
            
        }
        else
        {
            return false;
        }
        
    }
    else
    {
        return false;
    }
    
}

bool HTTPResolver::cpath(HTTPRequestData& data)
{
    size_t pos = data.head.path.raw.find('.');
    size_t getPos = data.head.path.raw.find('?', pos);

    if(getPos == std::string::npos)
    {
        getPos = data.head.path.raw.size();
        data.head.path.hasVariables = false;
    }
    else
    {
        data.head.path.hasVariables = true;
    }
    

    size_t pointBefore = pos;
    size_t pointAfter = 0;

    if(*data.head.path.raw.begin() == '/') {
        data.head.path.slash.bStart = true;
    }
    if(*(data.head.path.raw.end()-1) == '/') {
        data.head.path.slash.bEnd = true;
    }

    if(pos != std::string::npos && pos < getPos) // Dot is in url
    {
        data.head.path.file.valid = true;
        while((pointAfter = data.head.path.raw.find('.', pointBefore + 1)) < getPos) // Some Error to find
        {
            if(pointAfter - 1 == pointBefore)
            {
                return false;
            }
            
            pointBefore = pointAfter;
        }

        data.head.path.file.ext.assign(data.head.path.raw.begin() + pos + 1, data.head.path.raw.begin() + getPos);
        size_t nameStart = data.head.path.raw.find_last_of('/', pos);
        if(nameStart == std::string::npos)
        {
            nameStart = 0;
        }
        data.head.path.file.name.assign(data.head.path.raw.begin() + nameStart + 1, data.head.path.raw.begin() + pos);
    }
    else
    {
        data.head.path.file.valid = false;
    }
    return true;
}

bool HTTPResolver::rargument(HTTPRequestData& data, const std::string& argument)
{
    size_t aCut = argument.find(": ");
    if(aCut > 0 && argument.size() > aCut)
    {
        const std::string title(argument.begin(), argument.begin() + aCut);
        const std::string strValue(argument.begin() + aCut + 2, argument.end());
        Zyniac::MapReturn<HTTPResolver::HTTPArgument> arg = Zyniac::map_get(ArgumentTypes, title.c_str());
        HTTPResolver::HTTPArgument indexArgument;
        if(arg == nullptr)
        {
            // Algorithm does not know Argument Type
            return true;
        }
        else
        {
            indexArgument = *arg;
        }

        HeaderValue value;
        bool rt = true;
        switch(indexArgument) // Could be mapped in the future
        {
            case HTTPResolver::HTTPArgument::CONNECTION:
                rt = rconnectiona(value, strValue);
            break;

            case HTTPResolver::HTTPArgument::DNT:
                rt = rdnta(value, strValue);
            break;

            case HTTPResolver::HTTPArgument::HOST:
                rt = rhosta(value, strValue);
            break;

            case HTTPResolver::HTTPArgument::UPGRADE_INSECURE_REQUESTS:
                rt = rupdateinsecurerequestsa(value, strValue);
            break;

            case HTTPResolver::HTTPArgument::KEEP_ALIVE:
                rt = rkeepalivea(value, strValue);
            break;
        }

        data.arguments.emplace(indexArgument, std::move(value));
        
        return rt;
    }
    else
    {
        return false;
    }
}

bool HTTPResolver::httpd(HTTPRequestData& httpData, const std::string& http)
{
    // TODO SPLIT HEADER FROM DOCUMENT!
    std::vector<std::string> lines;
    const char* delimiter = "\r\n";
    size_t current;
    size_t previous = 0;
    current = http.find(delimiter);
    while(current != std::string::npos)
    {
        lines.emplace_back(http.substr(previous, current - previous));
        previous = current + 2;
        current = http.find(delimiter, previous);
    }
    lines.emplace_back(http.substr(previous, current - previous));

    if(lines.size() >= 1)
    {
        if(HTTPResolver::rhead(httpData, lines[0]))
        {
            // HTTPResolver::rargument() for every next line
            for(int i = 1; i < lines.size(); ++i)
            {
                if(lines[i].size() < 3)
                {
                    return true; // Probably end here (except has no document)
                }
                if(!HTTPResolver::rargument(httpData, lines[i]))
                {
                    return false;
                }
            }
            return true;
        }
        else
        {
            return false;
        }
        
    }
    else
    {
        return false;
    }
    
    return false;
}

bool HTTPResolver::rconnectiona(HeaderValue& value, const std::string& strValue)
{
    static constexpr Zyniac::MapArray<HTTPValue::Connection, 2> ConnectionTypes = {{"keep-alive", HTTPValue::Connection::KEEP_ALIVE}, {"close", HTTPValue::Connection::CLOSE}};
    Zyniac::MapReturn<HTTPValue::Connection> mgValue = Zyniac::map_get(ConnectionTypes, strValue.c_str());
    if(mgValue != nullptr)
    {
        value.put<HTTPValue::Connection>(*mgValue);
        return true;
    }
    else
    {
        return false;
    }
    
}

bool HTTPResolver::rdnta(HeaderValue& value, const std::string& strValue)
{
    static constexpr Zyniac::MapArray<HTTPValue::DNT, 2> DNTTypes = {{"1", HTTPValue::DNT::TRACKING_PROHIBITED}, {"0", HTTPValue::DNT::TRACKING_ALLOWED}};
    Zyniac::MapReturn<HTTPValue::DNT> mgValue = Zyniac::map_get(DNTTypes, strValue.c_str());
    if(mgValue != nullptr)
    {
        value.put<HTTPValue::DNT>(*mgValue);
        return true;
    }
    else
    {
        return false;
    }
    
}

bool HTTPResolver::rhosta(HeaderValue& value, const std::string& strValue)
{
    value.put_string(strValue.c_str(), strValue.size());
    value.put<size_t>(strValue.size());
    return true;
}

bool HTTPResolver::rupdateinsecurerequestsa(HeaderValue& value, const std::string& strValue)
{
    constexpr Zyniac::MapArray<HTTPResolver::HTTPValue::UpdateInsecureRequests, 2> UIRTypes = {{"1", HTTPResolver::HTTPValue::UpdateInsecureRequests::YES}, {"0", HTTPResolver::HTTPValue::UpdateInsecureRequests::NO}};
    Zyniac::MapReturn<HTTPResolver::HTTPValue::UpdateInsecureRequests> mgValue = Zyniac::map_get(UIRTypes, strValue.c_str());
    if(mgValue != nullptr)
    {
        value.put<HTTPResolver::HTTPValue::UpdateInsecureRequests>(*mgValue);
        return true;
    }
    else
    {
        return false;
    }

}

bool HTTPResolver::rkeepalivea(HeaderValue& value, const std::string& strValue)
{
    constexpr Zyniac::MapArray<HTTPResolver::HTTPValue::KeepAlive, 2> KeepAliveTypes = {{"timeout", HTTPResolver::HTTPValue::KeepAlive::TIMEOUT}, {"max", HTTPResolver::HTTPValue::KeepAlive::MAX}};
    size_t commaCut = strValue.find(", ");
    size_t equalCut = strValue.find("=");
    bool single = false;

    if(commaCut == std::string::npos)
    {
        commaCut = strValue.size();
        single = true;
    }

    if(equalCut == std::string::npos)
    {
        return false;
    }

    if(equalCut < commaCut)
    {
        const std::string title(strValue.begin(), strValue.begin() + equalCut);
        Zyniac::MapReturn<HTTPResolver::HTTPValue::KeepAlive> keepAlive = Zyniac::map_get(KeepAliveTypes, title.c_str());
        if(keepAlive != nullptr)
        {
            const std::string subVal(strValue.begin() + equalCut + 1, strValue.begin() + commaCut);
            int number = atoi(subVal.c_str());
            value.put<HTTPResolver::HTTPValue::KeepAlive>(*keepAlive);
            value.put<int>(number);
        }
        else
        {
            return false;
        }
    }

    if(!single)
    {
        size_t equalCut2 = strValue.find('=', commaCut + 2);
        
        if(equalCut2 == std::string::npos)
        {
            return false;
        }

        const std::string title(strValue.begin() + commaCut + 2, strValue.begin() + equalCut2);
        Zyniac::MapReturn<HTTPResolver::HTTPValue::KeepAlive> keepAlive = Zyniac::map_get(KeepAliveTypes, title.c_str());
        HTTPResolver::HTTPValue::KeepAlive before = *reinterpret_cast<HTTPResolver::HTTPValue::KeepAlive*>(value.get(0));
        if(keepAlive != nullptr && *keepAlive != before)
        {
            const std::string subVal(strValue.begin() + equalCut2 + 1, strValue.end());
            int number = atoi(subVal.c_str());
            value.put<HTTPResolver::HTTPValue::KeepAlive>(*keepAlive);
            value.put<int>(number);
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool HTTPResolver::dhttph(const HTTPRequestData& data, HTTPResponse& http, unsigned short statusCode, const char* statusMessage)
{
    switch(data.head.version)
    {
        case HTTPResolver::ProtoVersion::HTTP11:
            http.document << "HTTP/1.1 ";
        break;

        case HTTPResolver::ProtoVersion::HTTP10:
            http.document << "HTTP/1.0 ";
        break;
    }
    http.document << statusCode << " " << statusMessage << "\r\n";
    return true;

}

bool HTTPResolver::dhttpa(HTTPResponse& http, const char* title, const char* value)
{
    http.document << title << ": " << value << "\r\n";
    return true;

}

bool HTTPResolver::dhttpd(HTTPResponse& http, const char* data, size_t length)
{
    http.document << "Content-Length: " << length << "\r\n\r\n";
    http.document.write(data, length);
    return true;
    
}