make_executable()

target_include_directories(${PROJECT} PUBLIC
    ${ARMADILLO_INCLUDE_DIRS}
    )

target_link_libraries(${PROJECT}
    ${ARMADILLO_LIBRARIES}
    )

# Data files
configure_file(
    ${CMAKE_SOURCE_DIR}/models/model.txt
    ${CMAKE_CURRENT_BINARY_DIR}/models/model.txt COPYONLY)
