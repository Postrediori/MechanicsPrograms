make_executable()

# Data files
configure_file(
    ${CMAKE_SOURCE_DIR}/models/model.txt
    ${CMAKE_CURRENT_BINARY_DIR}/models/model.txt COPYONLY)
 