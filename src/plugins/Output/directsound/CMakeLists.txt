project(libdirectsound)

INCLUDE(CheckIncludeFile)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# Win32 DirectSound
CHECK_INCLUDE_FILE(dsound.h DSOUND_H_FOUND)

SET(libdirectsound_SRCS
  outputdirectsound.cpp
  outputdirectsoundfactory.cpp
  translations/translations.qrc
)

IF(DSOUND_H_FOUND)
ADD_LIBRARY(directsound MODULE ${libdirectsound_SRCS})
add_dependencies(directsound libqmmp)
target_link_libraries(directsound PRIVATE Qt6::Widgets libqmmp -ldxguid -lstrmiids -ldmoguids -lmsdmo -lole32 -loleaut32 -luuid -lgdi32 -ldsound)
install(TARGETS directsound DESTINATION ${PLUGIN_DIR}/Output)
ENDIF(DSOUND_H_FOUND)

