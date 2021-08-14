project(libladspa)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libladspa_SRCS
    ladspahost.cpp
    ladspahelper.cpp
    ladspaslider.cpp
    ladspabutton.cpp
    settingsdialog.cpp
    effectladspafactory.cpp
    settingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(ladspa MODULE ${libladspa_SRCS})
add_dependencies(ladspa libqmmp)
target_link_libraries(ladspa PRIVATE Qt6::Widgets libqmmp ${CMAKE_DL_LIBS})
install(TARGETS ladspa DESTINATION ${PLUGIN_DIR}/Effect)

