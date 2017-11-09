project(MechanicsPrograms)
cmake_minimum_required(VERSION 2.8)

# common properties
macro(make_project)
    if (NOT DEFINED PROJECT)
        get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    endif ()

    project(${PROJECT})

#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

    if (NOT DEFINED HEADERS)
        file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
    endif ()

    if (NOT DEFINED SOURCES)
        file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
    endif ()

    source_group("Header Files" FILES ${HEADERS})
    source_group("Source Files" FILES ${SOURCES})
endmacro()

macro(make_executable)
    make_project()
    add_executable(${PROJECT} ${HEADERS} ${SOURCES})
endmacro()

macro(make_fltk_executable)
    make_project()
    add_executable(${PROJECT} ${HEADERS} ${SOURCES})
    target_link_libraries(${PROJECT}
        ${OPENGL_LIBRARIES}
        ${FLTK_BASE_LIBRARY}
        ${FLTK_GL_LIBRARY})
endmacro()

#3rd party
find_package(OpenGL REQUIRED)
find_package(FLTK REQUIRED)

#build
add_subdirectory(FEMSolve)
add_subdirectory(FinitElem)
add_subdirectory(FluidView)
add_subdirectory(HazeDyn)
add_subdirectory(MediaWave)
add_subdirectory(SimplexView)
add_subdirectory(WaveView)

