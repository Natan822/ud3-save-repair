#pragma once

#include <string>
#include <vector>

const int SHA256_SIZE = 0x20;

class SaveFile
{
public:
    SaveFile(std::string path);
    virtual ~SaveFile();

    virtual void fixChecksums() = 0;
    void save(std::string path);

    bool isValid();
    std::string getName();
protected:
    virtual std::vector<std::byte> getSaveData() = 0;

    static std::array<uint8_t, 0x10> SYSTEM_HASH_SALT;
    static std::array<uint8_t, 0x10> GENERIC_HASH_SALT;

    std::vector<std::byte> m_buffer;
    std::string m_path;
    std::string m_name;
    bool m_valid;
    
    void load();

    static std::array<std::byte, SHA256_SIZE> toByteArray(std::array<uint8_t, SHA256_SIZE> src);
};