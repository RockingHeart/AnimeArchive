cmake_minimum_required(VERSION 3.31)

project(AnimeArchive)

set(CMAKE_CXX_STANDARD 23)

if (MSVC)
    add_compile_options(/std:c++latest)
    add_compile_options(/std:clatest)
    add_compile_options(/experimental:module)
    add_compile_options(/permissive-)
else()
    add_compile_options(-fmodules)
endif()

file(GLOB Header "src/*.ixx")

add_executable(AnimeArchive
    src/main.cpp
)

target_sources(AnimeArchive
    PRIVATE
    FILE_SET mods TYPE CXX_MODULES
    FILES ${Header}
)

source_group("Includes" FILES ${Header})
source_group("Execute" src/main.cpp)