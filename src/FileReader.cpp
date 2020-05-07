#include "FileReader.h"
#include <iostream>

void FileReader::append(const char* filename, std::stringstream& ss)
{
    boost::iostreams::mapped_file file;
    file.open(filename, boost::iostreams::mapped_file::readonly);
    ss.write(file.const_data(), file.size());
    file.close();
}

bool FileReader::exists(std::string filename) {
    return boost::filesystem::exists(boost::filesystem::path(filename.c_str()));
}