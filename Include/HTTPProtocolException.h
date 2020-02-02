#include <iostream>
#include <exception>
#include <stdexcept>
#include <sstream>

class HTTPProtocolException : public std::runtime_error
{
    HTTPProtocolException()
        : runtime_error("Protocol has false format")
    {}

    virtual const char* what() const throw()
    {
        return "Protocol wasn't valid";
    }
};