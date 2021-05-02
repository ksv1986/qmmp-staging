project(libstatusicon)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libstatusicon_SRCS
    settingsdialog.cpp
    statusicon.cpp
    statusiconfactory.cpp
    qmmptrayicon.cpp
    statusiconpopupwidget.cpp
    coverwidget.cpp
    settingsdialog.ui
    translations/translations.qrc
    images/tray_images.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(statusicon MODULE ${libstatusicon_SRCS})
add_dependencies(statusicon libqmmpui libqmmp)
target_link_libraries(statusicon PRIVATE Qt6::Widgets libqmmpui libqmmp)
install(TARGETS statusicon DESTINATION ${PLUGIN_DIR}/General)
