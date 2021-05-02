project(libplaylistoption)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libplaylistoption_SRCS
    playlistoption.cpp
    translations/translations.qrc
)

ADD_LIBRARY(playlistoption MODULE ${libplaylistoption_SRCS})
add_dependencies(playlistoption libqmmpui)
target_link_libraries(playlistoption PRIVATE Qt6::Widgets libqmmpui libqmmp)
install(TARGETS playlistoption DESTINATION ${PLUGIN_DIR}/CommandLineOptions)
