project(libtaskbar)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libtaskbar_SRCS
    taskbarhelper.cpp
    taskbarfactory.cpp
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(taskbar MODULE ${libtaskbar_SRCS})
add_dependencies(taskbar libqmmpui)
target_link_libraries(taskbar PRIVATE Qt6::Widgets Qt6::WinExtras libqmmpui libqmmp)
install(TARGETS taskbar DESTINATION ${PLUGIN_DIR}/General)
