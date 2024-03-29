project(libhttp)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# libcurl
pkg_search_module(CURL libcurl >= 7.19.4)
include_directories(${CURL_INCLUDE_DIRS})
link_directories(${CURL_LIBRARY_DIRS})
ADD_DEFINITIONS(${CURL_CFLAGS})

# libenca
IF(USE_ENCA AND ENCA_FOUND)
ADD_DEFINITIONS(-DWITH_ENCA)
include_directories(${ENCA_INCLUDE_DIRS})
link_directories(${ENCA_LIBRARY_DIRS})
ENDIF(USE_ENCA AND ENCA_FOUND)

SET(libhttp_SRCS
    httpstreamreader.cpp
    httpinputfactory.cpp
    httpinputsource.cpp
    settingsdialog.cpp
    settingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(CURL_FOUND)
ADD_LIBRARY(http MODULE ${libhttp_SRCS})

IF(USE_ENCA AND ENCA_FOUND)
target_link_libraries(http PRIVATE Qt6::Widgets libqmmp ${CURL_LDFLAGS} ${CURL_CFLAGS} ${ENCA_LDFLAGS} ${ENCA_CFLAGS})
ELSE(USE_ENCA AND ENCA_FOUND)
target_link_libraries(http PRIVATE Qt6::Widgets libqmmp ${CURL_LDFLAGS})
ENDIF(USE_ENCA AND ENCA_FOUND)

add_dependencies(http libqmmp)
install(TARGETS http DESTINATION ${PLUGIN_DIR}/Transports)
ENDIF(CURL_FOUND)
