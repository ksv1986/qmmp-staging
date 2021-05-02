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
    settingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(VORBISENC_FOUND)
    ADD_LIBRARY(filewriter MODULE ${libfilewriter_SRCS})
    add_dependencies(filewriter libqmmp libqmmpui)
    target_link_libraries(filewriter PRIVATE Qt6::Widgets libqmmp libqmmpui ${VORBISENC_LDFLAGS})
    install(TARGETS filewriter DESTINATION ${PLUGIN_DIR}/Effect)
ENDIF(VORBISENC_FOUND)
