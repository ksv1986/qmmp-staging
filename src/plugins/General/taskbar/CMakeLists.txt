project(libtaskbar)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libtaskbar_SRCS
    taskbarhelper.cpp
    taskbarfactory.cpp
)

SET(libtaskbar_RCCS translations/translations.qrc)

QT5_ADD_RESOURCES(libtaskbar_RCC_SRCS ${libtaskbar_RCCS})

# user interface


# QT5_WRAP_UI(libtaskbar_UIS_H ${libtaskbar_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(taskbar MODULE ${libtaskbar_SRCS} ${libtaskbar_RCC_SRCS})
add_dependencies(taskbar libqmmpui)
target_link_libraries(taskbar Qt5::Widgets Qt5::WinExtras libqmmpui libqmmp)
install(TARGETS taskbar DESTINATION ${PLUGIN_DIR}/General)
