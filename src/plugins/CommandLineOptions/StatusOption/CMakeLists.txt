project(statusoption)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(statusoption_SRCS
    statusoption.cpp
)

SET(statusoption_RCCS translations/translations.qrc)

QT5_ADD_RESOURCES(statusoption_RCC_SRCS ${statusoption_RCCS})


# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(statusoption MODULE ${statusoption_SRCS} ${statusoption_RCC_SRCS})
add_dependencies(statusoption libqmmpui)
target_link_libraries(statusoption Qt5::Widgets libqmmpui libqmmp)
install(TARGETS statusoption DESTINATION ${PLUGIN_DIR}/CommandLineOptions)
