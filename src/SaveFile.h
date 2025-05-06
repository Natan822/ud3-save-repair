#pragma once

#include <string>
#include <vector>

const int USER_ID_SIZE = 0x20;
const int SAVE_DATA_SIZE = 0x48F60;
const int SHA256_SIZE = 0x20;

const int USER_ID_OFFSET = 0x20;
const int SAVE_DATA_OFFSET = 0x80;

const int SECOND_CHECKSUM_OFFSET = 0x40;
const int THIRD_CHECKSUM_OFFSET = 0x48BD8;

class SaveFile
{
public:
    SaveFile(std::string path);
    ~SaveFile();

    void fixChecksums();
    void save(std::string path);

    std::array<std::byte, USER_ID_SIZE> getUserId();
    std::array<std::byte, SAVE_DATA_SIZE> getSaveData();

    void test();
    bool isValid();
private:
    static std::array<uint8_t, 0x10> HASH_SALT;

    std::vector<std::byte> m_buffer;
    std::string m_path;
    bool m_valid;
    
    void load();

    void cleanPath(std::string& path);

    std::array<std::byte, SHA256_SIZE> generateFirstSha();
    std::array<std::byte, SHA256_SIZE> generateSecondSha();

    const int INDEX_MAPPING[4] = {1, 4, 11, 14};
    void fixFirstChecksum(std::array<std::byte, SHA256_SIZE> sha);
    void fixSecondChecksum(std::array<std::byte, SHA256_SIZE> sha);
    void fixThirdChecksum(std::array<std::byte, SHA256_SIZE> sha);

    static std::array<std::byte, SHA256_SIZE> toByteArray(std::array<uint8_t, SHA256_SIZE> src);
};