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
)

SET(libudisks_RCCS translations/translations.qrc)

QT5_ADD_RESOURCES(libudisks_RCC_SRCS ${libudisks_RCCS})

# user interface

SET(libudisks_UIS
    settingsdialog.ui
)

QT5_WRAP_UI(libudisks_UIS_H ${libudisks_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(udisks MODULE ${libudisks_SRCS} ${libudisks_UIS_H} ${libudisks_RCC_SRCS})
add_dependencies(udisks libqmmpui)
target_link_libraries(udisks Qt5::Widgets Qt5::DBus libqmmpui libqmmp)
install(TARGETS udisks DESTINATION ${PLUGIN_DIR}/General)
