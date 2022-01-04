project(libmms)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# libcurl
pkg_search_module(MMS libmms>=0.4)
include_directories(${MMS_INCLUDE_DIRS})
link_directories(${MMS_LIBRARY_DIRS})
ADD_DEFINITIONS(${MMS_CFLAGS})


SET(libmms_SRCS
    mmsstreamreader.cpp
    mmsinputfactory.cpp
    mmsinputsource.cpp
    settingsdialog.cpp
    settingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})
IF(MMS_FOUND)
ADD_LIBRARY(mms MODULE ${libmms_SRCS})
target_link_libraries(mms PRIVATE Qt6::Widgets libqmmp ${MMS_LDFLAGS})
add_dependencies(mms libqmmp)
install(TARGETS mms DESTINATION ${PLUGIN_DIR}/Transports)
ENDIF(MMS_FOUND)
