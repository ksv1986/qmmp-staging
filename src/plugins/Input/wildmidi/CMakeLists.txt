project(libwildmidi)

INCLUDE(CheckIncludeFile)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)


SET(libwildmidi_SRCS
  decoder_wildmidi.cpp
  decoderwildmidifactory.cpp
  settingsdialog.cpp
  wildmidihelper.cpp
  settingsdialog.ui
  translations/translations.qrc
)

# libwildmidi
CHECK_INCLUDE_FILE(wildmidi_lib.h WILDMIDI_HEADER_FOUND)
FIND_LIBRARY(WILDMIDI_LIB NAME WildMidi PATHS /usr/lib /usr/lib32  /usr/local/lib /usr/local/lib32)

IF(WILDMIDI_HEADER_FOUND AND (NOT WILDMIDI_LIB MATCHES "^.*-NOTFOUND"))
SET(WILDMIDI_FOUND TRUE CACHE INTERNAL "wildmidi")
MESSAGE(STATUS "Found WildMidi: ${WILDMIDI_LIB}")
ELSE(WILDMIDI_HEADER_FOUND AND (NOT WILDMIDI_LIB MATCHES "^.*-NOTFOUND"))
MESSAGE(STATUS "Could not find WildMidi library")
ENDIF(WILDMIDI_HEADER_FOUND AND (NOT WILDMIDI_LIB MATCHES "^.*-NOTFOUND"))

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(WILDMIDI_FOUND)
ADD_LIBRARY(wildmidi MODULE ${libwildmidi_SRCS})
add_dependencies(wildmidi libqmmp)
target_link_libraries(wildmidi Qt6::Widgets libqmmp ${WILDMIDI_LIB})
install(TARGETS wildmidi DESTINATION ${PLUGIN_DIR}/Input)
ENDIF(WILDMIDI_FOUND)
