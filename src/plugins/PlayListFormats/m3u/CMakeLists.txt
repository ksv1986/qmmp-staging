project(libm3uplaylistformat)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)

SET(libm3uplaylistformat_SRCS
    m3uplaylistformat.cpp
)

ADD_LIBRARY(m3uplaylistformat MODULE ${libm3uplaylistformat_SRCS})

add_dependencies(m3uplaylistformat libqmmpui)
target_link_libraries(m3uplaylistformat PRIVATE Qt6::Widgets libqmmpui)
install(TARGETS m3uplaylistformat DESTINATION ${PLUGIN_DIR}/PlayListFormats)
