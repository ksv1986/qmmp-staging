project(libtrackchange)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libtrackchange_SRCS
    trackchange.cpp
    trackchangefactory.cpp
    settingsdialog.cpp
    settingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(trackchange MODULE ${libtrackchange_SRCS})
add_dependencies(trackchange libqmmpui)
target_link_libraries(trackchange PRIVATE Qt6::Widgets libqmmpui libqmmp)
install(TARGETS trackchange DESTINATION ${PLUGIN_DIR}/General)
