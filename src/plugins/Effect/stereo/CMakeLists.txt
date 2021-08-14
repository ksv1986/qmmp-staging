project(libstereo)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libstereo_SRCS
    stereoplugin.cpp
    settingsdialog.cpp
    effectstereofactory.cpp
    settingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(stereo MODULE ${libstereo_SRCS})
add_dependencies(stereo libqmmp)
target_link_libraries(stereo Qt6::Widgets libqmmp)
install(TARGETS stereo DESTINATION ${PLUGIN_DIR}/Effect)
