project(libmonotostereo)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libmonotostereo_SRCS
    monotostereoplugin.cpp
    effectmonotostereofactory.cpp
)

SET(libmonotostereo_HDRS
    monotostereoplugin.h
)

SET(libmonotostereo_RCCS translations/translations.qrc)

QT5_ADD_RESOURCES(libmonotostereo_RCC_SRCS ${libmonotostereo_RCCS})

# user interface

SET(libmonotostereo_UIS
    settingsdialog.ui
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})


ADD_LIBRARY(monotostereo MODULE ${libmonotostereo_SRCS} ${libmonotostereo_RCC_SRCS} ${libmonotostereo_HDRS})
add_dependencies(monotostereo libqmmp)
target_link_libraries(monotostereo Qt5::Widgets libqmmp)
install(TARGETS monotostereo DESTINATION ${PLUGIN_DIR}/Effect)
