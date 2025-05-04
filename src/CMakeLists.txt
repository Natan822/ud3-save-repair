set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(UD3-Save-Editor Main.cpp SaveFile.cpp Utils.cpp)

target_link_libraries(
    UD3-Save-Editor
    PRIVATE
    sha256
)