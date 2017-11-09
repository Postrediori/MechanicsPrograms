include_directories(
    ${OPENGL_INCLUDE_DIR}
    ${FLTK_INCLUDE_DIR})
if(MSVC)
    add_definitions(
        -D_USE_MATH_DEFINES)
endif()

make_fltk_executable()

