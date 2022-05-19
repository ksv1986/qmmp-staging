project(libfileops)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libfileops_SRCS
    fileops.cpp
    fileopsfactory.cpp
    settingsdialog.cpp
    hotkeydialog.cpp
    settingsdialog.ui
    hotkeydialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(fileops MODULE ${libfileops_SRCS})
add_dependencies(fileops libqmmpui)
target_link_libraries(fileops PRIVATE Qt6::Widgets libqmmpui libqmmp)
install(TARGETS fileops DESTINATION ${PLUGIN_DIR}/General)
