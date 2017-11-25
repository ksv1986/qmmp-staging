project(libfilewriter)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)
# libqmmpui
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)

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

QT5_ADD_RESOURCES(libfilewriter_RCC_SRCS ${libfilewriter_RCCS})

# user interface

SET(libfilewriter_UIS
    settingsdialog.ui
)

QT5_WRAP_UI(libfilewriter_UIS_H ${libfilewriter_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(VORBISENC_FOUND)
ADD_LIBRARY(filewriter MODULE ${libfilewriter_SRCS} ${libfilewriter_UIS_H} ${libfilewriter_RCC_SRCS} ${libfilewriter_HDRS})
add_dependencies(filewriter qmmp qmmpui)
target_link_libraries(filewriter Qt5::Widgets -lqmmp -lqmmpui ${VORBISENC_LDFLAGS})
install(TARGETS filewriter DESTINATION ${LIB_DIR}/qmmp/Effect)
ENDIF(VORBISENC_FOUND)
