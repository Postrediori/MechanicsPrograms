if(MSVC)
    add_definitions(
        -D_USE_MATH_DEFINES)
endif()

make_executable()

# Data files
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/model.txt
    ${CMAKE_CURRENT_BINARY_DIR}/data/model.txt @ONLY)

