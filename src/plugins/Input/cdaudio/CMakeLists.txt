project(libcdaudio)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# libcdaudio
pkg_check_modules(CDIO libcdio>=1.0.0 libcdio_cdda libcdio_paranoia libcddb)

include_directories(${CDIO_INCLUDE_DIRS})
link_directories(${CDIO_LIBRARY_DIRS})
ADD_DEFINITIONS(${CDIO_CFLAGS})

SET(libcdaudio_SRCS
  decoder_cdaudio.cpp
  decodercdaudiofactory.cpp
  settingsdialog.cpp
  settingsdialog.ui
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(CDIO_FOUND)
ADD_LIBRARY(cdaudio MODULE ${libcdaudio_SRCS})
add_dependencies(cdaudio libqmmp)
target_link_libraries(cdaudio PRIVATE Qt6::Widgets libqmmp ${CDIO_LDFLAGS})
install(TARGETS cdaudio DESTINATION ${PLUGIN_DIR}/Input)
ENDIF(CDIO_FOUND)
