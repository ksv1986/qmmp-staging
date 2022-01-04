project(libplsplaylistformat)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)

SET(libplsplaylistformat_SRCS
    plsplaylistformat.cpp
)

ADD_LIBRARY(plsplaylistformat MODULE ${libplsplaylistformat_SRCS})
add_dependencies(plsplaylistformat libqmmpui)
target_link_libraries(plsplaylistformat PRIVATE Qt6::Widgets libqmmpui)
install(TARGETS plsplaylistformat DESTINATION ${PLUGIN_DIR}/PlayListFormats)
