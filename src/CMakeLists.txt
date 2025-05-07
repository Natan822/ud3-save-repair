set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(UD3-Save-Repair Main.cpp SaveFile.cpp Utils.cpp GenericDat.cpp SystemDat.cpp SaveFileFactory.cpp)

target_link_libraries(
    UD3-Save-Repair
    PRIVATE
    sha256
)