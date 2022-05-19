project(doc)

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.cmake"
  IMMEDIATE @ONLY)

add_custom_target(docs COMMAND doxygen ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.cmake WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
