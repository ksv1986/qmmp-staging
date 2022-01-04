project(libanalyzer)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libanalyzer_SRCS
    analyzer.cpp
    colorwidget.cpp
    settingsdialog.cpp
    visualanalyzerfactory.cpp
    fft.c
    settingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(analyzer MODULE ${libanalyzer_SRCS})
add_dependencies(analyzer libqmmp)
target_link_libraries(analyzer PRIVATE Qt6::Widgets libqmmp)
install(TARGETS analyzer DESTINATION ${PLUGIN_DIR}/Visual)
