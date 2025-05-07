#include <algorithm>
#include <array>
#include <assert.h>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <SHA256.h>
#include <iostream>
#include <loguru.hpp>

#include "Utils.h"
#include "SaveFile.h"

std::array<uint8_t, 0x10> SaveFile::SYSTEM_HASH_SALT = {0x1d, 0xdb, 0x0a, 0xa9, 0xe9, 0xba, 0x59, 0x4a, 0x5e, 0xb7, 0xf8, 0x6e, 0x82, 0x7a, 0x15, 0x20};
std::array<uint8_t, 0x10> SaveFile::GENERIC_HASH_SALT = {0x5d, 0x33, 0x40, 0x56, 0x43, 0xe0, 0x3a, 0xe7, 0x1a, 0x5d, 0x8b, 0xe9, 0x91, 0x27, 0x40, 0xad};

SaveFile::SaveFile(std::string path)
    : m_valid(false)
{
    if (path.empty())
    {
        LOG_F(ERROR, "Failed to create file object: Path is empty: %s", path.c_str());

        m_valid = false;
        return;
    }

    Utils::cleanPath(path);
    m_path = path;
    m_name = Utils::splitString(path, "/").back();

    load();
}

SaveFile::~SaveFile(){}

bool SaveFile::isValid()
{
    return m_valid;
}

std::string SaveFile::getName()
{
    return m_name;
}

void SaveFile::load()
{
    std::ifstream file(m_path, std::ios::binary);

    if (!file.good())
    {
        LOG_F(ERROR, "Failed to load file at: %s", m_path.c_str());

        m_valid = false;
        return;
    }

    long size = std::filesystem::file_size(m_path);
    m_buffer.resize(size);

    
    file.read(reinterpret_cast<char*>(m_buffer.data()), size);
    file.close();
    m_valid = true;
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

void SaveFile::save(std::string path)
{
    std::ofstream out;

    out.open(path, std::ios::binary);
    out.write(reinterpret_cast<char *>(m_buffer.data()), m_buffer.size());

    out.close();
    LOG_F(INFO, "File saved at: %s", path.c_str());
}
