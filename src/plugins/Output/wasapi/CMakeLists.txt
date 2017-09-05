project(libwasapi)


INCLUDE(CheckIncludeFile)

if(COMMAND cmake_policy)
cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# qt plugin
ADD_DEFINITIONS( -Wall )
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_THREAD)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(QT_INCLUDES
  ${QT_INCLUDES}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../
)

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# Windows Audio Session API
CHECK_INCLUDE_FILE(Audioclient.h WASAPI_FOUND)

SET(libwasapi_SRCS
  outputwasapi.cpp
  outputwasapifactory.cpp
  settingsdialog.cpp
)

SET(libwasapi_HDRS
  outputwasapi.h
)

SET(libwasapi_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libwasapi_RCC_SRCS ${libwasapi_RCCS})

# user interface

SET(libwasapi_UIS
    settingsdialog.ui
)

QT4_WRAP_UI(libwasapi_UIS_H ${libwasapi_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(WASAPI_FOUND)
ADD_LIBRARY(wasapi MODULE ${libwasapi_SRCS} ${libwasapi_RCC_SRCS} ${libwasapi_HDRS} ${libwasapi_UIS_H})
add_dependencies(wasapi qmmp)
target_link_libraries(wasapi ${QT_LIBRARIES} libqmmp -lstrmiids -ldmoguids -lmsdmo -lole32 -loleaut32 -luuid -lgdi32 -lksuser)
install(TARGETS wasapi DESTINATION ${LIB_DIR}/qmmp/Output)
ENDIF(WASAPI_FOUND)
