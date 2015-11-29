project(MediaWave)
cmake_minimum_required(VERSION 2.6)


# Find packages
find_package(OpenGL REQUIRED)
find_package(FLTK REQUIRED)


# Include directories
include_directories(
    ${OPENGL_INCLUDE_DIR}
    ${FLTK_INCLUDE_DIR})

# Main executable
set(mediawave_SRCS
    src/pixmaps.h
    src/main.cpp)
add_executable(mediawave ${mediawave_SRCS})
target_link_libraries(mediawave
    ${OPENGL_LIBRARIES}
    ${FLTK_BASE_LIBRARY}
    ${FLTK_GL_LIBRARY})
if(MSVC)
    add_definitions(
        -D_USE_MATH_DEFINES)
endif()


# Installation
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../bundle/MediaWave")
install(
    TARGETS mediawave
    DESTINATION ${CMAKE_INSTALL_PREFIX})

