project(libudisks2)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libudisks2_SRCS
    udisks2factory.cpp
    udisks2manager.cpp
    udisks2plugin.cpp
    udisks2device.cpp
    settingsdialog.cpp
    settingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(udisks2 MODULE ${libudisks2_SRCS})
add_dependencies(udisks2 libqmmpui)
target_link_libraries(udisks2 PRIVATE Qt6::Widgets Qt6::DBus libqmmpui libqmmp)
install(TARGETS udisks2 DESTINATION ${PLUGIN_DIR}/General)
