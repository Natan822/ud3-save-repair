#pragma once

#include <string>
#include <vector>

const int USER_ID_SIZE = 0x20;
const int SAVE_DATA_SIZE = 0x48F60;
const int SHA256_SIZE = 0x20;

const int USER_ID_OFFSET = 0x20;
const int SAVE_DATA_OFFSET = 0x80;

class SaveFile
{
public:
    SaveFile(std::string path);
    ~SaveFile();

    void fixChecksums();

    std::array<std::byte, USER_ID_SIZE> getUserId();
    std::array<std::byte, SAVE_DATA_SIZE> getSaveData();

    void test();
private:
    static std::array<uint8_t, 0x10> HASH_SALT;

    std::vector<std::byte> m_buffer;
    std::string m_path;
    bool m_valid;
    
    void load();
    void save();

    void cleanPath(std::string& path);

    std::array<std::byte, SHA256_SIZE> generateFirstChecksum();
    std::array<std::byte, SHA256_SIZE> generateSecondChecksum();

    void fixFirstChecksum(std::array<std::byte, SHA256_SIZE> checksum);
    void fixSecondChecksum(std::array<std::byte, SHA256_SIZE> checksum);

    static std::array<std::byte, SHA256_SIZE> toByteArray(std::array<uint8_t, SHA256_SIZE> src);
};