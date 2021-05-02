project(libbs2b)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libbs2b_SRCS
    bs2bplugin.cpp
    settingsdialog.cpp
    effectbs2bfactory.cpp
    settingsdialog.ui
    translations/translations.qrc
)

pkg_search_module(BS2B libbs2b>=3.0.0)

include_directories(${BS2B_INCLUDE_DIRS})
link_directories(${BS2B_LIBRARY_DIRS})
ADD_DEFINITIONS(${BS2B_CFLAGS})

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(BS2B_FOUND)
ADD_LIBRARY(bs2b MODULE ${libbs2b_SRCS})
add_dependencies(bs2b libqmmp)
target_link_libraries(bs2b PRIVATE Qt6::Widgets libqmmp ${BS2B_LDFLAGS})
install(TARGETS bs2b DESTINATION ${PLUGIN_DIR}/Effect)
ENDIF(BS2B_FOUND)
