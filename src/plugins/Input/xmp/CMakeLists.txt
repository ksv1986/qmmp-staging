project(libxmp)

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# xmp
pkg_search_module(XMP libxmp)

include_directories(${XMP_INCLUDE_DIRS})
link_directories(${XMP_LIBRARY_DIRS})
ADD_DEFINITIONS(${XMP_CFLAGS})

SET(libxmp_SRCS
  decoder_xmp.cpp
  decoderxmpfactory.cpp
  xmpmetadatamodel.cpp
  settingsdialog.cpp
  xmpmetadatamodel.cpp
  settingsdialog.ui
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(XMP_FOUND)
ADD_LIBRARY(xmp MODULE ${libxmp_SRCS} ${libxmp_UIS_H})
add_dependencies(xmp libqmmp)
target_link_libraries(xmp PRIVATE Qt6::Widgets libqmmp ${XMP_LDFLAGS})
install(TARGETS xmp DESTINATION ${PLUGIN_DIR}/Input)
ENDIF(XMP_FOUND)
