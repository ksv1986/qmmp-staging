project(libmpeg)

INCLUDE(CheckIncludeFileCXX)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# libmad
IF(USE_MAD)
pkg_search_module(MAD libmad mad)
ENDIF(USE_MAD)

# libmpg123
IF(USE_MPG123)
pkg_search_module(MPG123 libmpg123>=1.13.0)
ENDIF(USE_MPG123)

include_directories(${TAGLIB_INCLUDE_DIRS})
link_directories(${TAGLIB_LIBRARY_DIRS})
ADD_DEFINITIONS(${TAGLIB_CFLAGS})

SET(USE_LIBRCD FALSE CACHE BOOL "enable/disable charset detection for ID3v1/2 tags using librcd")

IF(USE_LIBRCD)
  CHECK_INCLUDE_FILE_CXX(librcd.h LIBRCD_HEADER_FOUND)
ENDIF(USE_LIBRCD)

IF(LIBRCD_HEADER_FOUND)
  SET(LIBRCD_FOUND TRUE CACHE INTERNAL "rcd")
  SET(LIBRCD_LDFLAGS -lrcd)
  ADD_DEFINITIONS(-DWITH_LIBRCD)
ENDIF(LIBRCD_HEADER_FOUND)


SET(libmpeg_SRCS
  decodermpegfactory.cpp
  settingsdialog.cpp
  tagextractor.cpp
  mpegmetadatamodel.cpp
  settingsdialog.ui
  translations/translations.qrc
)

IF(MAD_FOUND)
  include_directories(${MAD_INCLUDE_DIRS})
  link_directories(${MAD_LIBRARY_DIRS})
  ADD_DEFINITIONS(${MAD_CFLAGS} -DWITH_MAD)
  SET(libmpeg_SRCS ${libmpeg_SRCS} decoder_mad.cpp)
ENDIF(MAD_FOUND)

IF(MPG123_FOUND)
  include_directories(${MPG123_INCLUDE_DIRS})
  link_directories(${MPG123_LIBRARY_DIRS})
  ADD_DEFINITIONS(${MPG123_CFLAGS} -DWITH_MPG123)
  SET(libmpeg_SRCS ${libmpeg_SRCS} decoder_mpg123.cpp)
ENDIF(MPG123_FOUND)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(MAD_FOUND OR MPG123_FOUND)
ADD_LIBRARY(mpeg MODULE ${libmpeg_SRCS})
add_dependencies(mpeg libqmmp)
target_link_libraries(mpeg PRIVATE Qt6::Widgets libqmmp ${TAGLIB_LDFLAGS} ${MAD_LDFLAGS} ${MPG123_LDFLAGS} ${LIBRCD_LDFLAGS})
install(TARGETS mpeg DESTINATION ${PLUGIN_DIR}/Input)
ENDIF(MAD_FOUND OR MPG123_FOUND)
