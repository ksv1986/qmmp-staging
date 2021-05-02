project(libffmpeg)

# fixes ffmpeg defines
ADD_DEFINITIONS(-D__STDC_CONSTANT_MACROS)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# minimal supported versions
# ffmpeg-3.4: libavcodec-57.107.100, libavformat-57.83.100, libavutil-55.78.100
pkg_check_modules(FFMPEG libavcodec>=57.107.0 libavformat>=57.83.0 libavutil>=55.78.0)

include_directories(${FFMPEG_INCLUDE_DIRS})
link_directories(${FFMPEG_LIBRARY_DIRS})
ADD_DEFINITIONS(${FFMPEG_CFLAGS})

include_directories(${TAGLIB_INCLUDE_DIRS})
link_directories(${TAGLIB_LIBRARY_DIRS})
ADD_DEFINITIONS(${TAGLIB_CFLAGS})

SET(libffmpeg_SRCS
  decoder_ffmpeg.cpp
  decoder_ffmpegcue.cpp
  decoder_ffmpegm4b.cpp
  decoderffmpegfactory.cpp
  ffmpegmetadatamodel.cpp
  settingsdialog.cpp
  replaygainreader.cpp
  settingsdialog.ui
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(FFMPEG_FOUND)
ADD_LIBRARY(ffmpeg MODULE ${libffmpeg_SRCS})
add_dependencies(ffmpeg libqmmp)
target_link_libraries(ffmpeg PRIVATE Qt6::Widgets libqmmp ${FFMPEG_LDFLAGS} ${TAGLIB_LDFLAGS})
install(TARGETS ffmpeg DESTINATION ${PLUGIN_DIR}/Input)
ENDIF(FFMPEG_FOUND)
