#include <algorithm>
#include <array>
#include <fstream>
#include <filesystem>

#include "Utils.h"
#include "SaveFile.h"

SaveFile::SaveFile(std::string path)
    : m_valid(false)
{
    if (path.empty())
    {
        m_valid = false;
        return;
    }

    cleanPath(path);
    m_path = path;

    m_valid = true;
    load();
}

SaveFile::~SaveFile(){}

void SaveFile::load()
{
    std::ifstream file(m_path, std::ios::binary);

    long size = std::filesystem::file_size(m_path);
    m_buffer.resize(size);

    if (!file.good())
    {
        m_valid = false;
        return;
    }
    
    file.read(reinterpret_cast<char*>(m_buffer.data()), size);
    file.close();
}

void SaveFile::cleanPath(std::string& path)
{
    Utils::trim(path);
    Utils::replaceAll(path, "\\", "/");
    Utils::replaceAll(path, "\"", "");
}

std::array<std::byte, USER_ID_SIZE> SaveFile::getUserId()
{
    if (!m_valid)
    {
        return {};
    }
    
    if (m_buffer.size() == 0)
    {
        return {};
    }

    std::array<std::byte, USER_ID_SIZE> userId;

    auto start = m_buffer.begin() + USER_ID_OFFSET;
    std::copy_n(start, USER_ID_SIZE, userId.begin());
    
    return userId;
}

std::array<std::byte, SAVE_DATA_SIZE> SaveFile::getSaveData()
{
    if (!m_valid)
    {
        return {};
    }
    
    if (m_buffer.size() == 0)
    {
        return {};
    }

    std::array<std::byte, SAVE_DATA_SIZE> saveData;
    
    const int FIRST_SECTION_SIZE = SAVE_DATA_SIZE - 0x428;
    
    // Save data before checksum
    auto start = m_buffer.begin() + SAVE_DATA_OFFSET;
    std::copy_n(start, FIRST_SECTION_SIZE, saveData.begin());

    // Save data after checksum
    start += FIRST_SECTION_SIZE + 0x20;
    std::copy_n(start, 0x408, saveData.end() - 0x408);
}