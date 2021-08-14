project(libconverter)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# taglib
include_directories(${TAGLIB_INCLUDE_DIRS})
link_directories(${TAGLIB_LIBRARY_DIRS})
ADD_DEFINITIONS(${TAGLIB_CFLAGS})


SET(libconverter_SRCS
    converterfactory.cpp
    converter.cpp
    converterhelper.cpp
    converterdialog.cpp
    preseteditor.cpp
    converterdialog.ui
    preseteditor.ui
    translations/translations.qrc
    presets.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(converter MODULE ${libconverter_SRCS})
add_dependencies(converter libqmmpui)
target_link_libraries(converter PRIVATE Qt6::Widgets libqmmpui libqmmp ${TAGLIB_LDFLAGS})
install(TARGETS converter DESTINATION ${PLUGIN_DIR}/General)
