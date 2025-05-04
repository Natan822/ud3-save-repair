include(FetchContent)

FetchContent_Declare(
    sha256
    GIT_REPOSITORY https://github.com/System-Glitch/SHA256.git
    GIT_COMMIT 9418899
)

FetchContent_MakeAvailable(sha256)

target_include_directories(
    sha256
    PUBLIC
    ${sha256_SOURCE_DIR}/include
)