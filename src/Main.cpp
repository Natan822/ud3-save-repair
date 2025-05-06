#include <iostream>
#include <cstring>
#include <fstream>
#include <sha256.h>
#include <loguru.hpp>
#include "SaveFile.h"

void usageMessage()
{
    std::cout << "usage:\n";
    std::cout << "    UD3-Checksum-Repair.exe <input-path>" << std::endl;
}

int main(int argc, char** argv)
{   
    loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
    loguru::init(argc, argv);

    if (argc < 2)
    {
        usageMessage();
        return -1;
    }

    if (std::strcmp(argv[1], "-h") == 0 || std::strcmp(argv[1], "--help") == 0)
    {
        usageMessage();
        return 0;
    }
    
    std::string path = argv[1];

    SaveFile file(path);

    if (!file.isValid())
    {
        std::cout << "Error: failed to open input file" << std::endl;
        return -2;
    }
    
    file.fixChecksums();
    file.save("fixed-SYSTEM.DAT");

    std::cout << "Checksum repaired. File saved: `fixed-SYSTEM.DAT`" << std::endl;
    return 0;
}