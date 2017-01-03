project(libshout)


if(COMMAND cmake_policy)
cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# qt plugin
ADD_DEFINITIONS(-Wall)
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_THREAD)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(QT_INCLUDES
  ${QT_INCLUDES}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../
)

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# libshout and other required libraries
pkg_check_modules(SHOUT ogg vorbis vorbisenc shout soxr)
include_directories(${SHOUT_INCLUDE_DIRS})
link_directories(${SHOUT_LIBRARY_DIRS})
ADD_DEFINITIONS(${SHOUT_CFLAGS})

SET(libshout_SRCS
  shoutoutput.cpp
  outputshoutfactory.cpp
  shoutclient.cpp
  settingsdialog.cpp
)

SET(libshout_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libshout_RCC_SRCS ${libshout_RCCS})

# user interface
SET(libshout_UIS
    settingsdialog.ui
)

QT4_WRAP_UI(libshout_UIS_H ${libshout_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(SHOUT_FOUND)
ADD_LIBRARY(shout MODULE ${libshout_SRCS} ${libshout_UIS_H} ${libshout_RCC_SRCS})
add_dependencies(shout qmmp)
target_link_libraries(shout ${QT_LIBRARIES} libqmmp ${SHOUT_LDFLAGS})
install(TARGETS shout DESTINATION ${LIB_DIR}/qmmp/Output)
ENDIF(SHOUT_FOUND)
