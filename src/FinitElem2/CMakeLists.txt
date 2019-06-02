make_executable()

include_directories(
    ${ARMADILLO_INCLUDE_DIRS}
    )

target_link_libraries(${PROJECT}
    ${ARMADILLO_LIBRARIES}
    )

# Data files
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/model.txt
    ${CMAKE_CURRENT_BINARY_DIR}/data/model.txt @ONLY)
