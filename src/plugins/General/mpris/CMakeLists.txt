project(libmpris)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libmpris_SRCS
    mpris.cpp
    mprisfactory.cpp
    root2object.cpp
    player2object.cpp
)

SET(libmpris_RCCS translations/translations.qrc)

QT5_ADD_RESOURCES(libmpris_RCC_SRCS ${libmpris_RCCS})

# user interface


# QT5_WRAP_UI(libmpris_UIS_H ${libmpris_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(mpris MODULE ${libmpris_SRCS} ${libmpris_RCC_SRCS})
add_dependencies(mpris libqmmpui)
target_link_libraries(mpris Qt5::Widgets Qt5::DBus libqmmpui libqmmp)
install(TARGETS mpris DESTINATION ${PLUGIN_DIR}/General)
