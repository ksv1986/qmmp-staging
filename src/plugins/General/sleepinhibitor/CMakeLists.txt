project(libsleepinhibitor)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libsleepinhibitor_SRCS
    sleepinhibitor.cpp
    sleepinhibitorfactory.cpp
)

SET(libsleepinhibitor_RCCS 
    translations/translations.qrc)

QT5_ADD_RESOURCES(libsleepinhibitor_RCC_SRCS ${libsleepinhibitor_RCCS})


# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(sleepinhibitor MODULE ${libsleepinhibitor_SRCS} ${libsleepinhibitor_RCC_SRCS})
add_dependencies(sleepinhibitor libqmmpui)
target_link_libraries(sleepinhibitor Qt5::Widgets Qt5::DBus libqmmpui libqmmp)
install(TARGETS sleepinhibitor DESTINATION ${PLUGIN_DIR}/General)
