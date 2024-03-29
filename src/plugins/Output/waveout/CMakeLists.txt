project(libwaveout)

INCLUDE(CheckIncludeFile)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# Win32 waveout
CHECK_INCLUDE_FILE(windows.h WINDOWS_H_FOUND)

SET(libwaveout_SRCS
  outputwaveout.cpp
  outputwaveoutfactory.cpp
  translations/translations.qrc
)

IF(WINDOWS_H_FOUND)
ADD_LIBRARY(waveout MODULE ${libwaveout_SRCS} ${libwaveout_RCC_SRCS} ${libwaveout_HDRS})
add_dependencies(waveout libqmmp)
target_link_libraries(waveout PRIVATE Qt6::Widgets libqmmp -lwinmm)
install(TARGETS waveout DESTINATION ${PLUGIN_DIR}/Output)
ENDIF(WINDOWS_H_FOUND)

