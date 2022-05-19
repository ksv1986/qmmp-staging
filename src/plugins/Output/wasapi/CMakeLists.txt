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
  settingsdialog.ui
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(WASAPI_FOUND)
ADD_LIBRARY(wasapi MODULE ${libwasapi_SRCS})
add_dependencies(wasapi libqmmp)
target_link_libraries(wasapi PRIVATE Qt6::Widgets libqmmp -lstrmiids -ldmoguids -lmsdmo -lole32 -loleaut32 -luuid -lgdi32 -lksuser)
install(TARGETS wasapi DESTINATION ${PLUGIN_DIR}/Output)
ENDIF(WASAPI_FOUND)
