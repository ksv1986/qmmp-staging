project(libmonotostereo)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libmonotostereo_SRCS
    monotostereoplugin.cpp
    effectmonotostereofactory.cpp
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(monotostereo MODULE ${libmonotostereo_SRCS})
add_dependencies(monotostereo libqmmp)
target_link_libraries(monotostereo PRIVATE Qt6::Widgets libqmmp)
install(TARGETS monotostereo DESTINATION ${PLUGIN_DIR}/Effect)
