project(libarchive)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# libarchive
pkg_search_module(LIBARCHIVE libarchive>=3.2.0)

include_directories(${archive_INCLUDE_DIRS} ${TAGLIB_INCLUDE_DIRS})
link_directories(${archive_LIBRARY_DIRS} ${TAGLIB_LIBRARY_DIRS})
ADD_DEFINITIONS(${archive_CFLAGS} ${TAGLIB_CFLAGS})

SET(libarchive_SRCS
  archiveinputdevice.cpp
  decoder_archive.cpp
  archivetagreader.cpp
  decoderarchivefactory.cpp
  translations/translations.qrc
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(LIBARCHIVE_FOUND)
ADD_LIBRARY(archive MODULE ${libarchive_SRCS})
add_dependencies(archive libqmmp)
target_link_libraries(archive PRIVATE Qt6::Widgets libqmmp ${LIBARCHIVE_LDFLAGS} ${TAGLIB_LDFLAGS})
install(TARGETS archive DESTINATION ${PLUGIN_DIR}/Input)
ENDIF(LIBARCHIVE_FOUND)
