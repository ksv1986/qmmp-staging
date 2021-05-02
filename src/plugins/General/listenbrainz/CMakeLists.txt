project(liblistenbrainz)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
#libqmmp
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(liblistenbrainz_SRCS
    settingsdialog.cpp
    listenbrainzfactory.cpp
    payloadcache.cpp
    listenbrainz.cpp
    settingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(listenbrainz MODULE ${liblistenbrainz_SRCS})
add_dependencies(listenbrainz libqmmpui)
target_link_libraries(listenbrainz PRIVATE Qt6::Widgets Qt6::Network libqmmpui libqmmp)
install(TARGETS listenbrainz DESTINATION ${PLUGIN_DIR}/General)
