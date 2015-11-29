project(finitelem)
cmake_minimum_required(VERSION 2.6)


# Main executable
set(finitelem_SRCS
    src/utils.h     src/utils.cpp
    src/matrix.h    src/matrix.cpp
    src/seqv.h      src/seqv.cpp 
    src/model.h     src/model.cpp
    src/main.cpp)
add_executable(finitelem ${finitelem_SRCS})
if(MSVC)
    add_definitions(
        -D_USE_MATH_DEFINES)
endif()


# Data files
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/model.txt
    ${CMAKE_CURRENT_BINARY_DIR}/data/model.txt COPYONLY)


# Installation
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../bundle/FinitElem")
install(
    TARGETS finitelem
    DESTINATION ${CMAKE_INSTALL_PREFIX})
install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/data"
    DESTINATION ${CMAKE_INSTALL_PREFIX})

