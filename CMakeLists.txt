project(MechanicsPrograms LANGUAGES C CXX)
cmake_minimum_required(VERSION 3.11)

option(USE_OPENGL1_4 "Use OpenGL 1.4 for rendering" OFF)

set(CMAKE_CXX_STANDARD 17)

# 3rd party
include(cmake/3rdparty.cmake)

# common properties
include(cmake/config.cmake)

# projects
add_subdirectory(src)
