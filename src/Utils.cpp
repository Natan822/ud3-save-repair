#include "Utils.h"

namespace Utils
{
    void replaceAll(std::string &_string, std::string from, std::string to)
    {
        if (from.empty())
            return;

        int index = _string.find(from, 0);
        while (index != std::string::npos)
        {
            _string.replace(index, from.size(), to);
            index = _string.find(from, 0);
        }
    }

    void ltrim(std::string &_string)
    {
        int index = _string.find_first_not_of(" \t\n\r\f\v");
        _string = _string.substr(index, _string.size() - index);
    }

    void rtrim(std::string &_string)
    {
        int end = _string.find_last_not_of(" \t\n\r\f\v");
        _string = _string.substr(0, end + 1);
    }

    void trim(std::string &_string)
    {
        ltrim(_string);
        rtrim(_string);
    }
}