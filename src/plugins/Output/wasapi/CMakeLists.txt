project(libwasapi)

INCLUDE(CheckIncludeFile)

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

SET(libwasapi_RCCS translations/translations.qrc)

QT5_ADD_RESOURCES(libwasapi_RCC_SRCS ${libwasapi_RCCS})

# user interface

SET(libwasapi_UIS
    settingsdialog.ui
)

QT5_WRAP_UI(libwasapi_UIS_H ${libwasapi_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(WASAPI_FOUND)
ADD_LIBRARY(wasapi MODULE ${libwasapi_SRCS} ${libwasapi_RCC_SRCS} ${libwasapi_HDRS} ${libwasapi_UIS_H})
add_dependencies(wasapi qmmp)
target_link_libraries(wasapi Qt5::Widgets libqmmp -lstrmiids -ldmoguids -lmsdmo -lole32 -loleaut32 -luuid -lgdi32 -lksuser)
install(TARGETS wasapi DESTINATION ${LIB_DIR}/qmmp/Output)
ENDIF(WASAPI_FOUND)
