make_executable()

target_link_libraries(${PROJECT}
    ${OPENGL_LIBRARIES}
    ${FLTK_BASE_LIBRARY}
    ${FLTK_GL_LIBRARY}
    )

target_include_directories(${PROJECT} PUBLIC
    ${OPENGL_INCLUDE_DIR}
    ${FLTK_INCLUDE_DIR}
    )
