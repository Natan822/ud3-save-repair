#include <algorithm>
#include <array>
#include <fstream>
#include <filesystem>
#include <SHA256.h>
#include <iostream>

#include "Utils.h"
#include "SaveFile.h"

std::array<uint8_t, 0x10> SaveFile::HASH_SALT = {0x1d, 0xdb, 0x0a, 0xa9, 0xe9, 0xba, 0x59, 0x4a, 0x5e, 0xb7, 0xf8, 0x6e, 0x82, 0x7a, 0x15, 0x20};

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

    if (!file.good())
    {
        m_valid = false;
        return;
    }

    long size = std::filesystem::file_size(m_path);
    m_buffer.resize(size);

    
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
    if (!m_valid || m_buffer.size() == 0)
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
    if (!m_valid || m_buffer.size() == 0)
    {
        return {};
    }

    std::array<std::byte, SAVE_DATA_SIZE> saveData;
    
    const int FIRST_SECTION_SIZE = SAVE_DATA_SIZE - 0x408;
    
    // Save data before checksum
    auto start = m_buffer.begin() + SAVE_DATA_OFFSET;
    std::copy_n(start, FIRST_SECTION_SIZE, saveData.begin());

    // Save data after checksum
    start += FIRST_SECTION_SIZE + 0x40;
    std::copy_n(start, 0x408, saveData.end() - 0x408);

    return saveData;
}

std::array<std::byte, SHA256_SIZE> SaveFile::generateFirstChecksum()
{
    if (!m_valid || m_buffer.size() == 0)
    {
        return {};
    }

    SHA256 sha;
    sha.update(reinterpret_cast<uint8_t*>(getUserId().data()), USER_ID_SIZE);
    sha.update(reinterpret_cast<uint8_t*>(getSaveData().data()), SAVE_DATA_SIZE);

    std::array<std::byte, SHA256_SIZE> digest = toByteArray(sha.digest());

    SHA256 finalSha;
    finalSha.update(reinterpret_cast<uint8_t*>(digest.data()), SHA256_SIZE);
    finalSha.update(reinterpret_cast<uint8_t*>(HASH_SALT.data()), HASH_SALT.size());
    
    return toByteArray(finalSha.digest());
}

std::array<std::byte, SHA256_SIZE> SaveFile::toByteArray(std::array<uint8_t, SHA256_SIZE> src)
{
    std::array<std::byte, SHA256_SIZE> result;
    for (int i = 0; i < SHA256_SIZE; i++)
    {
        result[i] = std::byte(src[i]);
    }
    return result;
}

void SaveFile::test()
{
    std::array<std::byte, SHA256_SIZE> out = generateFirstChecksum();
    for (int i = 0; i < out.size(); i++)
    {
        std::cout << std::hex << (int)out[i] << " ";
    }
    
}