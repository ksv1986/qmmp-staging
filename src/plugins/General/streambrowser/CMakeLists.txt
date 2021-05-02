project(libstreambrowser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libstreambrowser_SRCS
    streambrowser.cpp
    streambrowserfactory.cpp
    streamwindow.cpp
    editstreamdialog.cpp
    streamwindow.ui
    editstreamdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(streambrowser MODULE ${libstreambrowser_SRCS})
add_dependencies(streambrowser libqmmpui)
target_link_libraries(streambrowser PRIVATE Qt6::Widgets Qt6::Network libqmmpui libqmmp)
install(TARGETS streambrowser DESTINATION ${PLUGIN_DIR}/General)
