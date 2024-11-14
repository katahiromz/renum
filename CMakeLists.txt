# CMakeLists.txt --- CMake project settings
##############################################################################

# CMake minimum version
cmake_minimum_required(VERSION 3.6)

# project name and languages
project(renum CXX)

##############################################################################

# renum.exe
add_executable(renum renum.cpp)
target_compile_definitions(renum PRIVATE -DRENUM_EXE)

# librenum.a
add_library(librenum STATIC renum.cpp)
target_include_directories(librenum PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
set_target_properties(librenum PROPERTIES PREFIX "")

##############################################################################
