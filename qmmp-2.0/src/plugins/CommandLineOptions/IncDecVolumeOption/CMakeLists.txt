project(libincdecvolumeoption)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libincdecvolumeoption_SRCS
    incdecvolumeoption.cpp
    translations/translations.qrc
)

ADD_LIBRARY(incdecvolumeoption MODULE ${libincdecvolumeoption_SRCS})
add_dependencies(incdecvolumeoption libqmmpui)
target_link_libraries(incdecvolumeoption PRIVATE Qt6::Widgets libqmmpui libqmmp)
install(TARGETS incdecvolumeoption DESTINATION ${PLUGIN_DIR}/CommandLineOptions)
