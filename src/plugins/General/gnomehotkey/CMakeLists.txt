project(libgnomehotkey)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libgnomehotkey_SRCS
    gnomehotkeyfactory.cpp
    mediakeys.cpp
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(gnomehotkey MODULE ${libgnomehotkey_SRCS} ${libgnomehotkey_RCC_SRCS})
add_dependencies(gnomehotkey libqmmpui)
target_link_libraries(gnomehotkey PRIVATE Qt6::Widgets Qt6::DBus libqmmpui libqmmp)
install(TARGETS gnomehotkey DESTINATION ${PLUGIN_DIR}/General)
