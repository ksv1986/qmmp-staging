project(libnull)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libnull_SRCS
  outputnull.cpp
  outputnullfactory.cpp
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})


ADD_LIBRARY(null MODULE ${libnull_SRCS})
add_dependencies(null libqmmp)
target_link_libraries(null PRIVATE Qt6::Widgets libqmmp)
install(TARGETS null DESTINATION ${PLUGIN_DIR}/Output)
