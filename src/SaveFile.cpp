#include <algorithm>
#include <array>
#include <assert.h>
#include <fstream>
#include <filesystem>
#include <SHA256.h>
#include <iostream>
#include <loguru.hpp>

#include "Utils.h"
#include "SaveFile.h"

std::array<uint8_t, 0x10> SaveFile::HASH_SALT = {0x1d, 0xdb, 0x0a, 0xa9, 0xe9, 0xba, 0x59, 0x4a, 0x5e, 0xb7, 0xf8, 0x6e, 0x82, 0x7a, 0x15, 0x20};

SaveFile::SaveFile(std::string path)
    : m_valid(false)
{
    if (path.empty())
    {
        LOG_F(ERROR, "Failed to create file object: Path is empty: %s", path.c_str());

        m_valid = false;
        return;
    }

    cleanPath(path);
    m_path = path;

    m_valid = true;
    load();
}

SaveFile::~SaveFile(){}

bool SaveFile::isValid()
{
    return m_valid;
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
        LOG_F(ERROR, "Failed to retrieve user ID: Invalid file");
        return {};
    }
    

    if (m_buffer.size() == 0)
    {
        LOG_F(ERROR, "Failed to retrieve user ID: File buffer is empty");
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
        LOG_F(ERROR, "Failed to retrieve save data: Invalid file");
        return {};
    }
    

    if (m_buffer.size() == 0)
    {
        LOG_F(ERROR, "Failed to retrieve save data: File buffer is empty");
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

std::array<std::byte, SHA256_SIZE> SaveFile::generateFirstSha()
{
    if (!m_valid)
    {
        LOG_F(ERROR, "Failed to generate first checksum: Invalid file");
        return {};
    }
    

    if (m_buffer.size() == 0)
    {
        LOG_F(ERROR, "Failed to generate first checksum: File buffer is empty");
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

std::array<std::byte, SHA256_SIZE> SaveFile::generateSecondSha()
{
    if (!m_valid)
    {
        LOG_F(ERROR, "Failed to generate first checksum: Invalid file");
        return {};
    }
    

    if (m_buffer.size() == 0)
    {
        LOG_F(ERROR, "Failed to generate first checksum: File buffer is empty");
        return {};
    }

    const int INPUT_SIZE = 0x40;

    std::array<std::byte, INPUT_SIZE> input;
    std::copy_n(m_buffer.begin(), INPUT_SIZE, input.begin());

    std::array<std::byte, SAVE_DATA_SIZE> saveData = getSaveData();

    SHA256 sha;
    sha.update(reinterpret_cast<uint8_t*>(input.data()), INPUT_SIZE);
    sha.update(reinterpret_cast<uint8_t*>(saveData.data()), SAVE_DATA_SIZE);

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

void SaveFile::fixChecksums()
{
    std::array<std::byte, SHA256_SIZE> firstSha = generateFirstSha();
    fixFirstChecksum(firstSha);

    std::array<std::byte, SHA256_SIZE> secondSha = generateSecondSha();
    fixSecondChecksum(secondSha);
    fixThirdChecksum(secondSha);
}

void SaveFile::fixFirstChecksum(std::array<std::byte, SHA256_SIZE> sha)
{
    std::copy(sha.begin(), sha.end(), m_buffer.begin());
}

void SaveFile::fixSecondChecksum(std::array<std::byte, SHA256_SIZE> sha)
{
    int shaIndex = 0;
    for (int row = 0; row < 4; row++)
    {
        for (int index : INDEX_MAPPING)
        {
            m_buffer.at(SECOND_CHECKSUM_OFFSET + index + (0x10 * row)) = sha.at(shaIndex);
            shaIndex++;
        }
    }

    assert(shaIndex == 0x10);
}

void SaveFile::fixThirdChecksum(std::array<std::byte, SHA256_SIZE> sha)
{
    int shaIndex = 0x10;
    for (int row = 0; row < 4; row++)
    {
        for (int index : INDEX_MAPPING)
        {
            m_buffer.at(THIRD_CHECKSUM_OFFSET + index + (0x10 * row)) = sha.at(shaIndex);
            shaIndex++;
        }
    }

    assert(shaIndex == 0x20);
}

void SaveFile::save(std::string path)
{
    std::ofstream out;

    out.open(path, std::ios::binary);
    out.write(reinterpret_cast<char *>(m_buffer.data()), m_buffer.size());

    out.close();
    LOG_F(INFO, "File saved at: %s", path.c_str());
}

void SaveFile::test()
{
    std::array<std::byte, SHA256_SIZE> out = generateFirstSha();
    for (int i = 0; i < out.size(); i++)
    {
        std::cout << std::hex << (int)out[i] << " ";
    }
    
}