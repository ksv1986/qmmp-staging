project(libudisks)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libudisks_SRCS
    udisksfactory.cpp
    udisksmanager.cpp
    udisksplugin.cpp
    udisksdevice.cpp
    settingsdialog.cpp
    settingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(udisks MODULE ${libudisks_SRCS})
add_dependencies(udisks libqmmpui)
target_link_libraries(udisks PRIVATE Qt6::Widgets Qt6::DBus libqmmpui libqmmp)
install(TARGETS udisks DESTINATION ${PLUGIN_DIR}/General)
