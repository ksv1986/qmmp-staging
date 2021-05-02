project(librdetect)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(librdetect_SRCS
    rdetectfactory.cpp
    removablehelper.cpp
    settingsdialog.cpp
    settingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(rdetect MODULE ${librdetect_SRCS})
add_dependencies(rdetect libqmmpui)
target_link_libraries(rdetect PRIVATE Qt6::Widgets libqmmpui libqmmp)
install(TARGETS rdetect DESTINATION ${PLUGIN_DIR}/General)
