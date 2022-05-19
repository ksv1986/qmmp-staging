project(libsndfile)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# libsndfile
pkg_search_module(SNDFILE sndfile >= 1.0.22)

include_directories(${SNDFILE_INCLUDE_DIRS})
link_directories(${SNDFILE_LIBRARY_DIRS})
ADD_DEFINITIONS(${SNDFILE_CFLAGS})

SET(libsndfile_SRCS
  decoder_sndfile.cpp
  decodersndfilefactory.cpp
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(SNDFILE_FOUND)
ADD_LIBRARY(sndfile MODULE ${libsndfile_SRCS})
add_dependencies(sndfile libqmmp)
target_link_libraries(sndfile PRIVATE Qt6::Widgets libqmmp ${SNDFILE_LDFLAGS})
install(TARGETS sndfile DESTINATION ${PLUGIN_DIR}/Input)
ENDIF(SNDFILE_FOUND)



