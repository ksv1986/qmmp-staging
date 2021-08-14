project(libopus)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# libopus and libopusfile
pkg_check_modules(OPUS opus>=1.0.2 opusfile>=0.2)

include_directories(${OPUS_INCLUDE_DIRS}  ${TAGLIB_INCLUDE_DIRS})
link_directories(${OPUS_LIBRARY_DIRS} ${TAGLIB_LIBRARY_DIRS})
ADD_DEFINITIONS(${OPUS_CFLAGS} ${TAGLIB_CFLAGS})

SET(libopus_SRCS
  decoder_opus.cpp
  decoderopusfactory.cpp
  opusmetadatamodel.cpp
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(OPUS_FOUND)
ADD_LIBRARY(opus MODULE ${libopus_SRCS})
add_dependencies(opus libqmmp)
target_link_libraries(opus PRIVATE Qt6::Widgets libqmmp ${OPUS_LDFLAGS} ${TAGLIB_LDFLAGS})
install(TARGETS opus DESTINATION ${PLUGIN_DIR}/Input)
ENDIF(OPUS_FOUND)
