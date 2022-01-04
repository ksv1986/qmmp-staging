project(libscrobbler)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
#libqmmp
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libscrobbler_SRCS
    settingsdialog.cpp
    scrobblerhandler.cpp
    scrobblerfactory.cpp
    scrobblercache.cpp
    scrobbler.cpp
    settingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(scrobbler MODULE ${libscrobbler_SRCS})
add_dependencies(scrobbler libqmmpui)
target_link_libraries(scrobbler PRIVATE Qt6::Widgets Qt6::Network libqmmpui libqmmp)
install(TARGETS scrobbler DESTINATION ${PLUGIN_DIR}/General)
