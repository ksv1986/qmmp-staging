project(libqmmpfiledialog)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libqmmpfiledialog_SRCS
    qmmpfiledialogimpl.cpp
    qmmpfiledialog.cpp
    qmmpfiledialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(qmmpfiledialog MODULE ${libqmmpfiledialog_SRCS})
add_dependencies(qmmpfiledialog libqmmpui)
target_link_libraries(qmmpfiledialog PRIVATE Qt6::Widgets libqmmpui libqmmp)
install(TARGETS qmmpfiledialog DESTINATION ${PLUGIN_DIR}/FileDialogs)
