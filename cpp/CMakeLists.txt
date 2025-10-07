cmake_minimum_required(VERSION 3.15)
project(sa_app CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(sa_lib
    src/suffix_array_mm.cpp
    src/sais.cpp
    src/fm_index.cpp
)
target_include_directories(sa_lib PUBLIC include)

add_executable(sa_app src/main.cpp)
target_link_libraries(sa_app PRIVATE sa_lib)
