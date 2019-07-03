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
)

SET(liblistenbrainz_HDRS
    payloadcache.h
)

SET(liblistenbrainz_RCCS translations/translations.qrc)

QT5_ADD_RESOURCES(liblistenbrainz_RCC_SRCS ${liblistenbrainz_RCCS})

# user interface

SET(liblistenbrainz_UIS
    settingsdialog.ui
)

QT5_WRAP_UI(liblistenbrainz_UIS_H ${liblistenbrainz_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(listenbrainz MODULE ${liblistenbrainz_SRCS} ${liblistenbrainz_UIS_H}
  ${liblistenbrainz_RCC_SRCS} ${liblistenbrainz_HDRS})
add_dependencies(listenbrainz libqmmpui)
target_link_libraries(listenbrainz Qt5::Widgets Qt5::Network libqmmpui libqmmp)
install(TARGETS listenbrainz DESTINATION ${PLUGIN_DIR}/General)
