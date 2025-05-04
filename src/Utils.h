#pragma once

#include <string>

namespace Utils
{
    // Replace all ocurrences of "from" with "to" in a given string
    void replaceAll(std::string &_string, std::string from, std::string to);

    // Trim whitespaces
    void ltrim(std::string &_string); // Left
    void rtrim(std::string &_string); // RIght
    void trim(std::string &_string);  // Left and Right
}