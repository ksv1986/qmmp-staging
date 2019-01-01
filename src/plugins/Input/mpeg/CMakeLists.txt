project(libmpeg)

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


SET(libmpeg_SRCS
  decodermpegfactory.cpp
  settingsdialog.cpp
  tagextractor.cpp
  mpegmetadatamodel.cpp
)

SET(libmpeg_HDRS
  tagextractor.h
)

IF(MAD_FOUND)
  include_directories(${MAD_INCLUDE_DIRS})
  link_directories(${MAD_LIBRARY_DIRS})
  ADD_DEFINITIONS(${MAD_CFLAGS} -DWITH_MAD)
  SET(libmpeg_SRCS ${libmpeg_SRCS} decoder_mad.cpp)
  SET(libmpeg_HDRS ${libmpeg_HDRS} decoder_mad.h)
ENDIF(MAD_FOUND)

IF(MPG123_FOUND)
  include_directories(${MPG123_INCLUDE_DIRS})
  link_directories(${MPG123_LIBRARY_DIRS})
  ADD_DEFINITIONS(${MPG123_CFLAGS} -DWITH_MPG123)
  SET(libmpeg_SRCS ${libmpeg_SRCS} decoder_mpg123.cpp)
  SET(libmpeg_HDRS ${libmpeg_HDRS} decoder_mpg123.h)
ENDIF(MPG123_FOUND)

SET(libmpeg_RCCS translations/translations.qrc)

QT5_ADD_RESOURCES(libmpeg_RCC_SRCS ${libmpeg_RCCS})

# user interface

SET(libmpeg_UIS
    settingsdialog.ui
)

QT5_WRAP_UI(libmpeg_UIS_H ${libmpeg_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(MAD_FOUND OR MPG123_FOUND)
ADD_LIBRARY(mpeg MODULE ${libmpeg_SRCS} ${libmpeg_UIS_H} ${libmpeg_RCC_SRCS} ${libmpeg_HDRS})
add_dependencies(mpeg libqmmp)
target_link_libraries(mpeg Qt5::Widgets libqmmp ${TAGLIB_LDFLAGS} ${MAD_LDFLAGS} ${MPG123_LDFLAGS})
install(TARGETS mpeg DESTINATION ${PLUGIN_DIR}/Input)
ENDIF(MAD_FOUND OR MPG123_FOUND)
