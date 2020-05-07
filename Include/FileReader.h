#pragma once
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <sstream>

class FileReader
{
public:
    FileReader() = default;
    static void append(const char* filename, std::stringstream& ss);
    static bool exists(std::string filename);
    
private:
    // boost::iostreams::mapped_file file;
};