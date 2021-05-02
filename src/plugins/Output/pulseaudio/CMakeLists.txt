project(libpulseaudio)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# pulseaudio
pkg_check_modules(PULSE libpulse>=0.9.15)

include_directories(${PULSE_INCLUDE_DIRS})
link_directories(${PULSE_LIBRARY_DIRS})
ADD_DEFINITIONS(${PULSE_CFLAGS})

SET(libpulseaudio_SRCS
  outputpulseaudio.cpp
  outputpulseaudiofactory.cpp
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(PULSE_FOUND)
ADD_LIBRARY(pulseaudio MODULE ${libpulseaudio_SRCS})
add_dependencies(pulseaudio libqmmp)
target_link_libraries(pulseaudio PRIVATE Qt6::Widgets libqmmp ${PULSE_LDFLAGS})
install(TARGETS pulseaudio DESTINATION ${PLUGIN_DIR}/Output)
ENDIF(PULSE_FOUND)

