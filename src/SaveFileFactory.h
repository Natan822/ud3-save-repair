#include <memory>
#include "SaveFile.h"

namespace SaveFileFactory
{
    extern std::array<std::string, 8> genericFiles;
    std::unique_ptr<SaveFile> createSaveFile(std::string path);
}