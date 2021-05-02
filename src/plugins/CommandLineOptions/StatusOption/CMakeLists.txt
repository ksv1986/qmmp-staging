project(statusoption)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(statusoption_SRCS
    statusoption.cpp
    translations/translations.qrc
)

ADD_LIBRARY(statusoption MODULE ${statusoption_SRCS})
add_dependencies(statusoption libqmmpui)
target_link_libraries(statusoption Qt6::Widgets libqmmpui libqmmp)
install(TARGETS statusoption DESTINATION ${PLUGIN_DIR}/CommandLineOptions)
