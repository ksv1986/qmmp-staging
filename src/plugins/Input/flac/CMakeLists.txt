project(libflac)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# libflac
pkg_search_module(FLAC flac)

IF(NOT FLAC_LINK_FLAGS)
  SET(FLAC_LINK_FLAGS -lFLAC)
ENDIF(NOT FLAC_LINK_FLAGS)

include_directories(${FLAC_INCLUDE_DIRS} ${TAGLIB_INCLUDE_DIRS})
link_directories(${FLAC_LIBRARY_DIRS} ${TAGLIB_LIBRARY_DIRS})
ADD_DEFINITIONS(${FLAC_CFLAGS})
ADD_DEFINITIONS(${TAGLIB_CFLAGS})

SET(libflac_SRCS
  decoder_flac.cpp
  decoderflacfactory.cpp
  flacmetadatamodel.cpp
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(FLAC_FOUND)
ADD_LIBRARY(flac MODULE ${libflac_SRCS})
add_dependencies(flac libqmmp)
target_link_libraries(flac PRIVATE Qt6::Widgets libqmmp ${FLAC_LDFLAGS} ${TAGLIB_LDFLAGS})
install(TARGETS flac DESTINATION ${PLUGIN_DIR}/Input)
ENDIF(FLAC_FOUND)
