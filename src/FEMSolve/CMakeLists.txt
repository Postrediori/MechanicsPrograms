make_fltk_executable()

# Data files
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/models/model1.txt
    ${CMAKE_CURRENT_BINARY_DIR}/models/model1.txt @ONLY)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/models/model2.txt
    ${CMAKE_CURRENT_BINARY_DIR}/models/model2.txt @ONLY)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/models/model3.txt
    ${CMAKE_CURRENT_BINARY_DIR}/models/model3.txt @ONLY)
