project(libmodplug)

INCLUDE(CheckIncludeFile)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# modplug
pkg_search_module(MODPLUG libmodplug)

# add modplug defines
CHECK_INCLUDE_FILE(stdint.h STDINT_FOUND)
IF(STDINT_FOUND)
ADD_DEFINITIONS(-DHAVE_STDINT_H)
ENDIF(STDINT_FOUND)

CHECK_INCLUDE_FILE(inttypes.h INTTYPES_FOUND)
IF(INTTYPES_FOUND)
ADD_DEFINITIONS(-DHAVE_INTTYPES_H)
ENDIF(INTTYPES_FOUND)


include_directories(${MODPLUG_INCLUDE_DIRS})
link_directories(${MODPLUG_LIBRARY_DIRS})
ADD_DEFINITIONS(${MODPLUG_CFLAGS})

SET(libmodplug_SRCS
  decoder_modplug.cpp
  decodermodplugfactory.cpp
  modplugmetadatamodel.cpp
  settingsdialog.cpp
  archivereader.cpp
  settingsdialog.ui
  translations/translations.qrc
)


# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(MODPLUG_FOUND)
ADD_LIBRARY(modplug MODULE ${libmodplug_SRCS})
add_dependencies(modplug libqmmp)
target_link_libraries(modplug PRIVATE Qt6::Widgets libqmmp ${MODPLUG_LDFLAGS})
install(TARGETS modplug DESTINATION ${PLUGIN_DIR}/Input)
ENDIF(MODPLUG_FOUND)


