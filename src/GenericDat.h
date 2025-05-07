#include "SaveFile.h"


class GenericDat : public SaveFile
{

const int SAVE_DATA_OFFSET = 0x20;

public:
    GenericDat(std::string path);
    ~GenericDat();

    void fixChecksums();

protected:
    std::vector<std::byte> getSaveData() override;

private:
    std::array<std::byte, SHA256_SIZE> generateSha();
    void fixChecksum(std::array<std::byte, SHA256_SIZE> sha);
};