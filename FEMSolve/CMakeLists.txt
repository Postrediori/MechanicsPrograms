project(femsolve)
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
set(femsolve_SRCS
    src/utils.h     src/utils.cpp
    src/matrix.h    src/matrix.cpp
    src/seqv.h      src/seqv.cpp 
    src/model.h     src/model.cpp
    src/graphics.h  src/graphics.cpp
    src/modelwidget.h   src/modelwidget.cpp
    src/main.h      src/main.cpp)
add_executable(femsolve ${femsolve_SRCS})
target_link_libraries(femsolve
    ${OPENGL_LIBRARIES}
    ${FLTK_BASE_LIBRARY}
    ${FLTK_GL_LIBRARY})
if(MSVC)
    add_definitions(
        -D_USE_MATH_DEFINES)
endif()


# Data files
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/models/model1.txt
	${CMAKE_CURRENT_BINARY_DIR}/models/model1.txt COPYONLY)
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/models/model2.txt
	${CMAKE_CURRENT_BINARY_DIR}/models/model2.txt COPYONLY)
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/models/model3.txt
	${CMAKE_CURRENT_BINARY_DIR}/models/model3.txt COPYONLY)


# Installation
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../bundle/FEMSolve")
install(
    TARGETS femsolve
    DESTINATION ${CMAKE_INSTALL_PREFIX})
install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/data"
    DESTINATION ${CMAKE_INSTALL_PREFIX})

