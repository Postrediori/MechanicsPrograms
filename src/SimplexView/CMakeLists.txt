make_executable()

target_precompile_headers(${PROJECT} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/pch.h)

target_link_libraries(${PROJECT}
    ${OPENGL_LIBRARIES}
    ${FLTK_LIBRARIES}
    ${HMM_LIBRARY}
    )

target_include_directories(${PROJECT} PUBLIC
    ${OPENGL_INCLUDE_DIR}
    ${FLTK_INCLUDE_DIR}
    )
