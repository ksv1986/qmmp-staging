project(seekoption)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(seekoption_SRCS
    seekoption.cpp
    translations/translations.qrc
)

ADD_LIBRARY(seekoption MODULE ${seekoption_SRCS})
add_dependencies(seekoption libqmmpui)
target_link_libraries(seekoption PRIVATE Qt6::Widgets libqmmpui libqmmp)
install(TARGETS seekoption DESTINATION ${PLUGIN_DIR}/CommandLineOptions)
