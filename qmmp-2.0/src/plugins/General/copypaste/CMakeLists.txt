project(libcopypaste)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libcopypaste_SRCS
    copypaste.cpp
    copypastefactory.cpp
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(copypaste MODULE ${libcopypaste_SRCS})
add_dependencies(copypaste libqmmpui)
target_link_libraries(copypaste PRIVATE Qt6::Widgets libqmmpui libqmmp)
install(TARGETS copypaste DESTINATION ${PLUGIN_DIR}/General)
