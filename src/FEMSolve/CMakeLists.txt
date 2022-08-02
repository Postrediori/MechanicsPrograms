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

# Data files
configure_file(
    ${CMAKE_SOURCE_DIR}/models/model1.txt
    ${CMAKE_CURRENT_BINARY_DIR}/models/model1.txt @ONLY)
configure_file(
    ${CMAKE_SOURCE_DIR}/models/model2.txt
    ${CMAKE_CURRENT_BINARY_DIR}/models/model2.txt @ONLY)
configure_file(
    ${CMAKE_SOURCE_DIR}/models/model3.txt
    ${CMAKE_CURRENT_BINARY_DIR}/models/model3.txt @ONLY)
