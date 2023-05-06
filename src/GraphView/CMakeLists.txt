make_executable()

target_precompile_headers(${PROJECT} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/pch.h)

target_include_directories(${PROJECT} PUBLIC
    ${FLTK_INCLUDE_DIR}
    )

target_link_libraries(${PROJECT}
    ${FLTK_LIBRARIES}
    )
