cmake_minimum_required(VERSION 3.0.0)
project(tokenize VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(TSN_TOKENIZE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE STRING "TSN Parser Include Dir")

set (all_sources "")
macro(add_sources group dir)
    set(_all "")

    file(GLOB _inc "include/tokenize/${dir}/*.h" "include/tokenize/${dir}/*.hpp" "include/tokenize/${dir}/*.inl")
    list(APPEND _all ${_inc})

    file(GLOB _src "src/${dir}/*.cpp" "src/${dir}/*.c")
    list(APPEND _all ${_src})

    list(APPEND all_sources ${_all})
    string(REPLACE "/" "\\" group_path "${group}")
    source_group(${group_path} FILES ${_all})

    unset(_inc)
    unset(_src)
    unset(_all)
endmacro(add_sources)

add_sources("src"            ".")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(
    ${TSN_TOKENIZE_INCLUDE_DIR}
    ${TSN_UTILS_INCLUDE_DIR}
)

if (NOT TARGET utils)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/utils)
endif()

add_library(tokenize ${all_sources})
target_link_libraries(tokenize utils)

if (CMAKE_VERSION VERSION_GREATER 3.7.8)
    if (MSVC_IDE)
        option(VS_ADD_NATIVE_VISUALIZERS "Configure project to use Visual Studio native visualizers" TRUE)
    else()
        set(VS_ADD_NATIVE_VISUALIZERS FALSE CACHE INTERNAL "Native visualizers are a Visual Studio extension" FORCE)
    endif()

    # add natvis file to the library so it will automatically be loaded into Visual Studio
    if (VS_ADD_NATIVE_VISUALIZERS)
        target_sources(tokenize INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}/types.natvis
        )
    endif()
endif()

add_subdirectory("./test")