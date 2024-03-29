project(libshout)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# libshout and other required libraries
pkg_check_modules(SHOUT ogg vorbis vorbisenc shout>=2.4.3 soxr)
include_directories(${SHOUT_INCLUDE_DIRS})
link_directories(${SHOUT_LIBRARY_DIRS})
ADD_DEFINITIONS(${SHOUT_CFLAGS})

SET(libshout_SRCS
  shoutoutput.cpp
  outputshoutfactory.cpp
  shoutclient.cpp
  settingsdialog.cpp
  settingsdialog.ui
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(SHOUT_FOUND)
ADD_LIBRARY(shout MODULE ${libshout_SRCS})
add_dependencies(shout libqmmp)
target_link_libraries(shout PRIVATE Qt6::Widgets libqmmp ${SHOUT_LDFLAGS})
install(TARGETS shout DESTINATION ${PLUGIN_DIR}/Output)
ENDIF(SHOUT_FOUND)
