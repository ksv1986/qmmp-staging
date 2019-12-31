project(libffmpeg)

# fixes ffmpeg defines
ADD_DEFINITIONS(-D__STDC_CONSTANT_MACROS)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# minimal supported versions
# ffmpeg-3.2: libavcodec-57.64.100, libavformat-57.56.100, libavutil-55.34.100
pkg_check_modules(FFMPEG libavcodec>=57.64.0 libavformat>=57.56.0 libavutil>=55.34.0)

include_directories(${FFMPEG_INCLUDE_DIRS})
link_directories(${FFMPEG_LIBRARY_DIRS})
ADD_DEFINITIONS(${FFMPEG_CFLAGS})

SET(libffmpeg_SRCS
  decoder_ffmpeg.cpp
  decoder_ffmpegcue.cpp
  decoderffmpegfactory.cpp
  ffmpegmetadatamodel.cpp
  settingsdialog.cpp
  replaygainreader.cpp
)

SET(libffmpeg_HDRS
  decoder_ffmpeg.h
  decoder_ffmpegcue.h
  replaygainreader.h
)

SET(libffmpeg_RCCS translations/translations.qrc)

QT5_ADD_RESOURCES(libffmpeg_RCC_SRCS ${libffmpeg_RCCS})

# user interface

SET(libffmpeg_UIS
  settingsdialog.ui
)

QT5_WRAP_UI(libffmpeg_UIS_H ${libffmpeg_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(FFMPEG_FOUND)
ADD_LIBRARY(ffmpeg MODULE ${libffmpeg_SRCS} ${libffmpeg_UIS_H}
  ${libffmpeg_RCC_SRCS} ${libffmpeg_HDRS})
add_dependencies(ffmpeg libqmmp)
target_link_libraries(ffmpeg Qt5::Widgets libqmmp ${FFMPEG_LDFLAGS})
install(TARGETS ffmpeg DESTINATION ${PLUGIN_DIR}/Input)
ENDIF(FFMPEG_FOUND)
