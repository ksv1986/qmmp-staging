project(libcue)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

IF(USE_ENCA AND ENCA_FOUND)
ADD_DEFINITIONS(-DWITH_ENCA)
include_directories(${ENCA_INCLUDE_DIRS})
link_directories(${ENCA_LIBRARY_DIRS})
ADD_DEFINITIONS(${ENCA_CFLAGS})
ENDIF(USE_ENCA AND ENCA_FOUND)

SET(libcue_SRCS
  decoder_cue.cpp
  decodercuefactory.cpp
  settingsdialog.cpp
  cuefile.cpp
  cuemetadatamodel.cpp
  translations/translations.qrc
  settingsdialog.ui
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(cue MODULE ${libcue_SRCS} ${libcue_RCC_SRCS})
add_dependencies(cue libqmmp)
IF(USE_ENCA AND ENCA_FOUND)
target_link_libraries(cue PRIVATE Qt6::Widgets libqmmp ${ENCA_LDFLAGS})
ELSE(USE_ENCA AND ENCA_FOUND)
target_link_libraries(cue PRIVATE Qt6::Widgets libqmmp)
ENDIF(USE_ENCA AND ENCA_FOUND)
install(TARGETS cue DESTINATION ${PLUGIN_DIR}/Input)
