project(libhal)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libhal_SRCS
    halfactory.cpp
    halmanager.cpp
    halplugin.cpp
    haldevice.cpp
    settingsdialog.cpp
    settingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(hal MODULE ${libhal_SRCS})
add_dependencies(hal libqmmpui)
target_link_libraries(hal PRIVATE Qt6::Widgets Qt6::DBus libqmmpui libqmmp)
install(TARGETS hal DESTINATION ${PLUGIN_DIR}/General)
