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
)

SET(libstreambrowser_RCCS 
    translations/translations.qrc)

QT5_ADD_RESOURCES(libstreambrowser_RCC_SRCS ${libstreambrowser_RCCS})

# user interface

SET(libstreambrowser_UIS
    streamwindow.ui
)

QT5_WRAP_UI(libstreambrowser_UIS_H ${libstreambrowser_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(streambrowser MODULE ${libstreambrowser_SRCS} ${libstreambrowser_UIS_H} ${libstreambrowser_RCC_SRCS})
add_dependencies(streambrowser libqmmpui)
target_link_libraries(streambrowser Qt5::Widgets Qt5::Network libqmmpui libqmmp)
install(TARGETS streambrowser DESTINATION ${PLUGIN_DIR}/General)
