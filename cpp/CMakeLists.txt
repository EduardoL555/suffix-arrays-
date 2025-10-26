# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(fmi_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Flags recomendados (se ignoran si el compilador no los soporta)
if (MSVC)
  add_compile_options(/O2 /permissive- /W4)
else()
  add_compile_options(-O3 -Wall -Wextra -Wpedantic)
endif()

# Estructura de carpetas
#  include/           -> headers *.hpp
#  src/               -> código fuente *.cpp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(fmi_app
  src/main_fmi.cpp
  src/sa_mm.cpp
  src/bwt.cpp
  src/mtf.cpp
  src/rle0.cpp
  src/huffman.cpp
  src/fmi_container.cpp
)

# En Windows con MinGW, asegura que se busque stdc++ adecuadamente
if (MINGW)
  target_link_libraries(fmi_app stdc++)
endif()
