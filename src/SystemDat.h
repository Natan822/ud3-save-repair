#include "SaveFile.h"

class SystemDat : public SaveFile
{
public:
    const static int USER_ID_SIZE = 0x20;
    const static int SAVE_DATA_SIZE = 0x48F60;

    const static int USER_ID_OFFSET = 0x20;
    const static int SAVE_DATA_OFFSET = 0x80;

    const static int SECOND_CHECKSUM_OFFSET = 0x40;
    const static int THIRD_CHECKSUM_OFFSET = 0x48BD8;

    SystemDat(std::string path);
    ~SystemDat();

    void fixChecksums() override;

    std::array<std::byte, USER_ID_SIZE> getUserId();

protected:
    std::vector<std::byte> getSaveData() override;

private:
    std::array<std::byte, SHA256_SIZE> generateFirstSha();
    std::array<std::byte, SHA256_SIZE> generateSecondSha();

    const int INDEX_MAPPING[4] = {1, 4, 11, 14};
    void fixFirstChecksum(std::array<std::byte, SHA256_SIZE> sha);
    void fixSecondChecksum(std::array<std::byte, SHA256_SIZE> sha);
    void fixThirdChecksum(std::array<std::byte, SHA256_SIZE> sha);
};