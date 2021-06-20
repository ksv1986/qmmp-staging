project(libsid)

INCLUDE(CheckIncludeFile)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# libsidplayfp
pkg_search_module(SIDPLAYFP libsidplayfp>=1.0.3)

include_directories(${SIDPLAYFP_INCLUDE_DIRS})
link_directories(${SIDPLAYFP_LIBRARY_DIRS})
ADD_DEFINITIONS(${SIDPLAYFP_CFLAGS})

SET(libsid_SRCS
  decoder_sid.cpp
  decodersidfactory.cpp
  settingsdialog.cpp
  sidhelper.cpp
  settingsdialog.ui
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(SIDPLAYFP_FOUND)
ADD_LIBRARY(sid MODULE ${libsid_SRCS})
add_dependencies(sid libqmmp)
target_link_libraries(sid PRIVATE Qt6::Widgets libqmmp ${SIDPLAYFP_LDFLAGS})
install(TARGETS sid DESTINATION ${PLUGIN_DIR}/Input)
ENDIF(SIDPLAYFP_FOUND)
