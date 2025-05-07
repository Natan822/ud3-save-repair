#include "Utils.h"
#include <vector>

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

    std::vector<std::string> splitString(std::string _string, std::string delimiter)
    {
        std::vector<std::string> result;

        int start = 0;
        int index = _string.find(delimiter, start);
        while (index != std::string::npos)
        {
            result.push_back(_string.substr(start, index - start));

            start = index + delimiter.size();
            index = _string.find(delimiter, start);
        }
        result.push_back(_string.substr(start, _string.size() - start));

        return result;
    }

    void cleanPath(std::string &path)
    {
        Utils::trim(path);
        Utils::replaceAll(path, "\\", "/");
        Utils::replaceAll(path, "\"", "");
    }
}