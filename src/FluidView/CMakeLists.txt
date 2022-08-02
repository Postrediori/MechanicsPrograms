make_executable()

target_precompile_headers(${PROJECT} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/pch.h)

target_link_libraries(${PROJECT}
    ${FLTK_BASE_LIBRARY}
    )

target_include_directories(${PROJECT} PUBLIC
    ${FLTK_INCLUDE_DIR}
    )
