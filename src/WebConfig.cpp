#include <WebConfig.h>

WebConfig::WebConfig(const char* path)
{
    struct stat buffer;
    int exists = stat(path, &buffer);
    if(exists == 0 && S_ISREG(buffer.st_mode))
    {
        std::ifstream file;
        file.open(path, std::ifstream::ate | std::ifstream::binary);
        size_t size = file.tellg();
        file.seekg(std::ios::beg);
        char* space = reinterpret_cast<char*>(malloc(size));
        file.read(space, size);
        file.close();
        document.Parse(space, size);
        free(space);
        if(document.HasParseError())
        {
            this->status = ConfigStatus::FAILED_PARSING;
        }
        else
        {
            if(this->getVars())
            {
                this->status = ConfigStatus::SUCCESS;
            }
            else
            {
                this->status = ConfigStatus::FAILED_PREPARING;
            }
            
        }
    }
    else
    {
        this->status = ConfigStatus::FAILED_READING;
    }
}

bool WebConfig::getVars()
{
    bool serveProp = false;
    bool fileProp = false;
    bool mimeProp = false;
    if(document.HasMember("General"))
    {
        serveProp = this->getServeProp("General");
        fileProp = this->getFileProp("General");
    }
    mimeProp = this->getMimeProp("Types");

    return serveProp && fileProp && mimeProp;
}

bool WebConfig::getServeProp(const char* parentName)
{
    if(document[parentName].HasMember("Serve"))
    {
        rapidjson::Value& serve = document[parentName]["Serve"];
        if(serve.HasMember("Domain") && serve["Domain"].IsString())
        {
            this->config.domain = serve["Domain"].GetString();
        }
        else
        {
            return false;
        }
        
        if(serve.HasMember("Port") && serve["Port"].IsInt())
        {
            rapidjson::Value& val = serve["Port"];
            int port = val.GetInt();
            if(port <= 60000 && port > 0)
            {
                this->config.port = port;
            }
            else
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

bool WebConfig::getFileProp(const char* parentName)
{
    if(document[parentName].HasMember("File"))
    {
        rapidjson::Value& file = document[parentName]["File"];
        if(file.HasMember("Directory") && file["Directory"].IsString())
        {
            this->config.directory_path = file["Directory"].GetString();
            return true;
        }
        else
        {
            return false;
        }
        if(file.HasMember("Default") && file["Default"].IsString())
        {
            this->config.default_file = file["Default"].GetString();
        }
    }
}

bool WebConfig::getMimeProp(const char* propName)
{
    if(document.HasMember(propName) && document[propName].IsObject())
    {
        rapidjson::Value& types = document[propName];
        
        for(rapidjson::Value::ConstMemberIterator itr = types.MemberBegin();
                itr != types.MemberEnd(); ++itr)
        {
            if(itr->value.IsString())
            {
                this->config.types[itr->name.GetString()] = itr->value.GetString();
            }
        }
        return true;
    }
    return false;
}

const WebConfig::Config& WebConfig::getConfig() const
{
    return this->config;
}

WebConfig::ConfigStatus WebConfig::getStatus() const
{
    return this->status;
}