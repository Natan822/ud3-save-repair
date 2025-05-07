#include <array>
#include <loguru.hpp>

#include "SaveFileFactory.h"
#include "Utils.h"
#include "SystemDat.h"
#include "GenericDat.h"

namespace SaveFileFactory
{
    std::array<std::string, 8> genericFiles = {"CAFLIST.DAT",
                                               "CAROLIST.DAT",
                                               "HLRCLIST.DAT",
                                               "HLRFLIST.DAT",
                                               "HLRHLIST.DAT",
                                               "HLRQLIST.DAT",
                                               "PNTLIST.DAT",
                                               "STR.DAT"};

    std::unique_ptr<SaveFile> createSaveFile(std::string path)
    {
        if (path.empty())
        {
            LOG_F(ERROR, "Unable to create file object: path is empty.");
            return nullptr;
        }

        Utils::cleanPath(path);
        std::string name = Utils::splitString(path, "/").back();

        if (name == "SYSTEM.DAT")
        {
            LOG_F(INFO, "SYSTEM.DAT file object created.");
            return std::make_unique<SystemDat>(path);
        }

        for (std::string file : genericFiles)
        {
            if (name == file)
            {
                LOG_F(INFO, "Generic file object created.");
                return std::make_unique<GenericDat>(path);
            }
        }

        LOG_F(ERROR, "Unable to create file object: invalid file.");
        return nullptr;
    }
}