project(liboss)

INCLUDE(CheckIncludeFile)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

#OSS
CHECK_INCLUDE_FILE(sys/soundcard.h SYS_SOUNDCARD_H_FOUND)
IF(SYS_SOUNDCARD_H_FOUND)
ADD_DEFINITIONS(-DHAVE_SYS_SOUNDCARD_H)
ENDIF(SYS_SOUNDCARD_H_FOUND)

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)


SET(liboss_SRCS
  outputossfactory.cpp
  outputoss.cpp
  settingsdialog.cpp
  settingsdialog.ui
  translations/translations.qrc
)

ADD_LIBRARY(oss MODULE ${liboss_SRCS})
add_dependencies(oss libqmmp)
target_link_libraries(oss PRIVATE Qt6::Widgets libqmmp)
install(TARGETS oss DESTINATION ${PLUGIN_DIR}/Output)
