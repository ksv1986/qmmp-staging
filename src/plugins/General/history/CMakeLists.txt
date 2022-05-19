project(libhistory)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libhistory_SRCS
    dateinputdialog.cpp
    history.cpp
    historyfactory.cpp
    historysettingsdialog.cpp
    historywindow.cpp
    progressbaritemdelegate.cpp
    historysettingsdialog.ui
    dateinputdialog.ui
    historywindow.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(history MODULE ${libhistory_SRCS})
target_link_libraries(history PRIVATE Qt6::Widgets Qt6::Sql libqmmpui libqmmp)
install(TARGETS history DESTINATION ${PLUGIN_DIR}/General)
