#include <algorithm>
#include <array>
#include <assert.h>
#include <cstdint>
#include <loguru.hpp>
#include <SHA256.h>

#include "SystemDat.h"

SystemDat::SystemDat(std::string path): SaveFile(path){}
SystemDat::~SystemDat(){}

std::array<std::byte, SystemDat::USER_ID_SIZE> SystemDat::getUserId()
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

std::vector<std::byte> SystemDat::getSaveData()
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

    std::vector<std::byte> saveData;
    saveData.resize(SystemDat::SAVE_DATA_SIZE);

    const int FIRST_SECTION_SIZE = SystemDat::SAVE_DATA_SIZE - 0x408;
    
    // Save data before checksum
    auto start = m_buffer.begin() + SystemDat::SAVE_DATA_OFFSET;
    std::copy_n(start, FIRST_SECTION_SIZE, saveData.begin());

    // Save data after checksum
    start += FIRST_SECTION_SIZE + 0x40;
    std::copy_n(start, 0x408, saveData.end() - 0x408);

    return saveData;
}

std::array<std::byte, SHA256_SIZE> SystemDat::generateFirstSha()
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
    finalSha.update(reinterpret_cast<uint8_t*>(SYSTEM_HASH_SALT.data()), SYSTEM_HASH_SALT.size());
    
    return toByteArray(finalSha.digest());
}

std::array<std::byte, SHA256_SIZE> SystemDat::generateSecondSha()
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

    std::vector<std::byte> saveData = getSaveData();

    SHA256 sha;
    sha.update(reinterpret_cast<uint8_t*>(input.data()), INPUT_SIZE);
    sha.update(reinterpret_cast<uint8_t*>(saveData.data()), SAVE_DATA_SIZE);

    std::array<std::byte, SHA256_SIZE> digest = toByteArray(sha.digest());

    SHA256 finalSha;
    finalSha.update(reinterpret_cast<uint8_t*>(digest.data()), SHA256_SIZE);
    finalSha.update(reinterpret_cast<uint8_t*>(SYSTEM_HASH_SALT.data()), SYSTEM_HASH_SALT.size());
    
    return toByteArray(finalSha.digest());
}

void SystemDat::fixChecksums()
{
    std::array<std::byte, SHA256_SIZE> firstSha = generateFirstSha();
    fixFirstChecksum(firstSha);

    std::array<std::byte, SHA256_SIZE> secondSha = generateSecondSha();
    fixSecondChecksum(secondSha);
    fixThirdChecksum(secondSha);
}

void SystemDat::fixFirstChecksum(std::array<std::byte, SHA256_SIZE> sha)
{
    std::copy(sha.begin(), sha.end(), m_buffer.begin());
}

void SystemDat::fixSecondChecksum(std::array<std::byte, SHA256_SIZE> sha)
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

void SystemDat::fixThirdChecksum(std::array<std::byte, SHA256_SIZE> sha)
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