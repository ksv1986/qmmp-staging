project(liboss4)

INCLUDE(CheckIncludeFile)
INCLUDE(CheckCXXSourceCompiles)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

#oss4
include_directories(/usr/lib/oss/include /usr/local/lib/oss/include)
CHECK_INCLUDE_FILE(sys/soundcard.h SYS_SOUNDCARD_H_FOUND)
IF(SYS_SOUNDCARD_H_FOUND)
ADD_DEFINITIONS(-DHAVE_SYS_SOUNDCARD_H)
SET(CMAKE_REQUIRED_DEFINITIONS -DHAVE_SYS_SOUNDCARD_H)
ENDIF(SYS_SOUNDCARD_H_FOUND)


SET(CMAKE_REQUIRED_INCLUDES 
   /usr/lib/oss/include/
   /usr/local/lib/oss/include 
   ${CMAKE_REQUIRED_INCLUDES})

CHECK_CXX_SOURCE_COMPILES("
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_SYS_SOUNDCARD_H
#include <sys/soundcard.h>
#else
#include <soundcard.h>
#endif

#ifndef SNDCTL_DSP_COOKEDMODE
#error \\\"oss test failed\\\"
#endif


int main (int argc, char **argv)
{
   exit(0);
}" OSS4_SUPPORT)

IF(OSS4_SUPPORT)
SET(OSS4_FOUND TRUE CACHE INTERNAL "oss4")
ENDIF(OSS4_SUPPORT)


# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)


SET(liboss4_SRCS
  outputoss4factory.cpp
  outputoss4.cpp
  settingsdialog.cpp
  settingsdialog.ui
  translations/translations.qrc
)

IF(OSS4_FOUND)
ADD_LIBRARY(oss4 MODULE ${liboss4_SRCS})
add_dependencies(oss4 libqmmp)
target_link_libraries(oss4 PRIVATE Qt6::Widgets libqmmp)
install(TARGETS oss4 DESTINATION ${PLUGIN_DIR}/Output)
ENDIF(OSS4_FOUND)
