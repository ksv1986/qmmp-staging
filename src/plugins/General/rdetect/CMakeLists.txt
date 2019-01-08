project(librdetect)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(librdetect_SRCS
    rdetectfactory.cpp
    removablehelper.cpp
    settingsdialog.cpp
)

SET(librdetect_RCCS translations/translations.qrc)

QT5_ADD_RESOURCES(librdetect_RCC_SRCS ${librdetect_RCCS})

# user interface

SET(librdetect_UIS
    settingsdialog.ui
)

QT5_WRAP_UI(librdetect_UIS_H ${librdetect_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(rdetect MODULE ${librdetect_SRCS} ${librdetect_UIS_H} ${librdetect_RCC_SRCS})
add_dependencies(rdetect libqmmpui)
target_link_libraries(rdetect Qt5::Widgets libqmmpui libqmmp)
install(TARGETS rdetect DESTINATION ${PLUGIN_DIR}/General)
