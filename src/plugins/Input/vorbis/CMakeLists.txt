project(libvorbis)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# libvorbis and taglib
pkg_check_modules(VORBIS ogg vorbis vorbisfile)

include_directories(${VORBIS_INCLUDE_DIRS}  ${TAGLIB_INCLUDE_DIRS})
link_directories(${VORBIS_LIBRARY_DIRS} ${TAGLIB_LIBRARY_DIRS})
ADD_DEFINITIONS(${VORBIS_CFLAGS} ${TAGLIB_CFLAGS})

SET(libvorbis_SRCS
  decoder_vorbis.cpp
  decodervorbisfactory.cpp
  vorbismetadatamodel.cpp
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(VORBIS_FOUND)
ADD_LIBRARY(vorbis MODULE ${libvorbis_SRCS})
add_dependencies(vorbis libqmmp)
target_link_libraries(vorbis PRIVATE Qt6::Widgets libqmmp ${VORBIS_LDFLAGS} ${TAGLIB_LDFLAGS})
install(TARGETS vorbis DESTINATION ${PLUGIN_DIR}/Input)
ENDIF(VORBIS_FOUND)
