project(libcrossfade)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libcrossfade_SRCS
    crossfadeplugin.cpp
    settingsdialog.cpp
    effectcrossfadefactory.cpp
    settingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})


ADD_LIBRARY(crossfade MODULE ${libcrossfade_SRCS})
add_dependencies(crossfade libqmmp)
target_link_libraries(crossfade PRIVATE Qt6::Widgets libqmmp)
install(TARGETS crossfade DESTINATION ${PLUGIN_DIR}/Effect)
