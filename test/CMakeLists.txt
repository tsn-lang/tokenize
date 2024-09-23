cmake_minimum_required(VERSION 3.0.0)
project(tokenize_test VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(
    ${TSN_TOKENIZE_INCLUDE_DIR}
    ${TSN_UTILS_INCLUDE_DIR}
    ${FFI_INCLUDE_DIR}
)

if (NOT TARGET Catch2::Catch2)
    add_subdirectory(Catch2)
endif()

file(GLOB all_sources "./*.cpp")
add_executable(tokenize_test ${all_sources})
target_link_libraries(tokenize_test tokenize Catch2::Catch2)