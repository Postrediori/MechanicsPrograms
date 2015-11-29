project(simplexview)
cmake_minimum_required(VERSION 2.6)


# Find packages
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake-modules")
find_package(OpenGL REQUIRED)
find_package(FLTK REQUIRED)


# Include directories
include_directories(
    ${OPENGL_INCLUDE_DIR}
    ${FLTK_INCLUDE_DIR})

# Main executable
set(simplexview_SRCS
    src/Func.cpp            src/Func.h
    src/Graph.cpp           src/Graph.h
    src/SearchEngine.cpp    src/SearchEngine.h
    src/Simplex.cpp         src/Simplex.h
    src/SimplexEngine.cpp   src/SimplexEngine.h
    src/GaussSEngine.cpp    src/GaussSEngine.h
    src/DescentEngine.cpp   src/DescentEngine.h
    src/RelaxationEngine.cpp    src/RelaxationEngine.h
    src/ScanEngine.cpp      src/ScanEngine.h
    src/ContourPlot.cpp     src/ContourPlot.h
    src/GraphWidget.cpp     src/GraphWidget.h
    src/main.cpp)
add_executable(simplexview ${simplexview_SRCS})
target_link_libraries(simplexview
    ${OPENGL_LIBRARIES}
    ${FLTK_BASE_LIBRARY}
    ${FLTK_GL_LIBRARY})
if(MSVC)
    add_definitions(
        -D_USE_MATH_DEFINES)
endif()


# Installation
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../bundle")
install(
    TARGETS simplexview
    DESTINATION ${CMAKE_INSTALL_PREFIX})

