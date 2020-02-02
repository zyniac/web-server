#pragma once
#include <vector>
#include <tuple>

class HeaderValue
{
public:
    HeaderValue() = default;
    HeaderValue(const HeaderValue&) = delete;

    HeaderValue(HeaderValue&& headerValue)
        : values(std::move(headerValue.values))
    {}

    ~HeaderValue()
    {
        for(void* pair : values)
        {
            free(pair);
        }
    }

    template<class A>
    void put(A&& val)
    {
        A* value = reinterpret_cast<A*>(malloc(sizeof(val)));
        *value = val;
        values.emplace_back(value);
    }

    template<class A>
    void put(const A& val)
    {
        A* value = reinterpret_cast<A*>(malloc(sizeof(val)));
        *value = val;
        values.emplace_back(value);
    }

    size_t put_string(const char* string, size_t size=~0)
    {
        if(size == ~0)
        {
            size = strlen(string);
        }
        char* value = reinterpret_cast<char*>(malloc(size + 1));
        strcpy(value, string);
        values.emplace_back(value);
        return size;
    }

    void * get(size_t i) const
    {
        return values[i];
    }

    size_t size() const
    {
        return values.size();
    }

private:
    std::vector<void*> values;
};