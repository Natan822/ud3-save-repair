#include <array>
#include <loguru.hpp>
#include <SHA256.h>
#include "GenericDat.h"

GenericDat::GenericDat(std::string path): SaveFile(path){}
GenericDat::~GenericDat(){}

std::vector<std::byte> GenericDat::getSaveData()
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
    saveData.resize(m_buffer.size() - SAVE_DATA_OFFSET);

    std::copy(m_buffer.begin() + SAVE_DATA_OFFSET, m_buffer.end(), saveData.begin());

    return saveData;
}

void GenericDat::fixChecksums()
{
    std::array<std::byte, SHA256_SIZE> checksum = generateSha();
    std::copy(checksum.begin(), checksum.end(), m_buffer.begin());
}

std::array<std::byte, SHA256_SIZE> GenericDat::generateSha()
{
    std::vector<std::byte> saveData = getSaveData();

    SHA256 sha;
    sha.update(reinterpret_cast<uint8_t*>(saveData.data()), saveData.size());
    std::array<std::byte, SHA256_SIZE> digest = toByteArray(sha.digest());

    SHA256 finalSha;
    finalSha.update(reinterpret_cast<uint8_t*>(digest.data()), digest.size());
    finalSha.update(reinterpret_cast<uint8_t*>(GENERIC_HASH_SALT.data()), GENERIC_HASH_SALT.size());
    
    return toByteArray(finalSha.digest());
}