project(MechanicsPrograms)
cmake_minimum_required(VERSION 2.8)

# 3rd party
include(cmake/3rdparty.cmake)

# common properties
include(cmake/config.cmake)

# projects
add_subdirectory(src)
