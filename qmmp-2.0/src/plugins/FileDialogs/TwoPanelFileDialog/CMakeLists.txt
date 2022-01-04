project(libtwopanelfiledialog)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libtwopanelfiledialog_SRCS
    twopanelfiledialogimpl.cpp
    twopanelfiledialog.cpp
    twopanelfiledialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(twopanelfiledialog MODULE ${libtwopanelfiledialog_SRCS})
add_dependencies(twopanelfiledialog libqmmpui)
target_link_libraries(twopanelfiledialog PRIVATE Qt6::Widgets libqmmpui libqmmp)
install(TARGETS twopanelfiledialog DESTINATION ${PLUGIN_DIR}/FileDialogs)
