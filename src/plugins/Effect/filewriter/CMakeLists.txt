project(libfilewriter)


if(COMMAND cmake_policy)
cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# qt plugin
ADD_DEFINITIONS( -Wall )
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

pkg_check_modules(VORBISENC vorbis ogg vorbisenc)

include_directories(${VORBISENC_INCLUDE_DIRS})
link_directories(${VORBISENC_LIBRARY_DIRS})
ADD_DEFINITIONS(${VORBISENC_CFLAGS})

SET(libfilewriter_SRCS
    filewriterplugin.cpp
    settingsdialog.cpp
    effectfilewriterfactory.cpp
)

SET(libfilewriter_HDRS
    filewriterplugin.h
)

SET(libfilewriter_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libfilewriter_RCC_SRCS ${libfilewriter_RCCS})

# user interface

SET(libfilewriter_UIS
    settingsdialog.ui
)

QT4_WRAP_UI(libfilewriter_UIS_H ${libfilewriter_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(VORBISENC_FOUND)
ADD_LIBRARY(filewriter MODULE ${libfilewriter_SRCS} ${libfilewriter_UIS_H} ${libfilewriter_RCC_SRCS} ${libfilewriter_HDRS})
add_dependencies(filewriter qmmp)
target_link_libraries(filewriter ${QT_LIBRARIES} libqmmp ${VORBISENC_LDFLAGS})
install(TARGETS filewriter DESTINATION ${LIB_DIR}/qmmp/Effect)
ENDIF(VORBISENC_FOUND)
