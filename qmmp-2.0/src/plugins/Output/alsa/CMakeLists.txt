project(libalsa)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# alsa
pkg_search_module(ALSA alsa >= 1.0.22)
include_directories(${ALSA_INCLUDE_DIRS})
link_directories(${ALSA_LIBRARY_DIRS})
ADD_DEFINITIONS(${ALSA_CFLAGS})

SET(libalsa_SRCS
  outputalsa.cpp
  outputalsafactory.cpp
  settingsdialog.cpp
  settingsdialog.ui
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(ALSA_FOUND)
ADD_LIBRARY(alsa MODULE ${libalsa_SRCS})
add_dependencies(alsa libqmmp)
target_link_libraries(alsa PRIVATE Qt6::Widgets libqmmp ${ALSA_LDFLAGS})
install(TARGETS alsa DESTINATION ${PLUGIN_DIR}/Output)
ENDIF(ALSA_FOUND)

