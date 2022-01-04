project(libmplayer)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)


SET(libmplayer_SRCS
    mplayerenginefactory.cpp
    mplayerengine.cpp
    mplayermetadatamodel.cpp
    settingsdialog.cpp
    settingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(mplayer MODULE ${libmplayer_SRCS})
add_dependencies(mplayer libqmmp)
target_link_libraries(mplayer PRIVATE Qt6::Widgets libqmmp)
install(TARGETS mplayer DESTINATION ${PLUGIN_DIR}/Engines)

